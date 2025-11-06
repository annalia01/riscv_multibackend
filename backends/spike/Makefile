CC          := riscv64-unknown-elf-gcc
SPIKE_BIN   := ../../install/riscv-isa-sim/bin/spike
PK          := $(HOME)/RISC-V/rv64/pk/riscv64-unknown-elf/bin/pk

KERNEL      ?= fmatmul       
VLEN        ?= 256
MARCH       := rv64gcv_zicsr_zicntr
ABI         := lp64

KERNEL_DIR  := ../../kernel/$(KERNEL)
COMMON_DIR  := ../../common
DATA_DIR    := ../../data

TARGET      := $(KERNEL_DIR)/main.elf
SRCS        := $(wildcard $(KERNEL_DIR)/*.c) \
               $(wildcard $(KERNEL_DIR)/src/*.c) \
               $(COMMON_DIR)/util.c \
               $(DATA_DIR)/dataset.c

CFLAGS      := -O2 -march=$(MARCH) -mabi=$(ABI) -DSPIKEGEM \
               -I$(KERNEL_DIR)/inc -I$(COMMON_DIR) -I$(DATA_DIR)

.PHONY: build run clean

# ==== BUILD ====
build: $(TARGET)

$(TARGET): $(SRCS)
	@echo "[SPIKE] Compiling kernel $(KERNEL)..."
	$(CC) $(CFLAGS) -o $@ $^

# ==== RUN ====
run: $(TARGET)
	@echo "[SPIKE] Running kernel $(KERNEL) on Spike..."
	$(SPIKE_BIN) --isa=$(MARCH) --varch="vlen:$(VLEN),elen:64" $(PK) $^

# ==== CLEAN ====
clean:
	@echo "[SPIKE] Cleaning kernel $(KERNEL)..."
	$(RM) $(TARGET)
