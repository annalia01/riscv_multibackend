SELF_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
ROOT_DIR := $(abspath $(SELF_DIR)/../..)       

KERNEL_DIR := $(ROOT_DIR)/kernel
COMMON_DIR := $(ROOT_DIR)/kernel/common
DATA_DIR   := $(ROOT_DIR)/data

SIMPLYV_DIR := $(SELF_DIR)/Simply-V
EXAMPLES_DIR := $(SIMPLYV_DIR)/sw/SoC/examples
HW_DIR := $(SIMPLYV_DIR)/hw/xilinx

.PHONY: prepare_simplyv build openocd_run run

prepare_simplyv:
	@echo "[ARA2] Preparing Simply-V environment..."
	@cd $(SIMPLYV_DIR) && git apply ara_config.patch || true
	@make -C $(SIMPLYV_DIR) hw
	@make -C $(HW_DIR) program_bitstream
	@echo "[ARA2] Simply-V ready."

build:
	@echo "[ARA2] Building example $(KERNEL)..."
	@make -C $(SIMPLYV_DIR) sw
	@make -C $(EXAMPLES_DIR)/$(KERNEL) clean all
	@echo "[ARA2] Build complete."

openocd_run:
	@make -C $(HW_DIR) openocd_run

run:
	@if [ -z "$(KERNEL)" ]; then \
		echo "[ERROR] Please specify KERNEL=name"; \
		exit 1; \
	fi
	@if [ ! -d "$(EXAMPLES_DIR)/$(KERNEL)" ]; then \
		echo "[ERROR] Example not built. Run: make build KERNEL=$(KERNEL)"; \
		exit 1; \
	fi
	@echo "[ARA2] Running example $(KERNEL)..."
	@make -C $(HW_DIR) gdb_run EXAMPLE=$(KERNEL)
