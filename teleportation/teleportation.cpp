/*
   Quantum Teleportation

   page #26 in the "Quantum Computation and Quantum Information" book.

    

   |0⟩ ---[H]--o---
                |
   |0⟩ --------⊕---

*/

/* Gate definitions and keywords */
#include <clang/Quantum/quintrinsics.h>
/* Quantum Runtime Library APIs */
#include <quantum.hpp>

#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>

/* Declare 3 qubits */
qbit q_state_data;
qbit q_alice_entangled;
qbit q_bob_entangled;

/* 
   Declare 3 measurement readouts 
   Measurements are stored here as "classical bits".
*/
cbit c_state_data;
cbit c_alice_entangled;
cbit c_bob_entangled;

/* 
   initialize all the qubits.
   Prepare the qubits in the |0> state
*/
quantum_kernel void qubits_initialize() {
   PrepZ(q_state_data);
   PrepZ(q_alice_entangled);
   PrepZ(q_bob_entangled);
}

/* 
   Prepare an entangled two qubits state, 
   the phi-plus Bell-state, 
   from the |00〉basis.
*/
quantum_kernel void prepare_bell_phi_plus() {
   /*  Apply a Hadamard gate to q_state_data */
   H(q_alice_entangled);
   /*  
      Apply a Controlled-NOT (CNOT) gate, 
      with qubit q_state_data as the control, 
      and qubit q_alice_entangled as the target. 
   */
   CNOT(q_alice_entangled, q_bob_entangled);
}

/* 
   Prepare a third qubit with state data
   to be teleported.
*/
quantum_kernel void prepare_data_qbit() {
   /*
      Set the qubit quantum state using an X rotation gate
      with PI/2 angle.
   */
   RX(q_state_data, 1);
   //RX(q_state_data, M_PI_2);
}

quantum_kernel void teleportation_sender() {
   /*  
      Apply a Controlled-NOT (CNOT) gate, 
      with the state-data qubit (q_state_data) as the control, 
      and the sender's (Alice) entangled qubit (q_alice_entangled) as the target. 
   */
   CNOT(q_state_data, q_alice_entangled);

   /*  Apply a Hadamard gate to the state-data qubit (q_data) */
   H(q_state_data);
}

// Alice measures her two qbits.
quantum_kernel void measure_alice_qubits() {
  MeasZ(q_state_data, c_state_data);
  MeasZ(q_alice_entangled, c_alice_entangled);
}

// Corrections are invoked from the top level, so they must be wrapped in
// individual quantum_kernels
quantum_kernel void x_bob_entangled() {
  X(q_bob_entangled);
}
quantum_kernel void z_bob_entangled() {
  Z(q_bob_entangled);
}

int main() {
   using namespace iqsdk;

   /* Configure the simulator */
   IqsConfig iqs_config(/*num_qubits*/ 3,
                       /*simulation_type*/ "noiseless");
   FullStateSimulator iqs_device(iqs_config);
   if (QRT_ERROR_SUCCESS != iqs_device.ready()) {
      std::printf("Device not ready\n");
      return 1;
   }

   // Structures for reporting probabilities and amplitudes.
   std::vector<std::reference_wrapper<qbit>> qids = {
         std::ref(q_state_data), std::ref(q_alice_entangled), std::ref(q_bob_entangled)};
   QssMap<double> probability_map;
   QssMap<std::complex<double>> amplitude_map;

   qubits_initialize();
   prepare_data_qbit();

   std::cout << "\nThe data qubit state before teleportation:\n";
   // Only display non-zero amplitudes above 0.01
   amplitude_map = iqs_device.getAmplitudes(qids, {}, 0.01);
   FullStateSimulator::displayAmplitudes(amplitude_map);

   prepare_bell_phi_plus();
   teleportation_sender();

   /* Measure alice's qubits. */
   measure_alice_qubits();

   std::cout << "\nBob's state immediately after Alice's measurement:\n";
   // When we look at the state vector, we see that Alice's two qubits
   // have in fact been measured, and have collapsed so there are only two
   // non-zero amplitudes.
   amplitude_map = iqs_device.getAmplitudes(qids, {}, 0.01/*threshold--only display non-zero amplitudes*/);
   FullStateSimulator::displayAmplitudes(amplitude_map);
   
   // Now we apply corrections, using the results of alice's measurement to
   // decide on which corrections to apply.
   if (c_alice_entangled) {
      std::cout << "Applying X\n";
      x_bob_entangled();
   }
   if (c_state_data) {
      std::cout << "Applying Z\n";
      z_bob_entangled();
   }

   // After applying these corrections, we can project out the relevant
   // amplitudes and should get the same result as Alice's original prepared
   // state, up to a global phase
   std::cout << "\nBob's state after corrections have been applied:\n";
   // `bases` will expand into |c_state_data c_alice_entangled 0> and |c_state_data c_alice_entangled 1>
   std::vector<int> pattern = {c_state_data, c_alice_entangled, QssIndex::Wildcard};
   std::vector<QssIndex> bases = QssIndex::patternToIndices(pattern);
   amplitude_map = iqs_device.getAmplitudes(qids, bases);
   FullStateSimulator::displayAmplitudes(amplitude_map);

   return 0;
}
