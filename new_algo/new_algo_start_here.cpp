//===----------------------------------------------------------------------===//
// INTEL CONFIDENTIAL
//
// Copyright 2021-2022 Intel Corporation.
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

/// Quantum intrinsic function definitions
#include <clang/Quantum/quintrinsics.h>

/// Quantum Runtime Library APIs
#include <quantum.hpp>

/// Define our quantum resources
const int N = 3; // handle for number of qubits
qbit q[N];       // register of qubits to operate on
cbit c[N];       // register of measurement readouts

/// Can define additional arrays or variables here, must be global
/// For example, 'qbit ancilla[3];'

/// Define resources to pass numerical values from the classical functions to
/// the quantum_kernel
/// This is only required for quantum_kernels that contain algorithms that
/// require dynamic parameters.
double param[2];


/// prepare each of the qubits in q
quantum_kernel void prepare_all()
{
    for (int i = 0; i < N; i++)
    {
        PrepZ(q[i]); // 
    }
}

/// measure each qubit in q and store each measurement outcome in c
quantum_kernel void measure_all()
{
    for (int i = 0; i < N; i++)
    {
        MeasZ(q[i], c[i]);
    }
}

/// Define custom operations using the built-in quantum gates
quantum_kernel void custom_operation(qbit q1, qbit q2)
{
    /// Add quantum logic operations here
    RX(q1, param[0]);
    RX(q2, param[1]);
}

quantum_kernel void test_function()
{
    /// Add logic here
    custom_operation(q[0], q[1]);
}

int main()
{
    /// Setup quantum device
    iqsdk::IqsConfig iqs_config(/*num_qubits*/ N,
                                 /*simulation_type*/ "noiseless");
    iqsdk::FullStateSimulator iqs_device(iqs_config);
    if (iqsdk::QRT_ERROR_SUCCESS != iqs_device.ready()) {
      return 1;
    }
    prepare_all();

    param[0] =  15.0;
    param[1] = 	30.0;

    test_function();
    measure_all();

    for (int i = 0; i < N; i++) {
      std::cout << "qubit " << i << " - " << (bool) c[i] << std::endl;
    }

    return 0;
}
