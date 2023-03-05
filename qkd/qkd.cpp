/* Gate definitions and key words */
#include <clang/Quantum/quintrinsics.h>

/* Quantum Runtime Library APIs */
#include <quantum.hpp>

#include <iostream>

/// Define the quantum resources
/* Declare qubits */
const int total_qubits = 6;         // handle for number of qubits
qbit qubit_register[total_qubits];  // register of qubits to operate on

/* Declare measurement readouts */
/* Measurements are stored here as "classical bits" */
cbit cbit_register[total_qubits];   // register of measurement readouts

/* The quantum logic must be in a function with the keyword quantum_kernel */
/* pre-pended to the signature */

/// prepare each of the qubits in q
quantum_kernel void prepare_all() {
    for (int i = 0; i < total_qubits; i++) {
        PrepZ(qubit_register[i]);
    }
}

/// Put each qubit to superposition
/// Each qubit now has a 50% chance of being measured 0 or 1
quantum_kernel void superposition_all()
{
    for (int i = 0; i < total_qubits; i++) {
        H(qubit_register[i]);
    }
}

/// measure each qubit in q and store each measurement outcome in c
quantum_kernel void measure_all()
{
    for (int i = 0; i < total_qubits; i++)
    {
      MeasZ(qubit_register[i], cbit_register[i]);
    }
}

int main()
{
    /// Setup quantum device
    iqsdk::IqsConfig IqsConfig(/*num_qubits*/ total_qubits,
                                 /*simulation_type*/ "noiseless");
    
    iqsdk::FullStateSimulator iqs_device(IqsConfig);
    if (iqsdk::QRT_ERROR_SUCCESS != iqs_device.ready()) {
      return 1;
    }
    prepare_all();
    superposition_all();
    measure_all();

    std::cout << "------- qubit QRNG state -------" << std::endl;
    /* Here we can use the FullStateSimulator APIs to get data */
    /* or we can write classical logic that interacts with our measurement */
    /* results, as below. */
    for (int i = 0; i < total_qubits; i++)
    {
        bool result = cbit_register[i];
        if (result) {
            std::cout << "1\n";
        }
        else {
            std::cout << "0\n";
        }
    }
    std::cout << "------- --------------- -------" << std::endl;

    return 0;
}

