# GEM5 INSTALLATION

This guide provides the steps required to install and configure the Gem5 backend used in the multi-backend simulation framework.  
Gem5 is an architectural simulator that enables detailed performance analysis of RISC-V applications at the microarchitectural level.  
Follow the instructions below to correctly set up Gem5 before running applications from the project root.

## Clone the gem5 repository 

```bash
git clone https://github.com/gem5/gem5.git
```

## Build gem5 for RISC-V
```bash
cd gem5
scons build/RISCV/gem5.opt -j$(nproc)
```


