# application master makefile. Included by application makefiles

#XXX: explain the required variables here


# The first target Make finds is its default. So this line needs to be first to
# specify `all` as our default rule
all:


# ---- Output filenames

OUTPUT_NAME ?= $(addsuffix _sdk$(SDK_VERSION)_$(SOFTDEVICE_MODEL), $(PROJECT_NAME))
HEX = $(BUILDDIR)$(OUTPUT_NAME).hex
MERGED_HEX = $(BUILDDIR)$(OUTPUT_NAME)_merged.hex
MERGED_ALL_HEX = $(BUILDDIR)$(OUTPUT_NAME)_merged_all.hex
BOOTLOADER_SETTINGS = $(BUILDDIR)$(OUTPUT_NAME)_settings.hex
BOOTLOADER_HEX = $(wildcard $(BUILDDIR)$(BOOTLOADER)*.hex)
DEBUG_HEX = $(BUILDDIR)$(OUTPUT_NAME)_debug.hex
ELF = $(BUILDDIR)$(OUTPUT_NAME).elf
DEBUG_ELF = $(BUILDDIR)$(OUTPUT_NAME)_debug.elf
BIN = $(BUILDDIR)$(OUTPUT_NAME).bin
DEBUG_BIN = $(BUILDDIR)$(OUTPUT_NAME)_debug.bin
LST = $(BUILDDIR)$(OUTPUT_NAME).lst
MAP = $(BUILDDIR)$(OUTPUT_NAME).Map


# ---- Include additional supporting makefiles
# Build settings
include $(NRF_BASE_DIR)/make/Configuration.mk

# Various file inclusions
include $(NRF_BASE_DIR)/make/Includes.mk

# JTAG flashing configuration and rules
include $(NRF_BASE_DIR)/make/Program.mk


# ---- Rules for building apps
.PHONY:	all
ifeq ($(USE_BOOTLOADER),1)
all: $(OBJS) $(OBJS_AS) $(MERGED_HEX)
else
all: $(OBJS) $(OBJS_AS) $(HEX)
endif

# protobufs must exist before objects
$(OBJS): $(PBGENS)

%.pb.c: %.proto $(PBOPTS)
	$(PROTOC) $(PROTOC_OPTS) $(PROTOC_INCLUDES) --nanopb_out=$(NANOPB_OPTS):. $<

$(BUILDDIR):
	$(TRACE_DIR)
	$(Q)mkdir -p $@

$(BUILDDIR)%.o: %.c $(PGENS) | $(BUILDDIR)
	$(TRACE_CC)
	$(Q)$(CC) $(LDFLAGS) $(CFLAGS) $(OPTIMIZATION_FLAG) $< -o $@

$(BUILDDIR)%.o-debug: %.c $(PGENS) | $(BUILDDIR)
	$(TRACE_CC)
	$(Q)$(CC) $(LDFLAGS) $(CFLAGS) -g -O0 $< -o $@

.PRECIOUS: $(BUILDDIR)%.s
$(BUILDDIR)%.s: %.S | $(BUILDDIR)
	$(TRACE_CC)
	$(Q)$(CC) -E $(SDK_DEFINES) $< > $@

$(BUILDDIR)%.os: $(BUILDDIR)%.s | $(BUILDDIR)
	$(TRACE_AS)
	$(Q)$(AS) $< -o $@

$(BUILDDIR)%.os-debug: $(BUILDDIR)%.s | $(BUILDDIR)
	$(TRACE_AS)
	$(Q)$(AS) $< -o $@

$(ELF): $(OBJS) $(OBJS_AS) $(LIBS) | $(BUILDDIR)
	$(TRACE_LD)
	$(Q)$(LD) $(LDFLAGS) -Wl,--start-group $^ $(LDLIBS) -Wl,--end-group -o $@

$(DEBUG_ELF): $(DEBUG_OBJS) $(DEBUG_OBJS_AS) $(LIBS) | $(BUILDDIR)
	$(TRACE_LD)
	$(Q)$(LD) $(LDFLAGS) -Wl,--start-group $^ $(LDLIBS) -Wl,--end-group -o $@

$(HEX): $(ELF) | $(BUILDDIR)
	$(TRACE_HEX)
	$(Q)$(OBJCOPY) -Oihex $< $(HEX)
	$(TRACE_BIN)
	$(Q)$(OBJCOPY) -Obinary $< $(BIN)
	$(TRACE_SIZ)
	$(Q)$(SIZE) $<

$(DEBUG_HEX): $(DEBUG_ELF) | $(BUILDDIR)
	$(TRACE_HEX)
	$(Q)$(OBJCOPY) -Oihex $< $(DEBUG_HEX)
	$(TRACE_BIN)
	$(Q)$(OBJCOPY) -Obinary $< $(DEBUG_BIN)
	$(TRACE_SIZ)
	$(Q)$(SIZE) $<

.PHONY: bootloader
bootloader: $(BUILDDIR)
	$(Q)cd $(NRF_BASE_DIR)/apps/bootloader/$(BOOTLOADER)/ && KEY_DIR=$(KEY_DIR) make
	$(Q)cp $(NRF_BASE_DIR)/apps/bootloader/$(BOOTLOADER)/_build/*.hex $(BUILDDIR)

$(BOOTLOADER_SETTINGS): $(HEX)
	$(Q)$(NRFUTIL) $(NRFUTIL_SETTINGS_GEN_FLAGS)

$(MERGED_HEX): $(HEX) $(BOOTLOADER_SETTINGS)
	$(Q)$(MERGEHEX) --overlap=replace $(HEX) $(BUILDDIR)$(OUTPUT_NAME)_settings.hex -o $(MERGED_HEX)

$(MERGED_ALL_HEX): bootloader $(HEX) $(BOOTLOADER_SETTINGS)
	$(Q)$(MERGEHEX) --overlap=replace $(HEX) $(BUILDDIR)$(OUTPUT_NAME)_settings.hex $(MBR_PATH) $(BOOTLOADER_HEX) $(MBR_PATH) -o $(MERGED_ALL_HEX)

.PHONY: merged_all
merged_all: $(MERGED_ALL_HEX)

.PHONY: debug
debug: $(DEBUG_OBJS) $(DEBUG_OBJS_AS) $(DEBUG_HEX)

.PHONY: lst
lst: $(ELF)
	$(TRACE_LST)
	$(Q)$(OBJDUMP) $(OBJDUMP_FLAGS) $< > $(LST)

.PHONY: size
size: $(ELF)
	$(TRACE_SIZ)
	$(Q)$(SIZE) $<

.PHONY: clean
clean::
	@echo " Cleaning..."
	$(Q)rm -rf $(BUILDDIR)
ifeq ($(USE_BOOTLOADER), 1)
	$(Q)cd $(NRF_BASE_DIR)/apps/bootloader/$(BOOTLOADER)/ && make clean
endif

.PHONY: pbclean
pbclean::
	@echo " Cleaning..."
	$(Q)rm -rf *.pb.*


# ---- Dependencies
# Include dependency rules for picking up header changes (by convention at bottom of makefile)
-include $(DEPS)
