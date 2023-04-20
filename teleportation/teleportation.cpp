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

/* Declare 3 qubits */
qbit q[3];

/* 
   Prepare an entangled two qubits state, 
   the phi-plus Bell-state, 
   from the |00〉basis.
*/
quantum_kernel void prepare_bell_phi_plus() {
    /* Prepare both of Alice qubits in the |0> state */
    PrepZ(q[1]);
    PrepZ(q[2]);
    /*  Apply a Hadamard gate to qubit-1 */
    H(q[1]);
    /*  
       Apply a Controlled-NOT (CNOT) gate, 
       with qubit-1 as the control, 
       and qubit-2 as the target. 
    */
    CNOT(q[1], q[2]);
}

quantum_kernel void teleportation() {
    /* 
      Prepare a third qubit, psi, in the |0> state, 
      that holds the data to teleport.
    */
    PrepZ(q[0]);

    /*  
       Apply a Controlled-NOT (CNOT) gate, 
       with qubit-0 (psi) as the control, 
       and qubit-1 as the target. 
    */
    CNOT(q[0], q[1]);

    /*  Apply a Hadamard gate to qubit-0 */
    H(q[0]);
}

/* 
   Measure the qubits.
   Input 3 cbit variables addresses,
   to accept the measurement readouts.
*/
quantum_kernel void qubit_measurement(cbit &psi, cbit &alice_entangled_1, cbit &bob_entangled_2) {
    /* Measure qubit-0 */
    MeasZ(q[0], psi);
    /* Measure qubit-1 */
    MeasZ(q[1], alice_entangled_1);
    /* Measure qubit-2 */
    MeasZ(q[2], bob_entangled_2);
}

int main() {
    /* Configure the runtime env. */
    iqsdk::IqsConfig settings(2, "noiseless");
    iqsdk::FullStateSimulator quantum_8086(settings);
    if (iqsdk::QRT_ERROR_SUCCESS != quantum_8086.ready()) return 1;

    /* 
       Declare 3 measurement readouts.
       Measurements are stored here as "classical bits".
    */
    cbit c[3];

    /* Prepare the phi-plus Bell-state. */
    prepare_bell_phi_plus();

    teleportation();

    /* Measure the qubits. */
    qubit_measurement(c[0], c[1], c[2]);

    /* 
       Write classical logic that interacts with the measurement results.
       Convert the cbit type to bool type.
    */
    bool psi_result = c[0];
    bool alice_entangled_1 = c[1];
    bool bob_entangled_2 = c[2];

    std::cout << "The psi Qubit is: ";
    std::cout << std::to_string(psi_result) << std::endl;
    std::cout << "The entangled_1 Qubit is: ";
    std::cout << std::to_string(alice_entangled_1) << std::endl;
    std::cout << "The entangled_2 Qubit is: ";
    std::cout << std::to_string(bob_entangled_2) << std::endl;

    return 0;
}
