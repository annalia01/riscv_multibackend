# ===== backends/gem5/Makefile =====

# ==== Toolchain e percorsi ====
CC          := riscv64-unknown-elf-gcc
GEM5_BIN    := gem5/build/RISCV/gem5.opt
RUN_SCRIPT  := run_rvv.py

# ==== Parametri variabili ====
KERNEL      ?= fmatmul
VLEN        ?= 256
ELEN        ?= 64
MARCH       := rv64gcv_zicsr_zicntr
ABI         := lp64

# ==== Percorsi ====
KERNEL_DIR  := ../../kernel/$(KERNEL)
COMMON_DIR  := ../../kernel/common
DATA_DIR    := ../../data/$(KERNEL)

# ==== File ====
TARGET      := $(KERNEL_DIR)/main.elf
SRCS        := $(wildcard $(KERNEL_DIR)/*.c) \
               $(wildcard $(KERNEL_DIR)/src/*.c) \
               $(COMMON_DIR)/util.c \
               $(DATA_DIR)/dataset.c

# ==== Flag ====
CFLAGS      := -O2 -march=$(MARCH) -mabi=$(ABI) -DSPIKEGEM \
               -I$(KERNEL_DIR)/inc -I$(COMMON_DIR) -I$(DATA_DIR)

.PHONY: build run clean

# ==== BUILD ====
build: $(TARGET)

$(TARGET): $(SRCS)
	@echo "[GEM5] Compiling kernel $(KERNEL)..."
	$(CC) $(CFLAGS) -o $@ $^

# ==== RUN ====
run: $(TARGET)
	@echo "[GEM5] Running kernel $(KERNEL) on gem5..."
	@echo "[GEM5] ELF path: $(abspath $(TARGET))"
	VLEN=$(VLEN) ELEN=$(ELEN) GEM5_ELF=$(abspath $(TARGET)) \
	$(GEM5_BIN) $(RUN_SCRIPT)

# ==== CLEAN ====
clean:
	@echo "[GEM5] Cleaning kernel $(KERNEL)..."
	$(RM) $(TARGET)
