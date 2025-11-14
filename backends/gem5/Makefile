###############################################################################
#                         GEM5 BACKEND - UNIFIED MAKEFILE
###############################################################################

CC          := riscv64-unknown-elf-gcc
GEM5_BIN    := $(HOME)/gem5/build/RISCV/gem5.opt
RUN_SCRIPT  := run_rvv.py

KERNEL      ?= fmatmul
VLEN        ?= 256
ELEN        ?= 64
MARCH       := rv64gcv_zicsr_zicntr
ABI         := lp64

###############################################################################
# PATHS
###############################################################################

KERNEL_DIR      := ../../kernel/$(KERNEL)
COMMON_DIR      := ../../kernel/common
DATA_DIR        := ../../data/$(KERNEL)

OPENBLAS_DIR    := ../../benchmarks/openblas/kernel/riscv64
OPENBLAS_MAIN_DIR := ../../benchmarks/openblas_mains

RIVEC_DIR       := ../../benchmarks/rivec

###############################################################################
# FLAGS
###############################################################################

CFLAGS := -O2 -march=$(MARCH) -mabi=$(ABI) -I$(COMMON_DIR) -lm

###############################################################################
# BENCHMARK / KERNEL SELECTION LOGIC
###############################################################################

# Se NON esiste un kernel "nostro"
ifeq ($(wildcard ../../kernel/$(KERNEL)),)

    ###########################################################################
    #                              OPENBLAS
    ###########################################################################

    ifneq ($(wildcard $(OPENBLAS_DIR)/$(KERNEL).c),)
        BENCH_SRC  := $(OPENBLAS_DIR)/$(KERNEL).c
        CUSTOM_MAIN := $(OPENBLAS_MAIN_DIR)/$(KERNEL)_main.c

        SRCS   := $(BENCH_SRC) $(CUSTOM_MAIN) $(COMMON_DIR)/util.c
        TARGET := build/$(KERNEL).elf

        CFLAGS += -DOS_EMBEDDED=1 -DNO_SYSV_IPC=1 -DSMP=0 -DUSE_LOCKING=0 \
                  -DARCH_RISCV64=1 -DRISCV64_ZVL256B=1 -DSPIKEGEM

        OPENBLAS_INC := $(shell find ../../benchmarks/openblas -type d)
        CFLAGS += $(foreach d,$(OPENBLAS_INC),-I$d)

    ###########################################################################
    #                              RIVEC
    ###########################################################################

    else ifneq ($(wildcard $(RIVEC_DIR)/$(KERNEL).c),)
        BENCH_SRC  := $(RIVEC_DIR)/$(KERNEL).c
        CUSTOM_MAIN := $(OPENBLAS_MAIN_DIR)/$(KERNEL)_main.c

        SRCS   := $(BENCH_SRC) $(CUSTOM_MAIN)
        TARGET := build/$(KERNEL).elf
        CFLAGS += -I$(RIVEC_DIR)

    else
        $(error Invalid KERNEL '$(KERNEL)'. Not found in kernel/, openblas/, or rivec/)
    endif

else
    ###########################################################################
    #                            OUR OWN KERNEL
    ###########################################################################

    TARGET := $(KERNEL_DIR)/main.elf
    SRCS := $(wildcard $(KERNEL_DIR)/*.c) \
            $(wildcard $(KERNEL_DIR)/src/*.c) \
            $(COMMON_DIR)/util.c \
            $(DATA_DIR)/dataset.c

    CFLAGS += -I$(KERNEL_DIR)/inc -I$(DATA_DIR)
endif

###############################################################################
# BUILD
###############################################################################

.PHONY: build run clean

build: $(TARGET)

$(TARGET): $(SRCS)
	@echo "[GEM5] Compiling $(KERNEL)..."
	$(CC) $(CFLAGS) -o $@ $^

###############################################################################
# RUN
###############################################################################

run: $(TARGET)
	@echo "[GEM5] Running $(KERNEL) on gem5..."
	@echo "[GEM5] ELF path: $(abspath $(TARGET))"
	VLEN=$(VLEN) ELEN=$(ELEN) GEM5_ELF=$(abspath $(TARGET)) \
	$(GEM5_BIN) $(RUN_SCRIPT)

###############################################################################
# CLEAN
###############################################################################

clean:
	@echo "[GEM5] Cleaning..."
	@rm -rf build/*.elf
