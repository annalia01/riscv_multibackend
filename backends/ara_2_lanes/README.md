## Get Started

Before building or running any kernel on hardware, the Simply-V environment must be initialized.
Every time you open a new terminal, you must source the Simply-V environment script:

```bash
cd Simply-V
source settings.sh hpc
cd ..
```

The setup is automated in the Makefile through the following target:

```bash
make prepare_simplyv
```

Running make prepare_simplyv performs all required steps inside the Simply-V directory:
  1. Applies the ARA configuration patch
    - Ensures that the hardware/software configuration matches the ARA2 environment.

  2. Builds the Simply-V hardware
    - Compiles all hardware components of the SoC.

  3. Programs the FPGA bitstream
    - Flashes the newly built hardware design onto the FPGA board, making the system ready to execute kernels.

To build the libraries, use the following target:

```bash
make sw
```

