//===----------------------------------------------------------------------===//
// INTEL CONFIDENTIAL
//
// Copyright 2022 Intel Corporation.
//
// This software and the related documents are Intel copyrighted materials, and
// your use of them is governed by the express license under which they were
// provided to you ("License"). Unless the License provides otherwise, you may
// not use, modify, copy, publish, distribute, disclose or transmit this
// software or the related documents without Intel's prior written permission.
//
// This software and the related documents are provided as is, with no express
// or implied warranties, other than those that are expressly stated in the
// License.
//===----------------------------------------------------------------------===//
//
// Quantum teleportation
//
// Quantum teleportation aims to send a quantum state from Alice to Bob using
// only classical communication. Alice and Bob start off each having half of a
// shared Bell pair, and by the end of the protocol, Bob's qubit is in a state
// corresponding to Alice's original qubit.
//
// The protocol uses three qubits:
//   1. AliceState is Alice's initial state
//   2. BellA is Alice's half of the Bell pair
//   3. BellB is Bob's half of the Bell pair
//
// The protocol proceeds in four steps: 
// 1. [alicePrepState] In the initialization step, Alice initializes her
//    qubit's state to the state |phi>. 
// 2. [aliceProtocol] Alice creates a bell pair entangles her qubit
//    (AliceState) with her half of the bell pair (BellA).
// 3. [measAlice] Alice measures her two qubits and sends the classical results
//    to Bob.
// 4. [xBob, zBob] Bob corrects his state by applying either an X and/or
//    Z gate based on those results. Bob's qubit will now be in state |phi>, up
//    to a global phase.
//===----------------------------------------------------------------------===//
//
// This implementation highlights the usage of QRT state reporting using the API
// methods get_probabilities, get_amplitudes, and get_samples.
//
//===----------------------------------------------------------------------===//
#include <clang/Quantum/quintrinsics.h>

/// Quantum Runtime Library APIs
#include <quantum.hpp>

#include <iostream>

/* Declare 3 qubits */
qbit AliceState;
qbit BellA;
qbit BellB;

/* Declare 3 measurement readouts */
cbit AliceBit1;
cbit AliceBit2;
cbit BobResult;

// Create a bell pair
quantum_kernel void prepBellPair(qbit q0, qbit q1) {
  PrepZ(q0);
  PrepZ(q1);
  H(q0);
  CNOT(q0, q1);
}


quantum_kernel void alicePrepState() {
  // Prep all
  PrepZ(AliceState);
  PrepZ(BellA);
  PrepZ(BellB);

  // Alice creates her quantum state--here an X rotation
  RX(AliceState, 1);
}

quantum_kernel void aliceProtocol() {
  // Create a bell pair
  prepBellPair(BellA, BellB);

  // Alice entangles her qubit with the first element of the Bell pair
  CNOT(AliceState, BellA);
  H(AliceState);
}

// Alice measures her two bits and sends them to Bob.
quantum_kernel void measAlice() {
  MeasZ(AliceState, AliceBit1);
  MeasZ(BellA, AliceBit2);
}


// Corrections are invoked from the top level, so they must be wrapped in
// individual quantum_kernels
quantum_kernel void xBob() {
  X(BellB);
}
quantum_kernel void zBob() {
  Z(BellB);
}


// Alternatively, push the corrections into a quantum kernel with CX and CZ
// operations.
quantum_kernel void bobCorrections() {
  CNOT(BellA, BellB);
  CZ(AliceState, BellB);
}
quantum_kernel void teleportCircuit() {
  alicePrepState();
  aliceProtocol();
  measAlice();
  bobCorrections();
}


int main() {
  using namespace iqsdk;

  /*
                      alicePrepState()        aliceProtocol()         measAlice()     corrections
                    ===================    =====================     =============    =============
    |AliceState> : -- PrepZ --- RX -------------------- o --- H -------- MeasZ ----------------- o ----
                                                        |                                        |
    |BellA>      : -- PrepZ --------------- H --- o --- X -------------- MeasZ ------- o ------- | ----
                                                  |                                    |         |
    |BellB>      : -- PrepZ --------------------- X ---------------------------------- X ------- Z ----
  */

  // Set up the simulator
  IqsConfig iqs_config(/*num_qubits*/ 3,
                       /*simulation_type*/ "noiseless");
  FullStateSimulator iqs_device(iqs_config);
  if (QRT_ERROR_SUCCESS != iqs_device.ready()) {
    std::printf("Device not ready\n");
    return 1;
  }

  // Structures for reporting probabilities and amplitudes.
  std::vector<std::reference_wrapper<qbit>> qids = {
      std::ref(AliceState), std::ref(BellA), std::ref(BellB)};
  QssMap<double> probability_map;
  QssMap<std::complex<double>> amplitude_map;

  // Alice preps her state
  alicePrepState();

  std::cout << "\n==Alice's state before teleportation==\n";
  // Only display non-zero amplitudes above 0.01
  amplitude_map = iqs_device.getAmplitudes(qids, {}, 0.01);
  FullStateSimulator::displayAmplitudes(amplitude_map);


  std::cout << "\n==Alice's corresponding probability distribution==\n";
  std::vector<std::reference_wrapper<qbit>> alice_qid_ref = {
      std::ref(AliceState)};
  // By specifying only AliceState for the qid argument, we get out the
  // conditional probability of alice's state being in |0> or |1> respectively
  probability_map = iqs_device.getProbabilities(alice_qid_ref, {});
  FullStateSimulator::displayProbabilities(probability_map);

  aliceProtocol();

  // Before measurement, qubits are in superposition
  std::cout << "\n==Alice's state before measAlice()==\n";
  // Only display non-zero amplitudes above 0.01
  amplitude_map = iqs_device.getAmplitudes(qids, {}, 0.01);
  FullStateSimulator::displayAmplitudes(amplitude_map);

  measAlice();

  std::cout << "\n==Bob's state immediately after measurement==\n";
  // When we look at the state vector, we see that Alice's two qubits
  // have in fact been measured, and have collapsed so there are only two
  // non-zero amplitudes.
  amplitude_map = iqs_device.getAmplitudes(qids, {}, 0.01/*threshold--only display non-zero amplitudes*/);
  FullStateSimulator::displayAmplitudes(amplitude_map);

  // If we look at the conditional probability associated with bob's qubit
  // (BellB), we see that it now has the almost same distribution as Alice's
  // original prepared state, but with a possibility of the result being
  // swapped.
  std::cout << "\n==Bob's corresponding probability distribution==\n";
  std::vector<std::reference_wrapper<qbit>> bob_qid_ref
    = {std::ref(BellB)};
  probability_map = iqs_device.getProbabilities(bob_qid_ref, {});
  FullStateSimulator::displayProbabilities(probability_map);


  // Now we apply corrections, using the results of alice's measurement to
  // decide on which corrections to apply.
  if (AliceBit2) {
    std::cout << "Applying X\n";
    xBob();
  }
  if (AliceBit1) {
    std::cout << "Applying Z\n";
    zBob();
  }

  // After applying these corrections, we can project out the relevant
  // amplitudes and should get the same result as Alice's original prepared
  // state, up to a global phase
  std::cout << "\n==Bob's state after corrections have been applied==\n";
  // `bases` will expand into |AliceBit1 AliceBit2 0> and |AliceBit1 AliceBit2 1>
  std::vector<int> pattern = {AliceBit1, AliceBit2, QssIndex::Wildcard};
  std::vector<QssIndex> bases = QssIndex::patternToIndices(pattern);
  amplitude_map = iqs_device.getAmplitudes(qids, bases);
  FullStateSimulator::displayAmplitudes(amplitude_map);

  // The resulting probability distribution should be equivalent to Alice's
  // initial probability distribution
  std::cout << "\n==Bob's corresponding probability distribution==\n";
  probability_map = iqs_device.getProbabilities(bob_qid_ref, {});
  FullStateSimulator::displayProbabilities(probability_map);

  return 0;
}
