RPI_VERSION ?= 4

BOOTMNT ?= /mnt/dist
ARMGNU ?= aarch64-linux-gnu

COPS = -DRPI_VERSION=$(RPI_VERSION) -Wall -nostdlib -ffreestanding \
	   -Iinclude -mgeneral-regs-only

ASMOPS = -Iinclude

BUILD_DIR = build
SRC_DIR = src
DIST_DIR = dist
ARMSTUB_BUILD_DIR = armstub/build
ARMSTUB_SRC_DIR = armstub/src

all : kernel8.img armstub

clean :
	rm -rf $(BUILD_DIR) $(ARMSTUB_BUILD_DIR) *.img

clean-all: clean
	rm -rf $(DIST_DIR)

eject :
	diskutil unmount /Volumes/system-boot

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(ARMSTUB_BUILD_DIR)/%_s.o: $(ARMSTUB_SRC_DIR)/%.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

ARMSTUB_ASM_FILES = $(wildcard $(ARMSTUB_SRC_DIR)/%.S)
ARMSTUB_OBJ_FILES = $(wildcard $(ARMSTUB_BUILD_DIR)/*_s.o)

ARMSTUB_DEP_FILES = $(ARMSTUB_OBJ_FILES:%.o=%.d)
-include $(ARMSTUB_DEP_FILES)

# TODO: Determine why this doesn't work
# armstub: $(ARMSTUB_OBJ_FILES)
	# mkdir -p $(DIST_DIR)
	# $(ARMGNU)-ld --section-start=.text=0 -o $(ARMSTUB_BUILD_DIR)/armstub.elf $(ARMSTUB_OBJ_FILES)
	# $(ARMGNU)-objcopy $(ARMSTUB_BUILD_DIR)/armstub.elf -O binary $(DIST_DIR)/armstub-new.bin
	# mkdir -p $(BOOTMNT)
	# cp $(DIST_DIR)/armstub-new.bin $(BOOTMNT)/
	# sync

armstub: $(ARMSTUB_BUILD_DIR)/armstub_s.o
	mkdir -p $(DIST_DIR)
	$(ARMGNU)-ld --section-start=.text=0 -o $(ARMSTUB_BUILD_DIR)/armstub.elf $(ARMSTUB_BUILD_DIR)/armstub_s.o
	$(ARMGNU)-objcopy $(ARMSTUB_BUILD_DIR)/armstub.elf -O binary $(DIST_DIR)/armstub-new.bin
	mkdir -p $(BOOTMNT)
	cp $(DIST_DIR)/armstub-new.bin $(BOOTMNT)/
	sync

C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

kernel8.img: $(SRC_DIR)/linker.ld $(OBJ_FILES)
	mkdir -p $(DIST_DIR)
	@echo "Building for RPI $(value RPI_VERSION)"
	@echo "Deploy to $(value BOOTMNT)"
	@echo ""
	$(ARMGNU)-ld -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf $(OBJ_FILES)
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary $(DIST_DIR)/kernel8.img
	cp config.txt $(DIST_DIR)/
	mkdir -p $(BOOTMNT)
ifeq ($(RPI_VERSION), 4)
	mv $(DIST_DIR)/kernel8.img $(DIST_DIR)/kernel8-rpi4.img
endif
	cp $(DIST_DIR)/* $(BOOTMNT)/
	sync
