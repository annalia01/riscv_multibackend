# Installing Spike (RISC-V ISA Simulator)

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
