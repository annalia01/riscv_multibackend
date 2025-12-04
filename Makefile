# ===== Root Makefile: unified multi-backend controller =====

BACKEND ?= spike
KERNEL  ?= fmatmul

# === Percorsi ai backend ===
BACKENDS_DIR := backends
ARA_DIR      := $(BACKENDS_DIR)/ara
SPIKE_DIR    := $(BACKENDS_DIR)/spike
GEM5_DIR     := $(BACKENDS_DIR)/gem5
ARA2_DIR	 := $(BACKENDS_DIR)/ara_2_lanes

# === Target di build comuni ===
.PHONY: build run clean openocd_run

# === BUILD ===
build:
ifeq ($(BACKEND),spike)
	@echo "[ROOT] Building kernel $(KERNEL) for Spike..."
	@$(MAKE) -C $(SPIKE_DIR) KERNEL=$(KERNEL) build
else ifeq ($(BACKEND),gem5)
	@echo "[ROOT] Building kernel $(KERNEL) for Gem5..."
	@$(MAKE) -C $(GEM5_DIR) KERNEL=$(KERNEL) build
else ifeq ($(BACKEND),ara)
	@echo "[ROOT] Building kernel $(KERNEL) for Ara..."
	@$(MAKE) -C $(ARA_DIR) KERNEL=$(KERNEL) app
else ifeq ($(BACKEND),ara_2_lanes)
	@echo "[ROOT] Building kernel $(KERNEL) for Ara2..."
	@$(MAKE) -C $(ARA2_DIR) KERNEL=$(KERNEL) build
else
	@echo "[ERROR] Unknown BACKEND=$(BACKEND). Use ara | spike | gem5."
	exit 1
endif

openocd_run:
	@echo "Make openocd..."
	@$(MAKE) -C $(ARA2_DIR) openocd_run
# === RUN ===
run:
ifeq ($(BACKEND),spike)
	@echo "[ROOT] Running $(KERNEL) on Spike..."
	@$(MAKE) -C $(SPIKE_DIR) KERNEL=$(KERNEL) run
else ifeq ($(BACKEND),gem5)
	@echo "[ROOT] Running $(KERNEL) on Gem5..."
	@$(MAKE) -C $(GEM5_DIR) KERNEL=$(KERNEL) run
else ifeq ($(BACKEND),ara)
	@echo "[ROOT] Running $(KERNEL) on Ara (Verilator)..."
	@$(MAKE) -C $(ARA_DIR) KERNEL=$(KERNEL) run
else ifeq ($(BACKEND),ara_2_lanes)
	@echo "[ROOT] Running kernel $(KERNEL) on Ara2..."
	@$(MAKE) -C $(ARA2_DIR) KERNEL=$(KERNEL) run
else
	@echo "[ERROR] Unknown BACKEND=$(BACKEND). Use ara | spike | gem5."
	exit 1
endif

# === CLEAN ===
clean:
ifeq ($(BACKEND),spike)
	@$(MAKE) -C $(SPIKE_DIR) KERNEL=$(KERNEL) clean
else ifeq ($(BACKEND),gem5)
	@$(MAKE) -C $(GEM5_DIR) KERNEL=$(KERNEL) clean
else ifeq ($(BACKEND),ara)
	@echo "[ARA] Cleaning build directories..."
	cd $(ARA_DIR)/ara && make clean || true
else
	@echo "[ERROR] Unknown BACKEND=$(BACKEND)."
	exit 1
endif
