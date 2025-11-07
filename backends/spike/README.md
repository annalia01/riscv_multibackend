# Installing Spike 

This guide explains how to manually install **Spike**, the RISC-V ISA simulator, including a **patched version** that supports configurable **VLEN** values for vector extension experiments.

---

## 🧰 Requirements

Make sure the following packages are installed before proceeding:

```bash
sudo apt update
sudo apt install git build-essential autoconf automake libtool pkg-config \
    libboost-dev libboost-system-dev libboost-filesystem-dev device-tree-compiler
```
## Installation 

Clone and build Spike as following:

```bash
cd backends/spike
git clone https://github.com/riscv-software-src/riscv-isa-sim.git
cd riscv-isa-sim
git checkout 5cf439b
```
Apply the Ara patch, which extends Spike with the `--varch` option to allow dynamic configuration
of vector parameters such as `VLEN` (vector length):

```bash
git apply ../patches/0003-riscv-isa-sim-patch
```
## Build Spike
```bash
rm -rf build
mkdir build && cd build

#Clone and build the Device Tree Compiler (DTC)
[ -d dtc ] || git clone https://git.kernel.org/pub/scm/utils/dtc/dtc.git
cd dtc
git checkout b6910bec11614980a21e46fbccc35934b671bd81
make
cd ..

#Configure and build Spike
../configure --prefix=$(pwd)/../../../install/riscv-isa-sim-mod \
             --without-boost --without-boost-asio --without-boost-regex
make -j$(nproc)
```
# Installing Proxy Kernel 

Spike requires the Proxy Kernel (pk) to run bare-metal RISC-V programs. Follow the steps below to install it.

```bash
# Download sources directory
export DOWNLOAD_DIR=$(pwd)/downloads

# Installation directory
export INSTALL_DIR=$HOME/RISC-V/rv64

# Create directories
mkdir -p $DOWNLOAD_DIR
mkdir -p $INSTALL_DIR

# Clone the riscv-pk repository
cd $DOWNLOAD_DIR
git clone https://github.com/riscv/riscv-pk.git --depth 1
cd riscv-pk

# Build and install pk
mkdir build && cd build

# Add the RISC-V GCC toolchain to PATH
export PATH=$INSTALL_DIR/gnu-toolchain/bin:$PATH

# Configure and compile (zifencei_zicsr are necessary for GCC>=12)
../configure --prefix=$INSTALL_DIR/pk \
    --host=riscv64-unknown-elf \
    --with-arch=rv64gcv_zifencei_zicsr

make
make install

```
