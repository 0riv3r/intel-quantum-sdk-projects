/*
    Bell state

    page #5 in the Intel Quantum SDK pdf file

    https://en.wikipedia.org/wiki/Bell_state

    The Bell's states, or EPR pairs, are specific quantum states of two qubits 
    that represent the simplest (and maximal) examples of quantum entanglement.

    The Bell's states are a form of entangled and normalized basis vectors. 
    This normalization implies that the overall probability of the particle,
    being in one of the mentioned states is 1: ⟨Φ|Φ⟩=1
    Entanglement is a basis-independent result of superposition.
    Due to this superposition, measurement of the qubit will "collapse" it into 
    one of its basis states with a given probability.
    Because of the entanglement, measurement of one qubit will "collapse" the other 
    qubit to a state whose measurement will yield one of two possible values, 
    where the value depends on which Bell's state the two qubits are in initially. 
    
    Bell's states can be generalized to certain quantum states of multi-qubit systems, 
    such as the GHZ state for 3 or more subsystems.

    |0⟩ ---[H]--o---
                |
    |0⟩ --------⊕---

*/

/* Gate definitions and key words */
#include <clang/Quantum/quintrinsics.h>
/* Quantum Runtime Library APIs */
#include <quantum.hpp>
#include <iostream>
/* Declare 2 qubits */
qbit q[2];
/* The quantum logic must be in a function with the keyword quantum_kernel */
/* pre-pended to the signature */
quantum_kernel void prepare_bell_phi_plus() {
    /* Prepare both qubits in the |0> state */
    PrepZ(q[0]);
    PrepZ(q[1]);
    /* Apply a Hadamard gate to the top qubit */
    H(q[0]);
    /* Apply a Controlled-NOT (CNOT) gate with the top qubit as
    * the control and the bottom qubit as the target */
    CNOT(q[0], q[1]);
    /* Measure qubit 0 */
    // MeasZ(q[0], control_read_out);
    // /* Measure qubit 1 */
    // MeasZ(q[1], target_read_out);
}

quantum_kernel void qubit_measurement(cbit &control_read_out, cbit &target_read_out) {
    /* Measure qubit 0 */
    MeasZ(q[0], control_read_out);
    /* Measure qubit 1 */
    MeasZ(q[1], target_read_out);
}

int main() {
    /* Configure the setting of the simulator. */
    iqsdk::IqsConfig settings(2, "noiseless");
    iqsdk::FullStateSimulator quantum_8086(settings);
    if (iqsdk::QRT_ERROR_SUCCESS != quantum_8086.ready()) return 1;
    /* Declare 2 measurement readouts */
    /* Measurements are stored here as "classical bits" */
    cbit control_classical_bit;
    cbit target_classical_bit;
    prepare_bell_phi_plus();
    qubit_measurement(control_classical_bit, target_classical_bit);
    /* Here we can use the FullStateSimulator APIs to get data */
    /* or we can write classical logic that interacts with our measurement */
    /* results, as below. */
    bool control_result = control_classical_bit;
    bool target_result = target_classical_bit;

    std::cout << "The Control Qubit is: ";
    if (control_result) {
        std::cout << "1\n";
    }
    else {
        std::cout << "0\n";
    }

    std::cout << "The Target Qubit is: ";
    if (target_result) {
        std::cout << "1\n";
    }
    else {
        std::cout << "0\n";
    }
    return 0;
}
