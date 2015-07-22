#defines from nordic
TARGET_CHIP := NRF51822_QFAA_CA
BOARD := BOARD_SUN_1_3

DEFINES	+= -DNRF51822_QFAA_CA -DBLE_STACK_SUPPORT_REQD


LDFLAGS += --specs=nano.specs 

#set some relative paths
SDK_PATH := ../sdk/
SRC_PATH := ../src/
LIB_PATH := ../libraries/

LIBS = lava_neopixel lava_mpr121 lava_bootloader lava_patterns 

#main project source
C_SOURCE_FILES += $(shell find $(SRC_PATH) -type f -name *.c)
CPP_SOURCE_FILES += $(shell find $(SRC_PATH) -type f -name *.cpp)
C_SOURCE_PATHS += $(shell find $(SRC_PATH) -type d)


#libraries
C_SOURCE_FILES += $(foreach lib, $(LIBS), $(wildcard $(LIB_PATH)$(lib)/*.c))
CPP_SOURCE_FILES += $(foreach lib, $(LIBS), $(wildcard $(LIB_PATH)/$(lib)/*.cpp))

C_SOURCE_PATHS += $(foreach lib, $(LIBS), $(LIB_PATH)$(lib))


#includes 
INCLUDEPATHS += -I"$(SRC_PATH)"
INCLUDEPATHS += $(foreach dir, $(shell find $(SRC_PATH) -type d), -I"$(dir)")
INCLUDEPATHS += $(foreach lib, $(LIBS), -I"$(LIB_PATH)$(lib)")

#nordic code
#C_SOURCE_FILES += $(SDK_PATH)/Source/ble/ble_radio_notification.c
#C_SOURCE_FILES += $(SDK_PATH)/Source/ble/ble_debug_assert_handler.c
#C_SOURCE_FILES += $(SDK_PATH)/Source/sd_common/softdevice_handler.c
#C_SOURCE_FILES += $(SDK_PATH)/Source/ble/ble_conn_params.c
C_SOURCE_FILES += $(SDK_PATH)/Source/ble/ble_advdata.c
#C_SOURCE_FILES += $(SDK_PATH)/Source/ble/ble_bondmngr.c
#C_SOURCE_FILES += $(SDK_PATH)/Source/ble/ble_flash.c
#C_SOURCE_FILES += $(SDK_PATH)/Source/ble/ble_error_log.c
#C_SOURCE_FILES += $(SDK_PATH)/Source/app_common/app_gpiote.c
#C_SOURCE_FILES += $(SDK_PATH)/Source/app_common/app_timer.c
#C_SOURCE_FILES += $(SDK_PATH)/Source/app_common/app_scheduler.c
#C_SOURCE_FILES += $(SDK_PATH)/Source/app_common/pstorage.c
#C_SOURCE_FILES += $(SDK_PATH)/Source/app_common/crc16.c

#find all the h files 
INCLUDEPATHS += -I"$(SDK_PATH)Include/"
INCLUDEPATHS += -I"$(SDK_PATH)Include/app_common/"
INCLUDEPATHS += -I"$(SDK_PATH)Include/ble/"
INCLUDEPATHS += -I"$(SDK_PATH)Include/s110/"
INCLUDEPATHS += -I"$(SDK_PATH)Include/sd_common/"
INCLUDEPATHS += -I"$(SDK_PATH)Include/ble/ble_services/"

#lumen yo
OUTPUT_FILENAME := lumen

DEVICE_VARIANT := xxaa
#DEVICE_VARIANT := xxab

USE_SOFTDEVICE := S110
#USE_SOFTDEVICE := S210

DEVICE := NRF51
DEVICESERIES := nrf51

SDK_INCLUDE_PATH = $(SDK_PATH)/Include/
SDK_SOURCE_PATH = $(SDK_PATH)/Source/
TEMPLATE_PATH += $(SDK_SOURCE_PATH)/templates/gcc/
OUTPUT_BINARY_DIRECTORY := _build

NRF51_TOOLCHAIN_PATH = /opt/nrf51-toolchain/

#system specific search paths for toolchain
ifeq ($(OS),Windows_NT)
	ifeq ($(findstring 86, $(ProgramFiles)), )
		PROGFILES := C:/Program Files
	else
		PROGFILES := C:/Program Files (x86)
	endif
	GNU_INSTALL_ROOT := $(PROGFILES)/GNU Tools ARM Embedded/4.8 2013q4
	GNU_VERSION := 4.8.3
	GNU_PREFIX := arm-none-eabi-
	RM 				:= rm -fr
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		GNU_INSTALL_ROOT := $(NRF51_TOOLCHAIN_PATH)gcc-linux
		GNU_VERSION := 4.7.4
		GNU_PREFIX := arm-none-eabi-
		RM 				:= rm -rf
	endif
	ifeq ($(UNAME_S),Darwin)
		GNU_INSTALL_ROOT := $(NRF51_TOOLCHAIN_PATH)gcc-osx
		GNU_VERSION := 4.7.4
		GNU_PREFIX := arm-none-eabi-
		RM 				:= rm -rf
	endif
endif


ifeq ($(LINKER_SCRIPT),)
	ifeq ($(USE_SOFTDEVICE), S110)
		LINKER_SCRIPT = gcc_$(DEVICESERIES)_s110_$(DEVICE_VARIANT).ld
		OUTPUT_FILENAME := $(OUTPUT_FILENAME)_s110_$(DEVICE_VARIANT)
	else
		ifeq ($(USE_SOFTDEVICE), S210)
			LINKER_SCRIPT = gcc_$(DEVICESERIES)_s210_$(DEVICE_VARIANT).ld
			OUTPUT_FILENAME := $(OUTPUT_FILENAME)_s210_$(DEVICE_VARIANT)
		else
			LINKER_SCRIPT = gcc_$(DEVICESERIES)_blank_$(DEVICE_VARIANT).ld
			OUTPUT_FILENAME := $(OUTPUT_FILENAME)_$(DEVICE_VARIANT)
		endif
	endif
else
# Use externally defined settings
endif

CPU := cortex-m0

# Toolchain commands
CC       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)gcc"
CXX       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)g++"
AS       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)as"
AR       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)ar" -r
LD       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)ld"
NM       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)nm"
OBJDUMP  		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)objdump"
OBJCOPY  		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)objcopy"

MK 				:= mkdir


OBJECT_DIRECTORY := _build
LISTING_DIRECTORY := _build

C_SOURCE_FILES += system_$(DEVICESERIES).c
ASSEMBLER_SOURCE_FILES += gcc_startup_$(DEVICESERIES).s



# Compiler flags
CFLAGS += -c -Os -mcpu=$(CPU) -mthumb -mabi=aapcs -D$(DEVICE) -D$(BOARD) -D$(TARGET_CHIP) --std=gnu99
CFLAGS += -Wall #-Werror
CFLAGS += -mfloat-abi=soft $(DEFINES) 

CXXFLAGS = -std=c++11 -fno-exceptions -ffreestanding -fno-rtti 
CPPFLAGS = -Wall -Wno-return-type -Os -mcpu=cortex-m0 -mthumb  -ffunction-sections -fdata-sections $(DEFINES) -D$(DEVICE) -D$(BOARD) -D$(TARGET_CHIP) -MMD 

# Linker flags
LDFLAGS += -L"$(GNU_INSTALL_ROOT)/arm-none-eabi/lib/armv6-m"
LDFLAGS += -L"$(GNU_INSTALL_ROOT)/lib/gcc/arm-none-eabi/$(GNU_VERSION)/armv6-m"
LDFLAGS += -Xlinker -Map=$(LISTING_DIRECTORY)/$(OUTPUT_FILENAME).map
LDFLAGS += -Os -mcpu=$(CPU) -mthumb -mabi=aapcs -std=c++11 $(CXXFLAGS) -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT) 
LDFLAGS +=  -lm #-lc_s #-lsupc++_s -lstdc++_s

# Assembler flags
ASMFLAGS += -x assembler-with-cpp
 
INCLUDEPATHS += -I"../"
INCLUDEPATHS += -I"$(SDK_PATH)Include"
INCLUDEPATHS += -I"$(SDK_PATH)Include/gcc"
INCLUDEPATHS += -I"$(SDK_PATH)Include/ext_sensors"

# Sorting removes duplicates
BUILD_DIRECTORIES := $(sort $(OBJECT_DIRECTORY) $(OUTPUT_BINARY_DIRECTORY) $(LISTING_DIRECTORY) )

####################################################################
# Rules                                                            #
####################################################################

C_SOURCE_FILENAMES = $(notdir $(C_SOURCE_FILES) )
CPP_SOURCE_FILENAMES = $(notdir $(CPP_SOURCE_FILES) )
ASSEMBLER_SOURCE_FILENAMES = $(notdir $(ASSEMBLER_SOURCE_FILES) )

# Make a list of source paths
C_SOURCE_PATHS += ../ $(SDK_SOURCE_PATH) $(TEMPLATE_PATH) $(wildcard $(SDK_SOURCE_PATH)*/)  $(wildcard $(SDK_SOURCE_PATH)ext_sensors/*/) $(wildcard $(SDK_SOURCE_PATH)ble/*/)
ASSEMBLER_SOURCE_PATHS = ../ $(SDK_SOURCE_PATH) $(TEMPLATE_PATH) $(wildcard $(SDK_SOURCE_PATH)*/)

C_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(C_SOURCE_FILENAMES:.c=.o) )
CPP_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(CPP_SOURCE_FILENAMES:.cpp=.o) )
ASSEMBLER_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(ASSEMBLER_SOURCE_FILENAMES:.s=.o) )


# Set source lookup paths
vpath %.c $(C_SOURCE_PATHS)
vpath %.cpp $(C_SOURCE_PATHS)
vpath %.s $(ASSEMBLER_SOURCE_PATHS)

# Include automatically previously generated dependencies
-include $(addprefix $(OBJECT_DIRECTORY)/, $(COBJS:.o=.d))
-include $(addprefix $(OBJECT_DIRECTORY)/, $(CXXOBJS:.o=.d))

### Targets
debug:    CFLAGS += -DDEBUG  -Os
debug:    ASMFLAGS += -DDEBUG -Os
debug:    $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex

.PHONY: release
release: clean
release:  CFLAGS += -DNDEBUG -Os
release:  ASMFLAGS += -DNDEBUG -Os
release:  $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex

echostuff:
	@echo C_OBJECTS: [$(C_OBJECTS)]
	@echo C_SOURCE_FILES: [$(C_SOURCE_FILES)]
	@echo CPP_FILES: [$(CPP_SOURCE_FILES)]

	

## Create build directories
$(BUILD_DIRECTORIES):
	$(MK) $@

## Create objects from C source files
$(OBJECT_DIRECTORY)/%.o: %.c
# Build header dependencies
	$(CC) $(CFLAGS) $(INCLUDEPATHS) -M $< -MF "$(@:.o=.d)" -MT $@
# Do the actual compilation
	$(CC) $(CFLAGS) $(INCLUDEPATHS) -c -o $@ $<

#cpp
$(OBJECT_DIRECTORY)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INCLUDEPATHS) -M $< -MF "$(@:.o=.d)" -MT $@
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INCLUDEPATHS) -c -o $@ $<

## Assemble .s files
$(OBJECT_DIRECTORY)/%.o: %.s
	$(CC) $(ASMFLAGS) $(INCLUDEPATHS) -c -o $@ $<

## Link C and assembler objects to an .out file
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out: $(BUILD_DIRECTORIES) $(C_OBJECTS) $(CPP_OBJECTS) $(ASSEMBLER_OBJECTS) $(LIBRARIES)
	$(CXX) $(LDFLAGS) $(C_OBJECTS) $(CPP_OBJECTS) $(ASSEMBLER_OBJECTS) $(LIBRARIES) -lm -o $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out

## Create binary .bin file from the .out file
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	$(OBJCOPY) -O binary $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin
	du -sh $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin

## Create binary .hex file from the .out file
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	$(OBJCOPY) -O ihex $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex
	du -sh $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex
	
## Default build target
.PHONY: all
all: clean debug

clean:
	$(RM) $(OUTPUT_BINARY_DIRECTORY)

## Program device
#.PHONY: flash
#flash: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex
#	nrfjprog --reset --program $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex
#
#.PHONY: reset
#reset:
#	nrfjprog --reset
#
#.PHONY: erase
#erase:
#	nrfjprog --erase

flash: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin 
	sudo node $(NRF51_TOOLCHAIN_PATH)/ionode/flash.js $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin

publish: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin
	node $(NRF51_TOOLCHAIN_PATH)/lumen-firmware-publisher/upload.js $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin

dfumode:
	sudo sh ./dfumode.sh
	sleep 3




