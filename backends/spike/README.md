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
