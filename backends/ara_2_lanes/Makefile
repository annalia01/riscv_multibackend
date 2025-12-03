SELF_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
ROOT_DIR := $(abspath $(SELF_DIR)/../..)       

KERNEL_DIR := $(ROOT_DIR)/kernel
COMMON_DIR := $(ROOT_DIR)/kernel/common
DATA_DIR   := $(ROOT_DIR)/data

SIMPLYV_DIR := $(SELF_DIR)/Simply-V
EXAMPLES_DIR := $(SIMPLYV_DIR)/sw/SoC/examples
HW_DIR := $(SIMPLYV_DIR)/hw/xilinx


ifeq ($(findstring _32,$(KERNEL)),_32)
    COMMON_SUFFIX := 32
else
    COMMON_SUFFIX := 8
endif


.PHONY: build run


build:
	@if [ -z "$(KERNEL)" ]; then \
		echo "[ERROR] Please specify a kernel: make app KERNEL=name"; \
		exit 1; \
	fi

	@echo "[ARA2] Preparing example for $(KERNEL)..."
	mkdir -p $(EXAMPLES_DIR)/$(KERNEL)

	@echo "[ARA2] Copy kernel sources..."
	if [ ! -d "$(KERNEL_DIR)/$(KERNEL)" ]; then \
		echo "[ERROR] Kernel $(KERNEL) does not exist in kernel/"; \
		exit 1; \
	fi

	cp -u $(KERNEL_DIR)/$(KERNEL)/*.c $(EXAMPLES_DIR)/$(KERNEL)/ 2>/dev/null || true

	if [ -d "$(KERNEL_DIR)/$(KERNEL)/inc" ]; then \
		cp -u $(KERNEL_DIR)/$(KERNEL)/inc/* $(EXAMPLES_DIR)/$(KERNEL)/ 2>/dev/null || true; \
	fi

	if [ -d "$(KERNEL_DIR)/$(KERNEL)/src" ]; then \
		cp -u $(KERNEL_DIR)/$(KERNEL)/src/*.c $(EXAMPLES_DIR)/$(KERNEL)/ 2>/dev/null || true; \
	fi


	@echo "[ARA2] Copying dataset (if available)..."
	if [ -d "$(DATA_DIR)/$(KERNEL)" ]; then \
		cp -u $(DATA_DIR)/$(KERNEL)/* $(EXAMPLES_DIR)/$(KERNEL)/ 2>/dev/null || true; \
	fi

	@echo "[ARA2] Copying common files (*.$(COMMON_SUFFIX))..."
	cp -u $(COMMON_DIR)/*_$(COMMON_SUFFIX).c $(EXAMPLES_DIR)/$(KERNEL)/ 2>/dev/null || true
	cp -u $(COMMON_DIR)/*_$(COMMON_SUFFIX).h $(EXAMPLES_DIR)/$(KERNEL)/ 2>/dev/null || true

	@echo "[ARA2] Building example in Simply-V/sw/SoC/examples/$(KERNEL)..."
	make -C $(EXAMPLES_DIR)/$(KERNEL) clean all


run:
	@if [ -z "$(KERNEL)" ]; then \
		echo "[ERROR] Please specify KERNEL=name"; \
		exit 1; \
	fi

	@if [ ! -d "$(EXAMPLES_DIR)/$(KERNEL)" ]; then \
		echo "[ERROR] Example not built. First run: make app KERNEL=$(KERNEL)"; \
		exit 1; \
	fi

	@echo "[ARA2] Running example $(KERNEL) with hw/xilinx..."
	make -C $(HW_DIR) gdb_run EXAMPLE=$(KERNEL)
