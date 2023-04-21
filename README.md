## SDK README
Launch the SDK README at:
        /glob/development-tools/intel-quantum-sdk/README

Examples at:
        /glob/development-tools/intel-quantum-sdk/quantum-examples/

## SDK Version
        u181733@login-2:~/workspace$ /glob/development-tools/intel-quantum-sdk/intel-quantum-compiler -h

## Run a job
        # to run job, run in the devcloud machine:
        u181733@login-2:~/workspace$ qsub qrng_job
        # to view the job status:
        u181733@login-2:~/workspace$ qstat

### How much memory:

memory = 2^(4+qubits) bytes

Memory Requirements
qubits   |  memory
  10     |   17 KB
  20     |   17 MB
  30     |   17.2 GB

        $ grep MemTotal /proc/meminfo
        MemTotal:       16037092 kB

Can use max of 29 qubits in this system (or 2^33 bytes, which is 8.6 GB)

## Copy files from devcloud machine to my local machine
        # while connected to devcloud machine via ssh
        # in a local terminal run:
        $ scp devcloud:/home/u181733/workspace/qrng.cpp .
        $ scp -r devcloud:/home/u181733/workspace .
