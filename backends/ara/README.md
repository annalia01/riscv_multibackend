## Toolchain

Ara requires a RISC-V LLVM toolchain capable of understanding the vector extension, version 1.0.

To build this toolchain, run the following command in the project's root directory.

```bash
# Build the LLVM toolchain
make toolchain-llvm
```

## Verilator

Ara requires an updated version of Verilator, for RTL simulations.

To build it, run the following command in the project's root directory.

```bash
# Build Verilator
make verilator
```

## RTL Simulation

### Hardware dependencies

The Ara repository depends on external IPs and uses Bender to handle the IP dependencies.
To install Bender and initialize all the hardware IPs, run the following commands:

```bash
make checkout
```

### Patches (only once!)

Note: this step is required only once, and needs to be repeated ONLY if the IP hardware dependencies are deleted and checked out again.

Some of the IPs need to be patched to work with Verilator.

```bash
# Apply the patches (only need to run this once)
make apply-patches
```

### Compile hardware

#Only compile the hardware without running the simulation.
make verilate
