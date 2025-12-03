SELF_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
ROOT_DIR := $(abspath $(SELF_DIR)/../../..)

KERNEL_DIR := $(ROOT_DIR)/kernel
DATA_DIR   := $(ROOT_DIR)/data
COMMON_DIR := $(ROOT_DIR)/kernel/common
EXAMPLES_DIR := $(ROOT_DIR)/sw/SoC/examples
HW_DIR := $(ROOT_DIR)/hw/xilinx

ifeq ($(findstring _32,$(KERNEL)),_32)
    COMMON_SUFFIX := 32
else
    COMMON_SUFFIX := 8
endif

.PHONY: app run

app:
	@if [ -z "$(KERNEL)" ]; then \
		echo "[SV][ERROR] Please specify a kernel, e.g. make app KERNEL=fmatmul_8"; \
		exit 1; \
	fi
	@echo "[SV] Preparing kernel $(KERNEL)..."

	@mkdir -p $(EXAMPLES_DIR)/$(KERNEL)

	@echo "[SV] Copying kernel sources..."
	@if [ ! -d "$(KERNEL_DIR)/$(KERNEL)" ]; then \
		echo "[SV][ERROR] Kernel $(KERNEL) does not exist in kernel/"; \
		exit 1; \
	fi
	cp -u $(KERNEL_DIR)/$(KERNEL)/*.c $(EXAMPLES_DIR)/$(KERNEL)/ 2>/dev/null || true
	cp -u $(KERNEL_DIR)/$(KERNEL)/inc/* $(EXAMPLES_DIR)/$(KERNEL)/ 2>/dev/null || true

	@echo "[SV] Copying dataset sources (if available)..."
	@if [ -d "$(DATA_DIR)/$(KERNEL)" ]; then \
		cp -u $(DATA_DIR)/$(KERNEL)/*.c $(EXAMPLES_DIR)/$(KERNEL)/ 2>/dev/null || true; \
		cp -u $(DATA_DIR)/$(KERNEL)/*.h $(EXAMPLES_DIR)/$(KERNEL)/ 2>/dev/null || true; \
	fi

	@echo "[SV] Copying common files (*.$(COMMON_SUFFIX))..."
	cp -u $(COMMON_DIR)/*_$(COMMON_SUFFIX).c $(EXAMPLES_DIR)/$(KERNEL)/ 2>/dev/null || true
	cp -u $(COMMON_DIR)/*_$(COMMON_SUFFIX).h $(EXAMPLES_DIR)/$(KERNEL)/ 2>/dev/null || true

	@echo "[SV] Building kernel $(KERNEL)..."
	@make -C $(EXAMPLES_DIR)/$(KERNEL) clean all

	@echo "[SV] Kernel $(KERNEL) built successfully."


run:
	@if [ -z "$(KERNEL)" ]; then \
		echo "[SV][ERROR] Please specify a kernel, e.g. make run KERNEL=fmatmul_32"; \
		exit 1; \
	fi
	@if [ ! -d "$(EXAMPLES_DIR)/$(KERNEL)" ]; then \
		echo "[SV][ERROR] Build not found for $(KERNEL). Run: make app KERNEL=$(KERNEL)"; \
		exit 1; \
	fi

	@echo "[SV] Running kernel $(KERNEL) using hw/xilinx..."
	@make -C $(HW_DIR) gdb_run EXAMPLE=$(KERNEL)

