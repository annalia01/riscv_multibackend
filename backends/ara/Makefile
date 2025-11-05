# ===== backends/ara/Makefile (minimale) =====
SELF_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
ARA_DIR  := $(SELF_DIR)/ara

.PHONY: git-submodules toolchain-llvm verilator

git-submodules:
	@echo "[ARA] make git-submodules"
	cd $(ARA_DIR) && make git-submodules

toolchain-llvm:
	@echo "[ARA] make toolchain-llvm"
	cd $(ARA_DIR) && make toolchain-llvm

verilator:
	@echo "[ARA] make verilator"
	cd $(ARA_DIR) && make verilator
verilate:
	@echo "[ARA] make verilate"
	cd $(ARA_DIR)/hardware && make verilate
