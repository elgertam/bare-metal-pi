RPI_VERSION ?= 4

BOOTMNT ?= /Volumes/system-boot

LLVMPATH ?= /usr/local/opt/llvm/bin
CLANGFLAGS = -DRPI_VERSION=$(RPI_VERSION) -Wall -O2 -ffreestanding -nostdlib \
	         -Iinclude -mgeneral-regs-only --target=aarch64-elf

BUILD_DIR = build
SRC_DIR = src
DIST_DIR = dist

all: clean kernel8.img armstub

clean:
	rm -rf $(BUILD_DIR) armstub/$(BUILD_DIR) *.elf *.o *.img > /dev/null 2> /dev/null || true

clean-all: clean
	rm -rf $(DIST_DIR)

eject :
	diskutil unmount /Volumes/system-boot

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(LLVMPATH)/clang $(CLANGFLAGS) -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(LLVMPATH)/clang $(CLANGFLAGS) -c $< -o $@

armstub/$(BUILD_DIR)/%_s.o: armstub/$(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(LLVMPATH)/clang $(CLANGFLAGS) -c $< -o $@

C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

ARMSTUB_FILES = $(wildcard armstub/$(SRC_DIR)/*.S)
ARMSTUB_OBJ_FILES = $(ARMSTUB_FILES:armstub/$(SRC_DIR)/%.S=armstub/$(BUILD_DIR)/%_s.o)

ARMSTUB_DEP_FILES = $(ARMSTUB_OBJ_FILES:%.o=%.d)
-include $(ARMSTUB_DEP_FILES)

kernel8.img: $(SRC_DIR)/linker.ld $(OBJ_FILES)
	mkdir -p $(DIST_DIR)
	@echo "Building for RPI $(value RPI_VERSION)"
	@echo "Deploy to $(value BOOTMNT)"
	@echo ""
	$(LLVMPATH)/ld.lld -m aarch64elf -nostdlib $(OBJ_FILES) -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf
ifeq ($(RPI_VERSION), 4)
	$(LLVMPATH)/llvm-objcopy -O binary $(BUILD_DIR)/kernel8.elf $(DIST_DIR)/kernel8-rpi4.img
else
	$(LLVMPATH)/llvm-objcopy -O binary $(BUILD_DIR)/kernel8.elf $(DIST_DIR)/kernel8.img
endif
	cp config.txt $(DIST_DIR)/
	cp $(DIST_DIR)/* $(BOOTMNT)/
	sync

armstub: $(ARMSTUB_OBJ_FILES)
	mkdir -p $(DIST_DIR)
	$(LLVMPATH)/ld.lld --section-start=.text=0 -m aarch64elf -o armstub/$(BUILD_DIR)/armstub.elf $(ARMSTUB_OBJ_FILES)
	$(LLVMPATH)/llvm-objcopy -O binary armstub/$(BUILD_DIR)/armstub.elf $(DIST_DIR)/armstub-new.bin
	cp $(DIST_DIR)/armstub-new.bin $(BOOTMNT)/
	sync

.PHONY: all armstub clean clean-all eject
