SELF_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
ARA_DIR  := $(SELF_DIR)/ara
DATA_DIR := $(abspath $(SELF_DIR)/../../data)

.PHONY: git-submodules toolchain-llvm verilator checkout apply-patches verilate app run

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
	cd $(ARA_DIR)/hardware && make apply-patches

app:
        @if [ -z "$(KERNEL)" ]; then \
                echo "[ARA][ERROR] Please specify a kernel, e.g. make app KERNEL=fmatmul_8"; \
                exit 1; \
        fi
        @echo "[ARA] Building kernel $(KERNEL)..."
        @cd $(ARA_DIR)/apps && \
        if [ ! -L "$(KERNEL)" ]; then \
                echo "[ARA] Linking $(KERNEL) -> ../../../../kernel/$(KERNEL)"; \
                ln -sfn ../../../../kernel/$(KERNEL) $(KERNEL); \
        fi && \
        if [ -d "$(abspath $(SELF_DIR)/../../data)/$(KERNEL)" ]; then \
                echo "[ARA] Copying dataset sources for $(KERNEL)"; \
                cp -u $(abspath $(SELF_DIR)/../../data)/$(KERNEL)/*.c $(KERNEL)/ || true; \
        fi && \
        make bin/$(KERNEL) \
             APPS_DIR=$(abspath $(SELF_DIR)/../../kernel) \
             ENV_DEFINES="-I$(COMMON_DIR) -I$(DATA_DIR) -I$(DATA_DIR)/$(KERNEL) -I$(abspath $(SELF_DIR)/../../kernel)/$(KERNEL)/inc"


run:
        @if [ -z "$(KERNEL)" ]; then \
                echo "[ARA][ERROR] Please specify a kernel, e.g. make run KERNEL=fmatmul_32"; \
                exit 1; \
        fi
        @echo "[ARA] Running kernel $(KERNEL) with Verilator..."
        cd $(ARA_DIR)/hardware && app=$(KERNEL) make simv
