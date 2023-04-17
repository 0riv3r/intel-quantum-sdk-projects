/*
   Swap

   page #23 in the "Quantum Computation and Quantum Information" book.

   Swaps the states of the two qubits.
   The sequence of the gates has the following sequence of effects on a computational basis state
   |a, b〉, where all additions are done modulo 2. 
   The effect of the circuit, therefore, is to interchange the state of the two qubits.

   |a, b〉 −→ |a, a ⊕ b〉
      −→ |a ⊕ (a ⊕ b), a ⊕ b〉 = |b, a ⊕ b〉
      −→ |b, (a ⊕ b) ⊕ b〉 = |b, a〉

    |1⟩ ----●---⊕---●---
            |   |    |
    |0⟩ ----⊕---●---⊕--


   Run sample:
   -----------
   |1, 0〉 −→ |1, 1 ⊕ 0〉= |1, 1〉
      −→ |1 ⊕ (1 ⊕ 1), 1 ⊕ 1〉 = |1, 1 ⊕ 1〉= |1, 0〉
      −→ |0, (1 ⊕ 0) ⊕ 0〉 = |0, 1〉
         
         
   --|1⟩--●---|1⟩--⊕--|0⟩--●---|0⟩
          |        |       |
   --|0⟩--⊕--|1⟩---●--|1⟩--⊕--|1⟩

*/

/* Gate definitions and keywords */
#include <clang/Quantum/quintrinsics.h>
/* Quantum Runtime Library APIs */
#include <quantum.hpp>
#include <iostream>

/* 
   Declare 2 qubits 
*/
qbit q[2];

/* 
   swaps the states of the two qubits.
*/
quantum_kernel void swap_qbits() {
   /* Prepare both qubits in the |0> state */
   PrepZ(q[0]);
   PrepZ(q[1]);

   /*
      Apply a Not gate to qubit-0, 
      so it changes to be in |1> state.
   */
   X(q[0]);

   /*
      At this point, the qubits sates are:
      q[0] is |1>
      q[1] is |0>
   */

   /*  
      1)
      Apply a Controlled-NOT (CNOT) gate with:
      Control -> q[0]
      Target  -> q[1]
   */
   CNOT(q[0], q[1]);

   /*  
      2)
      Apply a CNOT gate with:
      Control -> q[1]
      Target  -> q[0]
   */
   CNOT(q[1], q[0]);

   /*  
      3)
      Apply a CNOT gate with:
      Control -> q[0]
      Target  -> q[1]
   */
   CNOT(q[0], q[1]);

   /*
      At this point, the qubits sates should be:
      q[0] is |0>
      q[1] is |1>
   */
}

/* 
   Measure the qubits.
   Input 2 cbit variables addresses,
   to accept the measurement readouts.
*/
quantum_kernel void qubit_measurement(cbit &q_zero_read_out, cbit &q_one_read_out) {
   /* Measure qubit-0 */
   MeasZ(q[0], q_zero_read_out);
   /* Measure qubit-1 */
   MeasZ(q[1], q_one_read_out);
}

int main() {
   /* Configure the runtime env. */
   iqsdk::IqsConfig settings(2, "noiseless");
   iqsdk::FullStateSimulator quantum_8086(settings);
   if (iqsdk::QRT_ERROR_SUCCESS != quantum_8086.ready()) return 1;

   /* 
      Declare 2 measurement readouts.
      Measurements are stored here as "classical bits".
   */
   cbit q_zero_classical_bit;
   cbit q_one_classical_bit;

   /*
      Initial qubits states
   */
   std::cout << "Initial qubits states:" << std::endl;
   std::cout << "q[0] is |1>" << std::endl;
   std::cout << "q[1] is |0>" << std::endl;
   std::cout << "" << std::endl;

   /* Prepare the phi-plus Bell-state. */
   swap_qbits();
   /* Measure the qubits. */
   qubit_measurement(q_zero_classical_bit, q_one_classical_bit);

   /* 
      Write classical logic that interacts with the measurement results.
      Convert the cbit type to bool type.
   */
   bool q_zero_result = q_zero_classical_bit;
   bool q_one_result = q_one_classical_bit;

   std::cout << "Qubits states after swap function:" << std::endl;
   std::cout << "q[0] is |";
   std::cout << std::to_string(q_zero_result);
   std::cout << ">" << std::endl;
   std::cout << "q[1] is |";
   std::cout << std::to_string(q_one_result);
   std::cout << ">" << std::endl;

   return 0;
}
