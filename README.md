# RISC-V MULTI-BACKEND

This repository provides a modular simulation environment for evaluating the performance of RISC-V vector code across different backends.  
It integrates multiple simulation targets — functional, architectural, and RTL — enabling seamless compilation and execution of the same benchmark on each backend.

The goal of this framework is to **analyze the performance, scalability, and correctness of RISC-V Vector (RVV) applications** under different simulation levels.  
Each backend (e.g., Spike, Gem5, Ara) can be independently built and configured, allowing flexible experimentation and comparison.

## RISC-V GNU Toolchain

To build the applications that will be executed on Spike and gem5, a RISC-V GNU toolchain is required.

```bash
#Download sources directory
export DOWNLOAD_DIR=$(pwd)/downloads

#Installation directory
export INSTALL_DIR=$HOME/RISC-V/rv64

#Create directories
mkdir -p $DOWNLOAD_DIR
mkdir -p $INSTALL_DIR

#Clone the RISC-V GNU toolchain
cd $DOWNLOAD_DIR
git clone https://github.com/riscv/riscv-gnu-toolchain.git --depth 1
cd riscv-gnu-toolchain

#Initialize required submodules
git submodule update --init --recursive --depth 1 binutils gcc glibc dejagnu newlib gdb

#Build and install
mkdir build && cd build
../configure --prefix=$INSTALL_DIR/gnu-toolchain \
             --with-arch=rv64gcv_zicsr_zifencei \
             --with-abi=lp64

make -j$(nproc)
make install
```
After the installation is complete, export the toolchain binaries to your system `PATH` so they can be accessed from anywhere:

```bash
export PATH=$HOME/RISC-V/rv64/gnu-toolchain/bin:$PATH
```

## SPIKE SETUP

For the Spike backend setup, go to the `backends/spike/` directory and follow the installation instructions provided there. 
All commands must be executed within that directory.
Once Spike is installed, all applications can be compiled and executed from the root directory of the project.

## GEM5 SETUP

For the gem5 backend setup, go to the backends/gem5/ directory and follow the installation instructions provided there.
All commands must be executed within that directory.
Once gem5 is installed, all applications can be compiled and executed from the root directory of the project.

## VERILATOR SETUP 

For the verilator backend setup, go to the backends/ara/ directory and follow the installation instructions provided there.
All commands must be executed within that directory.
Once verilator set up is finished, all applications can be compiled and executed from the root directory of the project.
