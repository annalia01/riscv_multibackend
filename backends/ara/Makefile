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
checkout:
        @echo "[ARA] make checkout"
        cd $(ARA_DIR)/hardware && make checkout
apply-patches:
        @echo "[ARA] make apply-patches"
        cd $(ARA_DIR)/hardware && make verilate

app:
	@if [ -z "$(KERNEL)" ]; then \
		echo "[ARA][ERROR] Please specify a kernel, e.g. make app KERNEL=fmatmul_2"; \
		exit 1; \
	fi
	@echo "[ARA] Building kernel $(KERNEL)..."
	cd $(ARA_DIR)/apps && \
	make APPS_DIR=$(abspath $(SELF_DIR)/../../kernel) bin/$(KERNEL)
