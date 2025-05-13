BIN     = ../../riscv-embedded-gcc/bin
# BIN_PREFIX = riscv-wch-elf
BIN_PREFIX = riscv-none-embed

CHIP_INI = ./ch573.ini

CFLAGS 	= -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -g
DEFINES = -D BLE_MEMHEAP_SIZE=4096 

LINTER = clang-format --style=LLVM -i

OUTPUT_DIR = ./dist

TARGET     = $(addprefix $(OUTPUT_DIR)/, $(notdir $(CURDIR)))

Project_ROOT    = .
Project_INC_DIR = $(Project_ROOT)/inc
Project_SRC_DIR = $(Project_ROOT)/src
Project_SOURCES = $(notdir $(wildcard $(Project_SRC_DIR)/*.c))
Project_OBJECTS = $(addprefix $(OUTPUT_DIR)/, $(Project_SOURCES:.c=.o))

RVMSIS_ROOT			= ../../EVT/EXAM/SRC/RVMSIS
RVMSIS_SOURCES 	= core_riscv.c
RVMSIS_OBJECTS  = $(addprefix $(OUTPUT_DIR)/, $(RVMSIS_SOURCES:.c=.o))

StdPeriphDriver_ROOT    = ../../EVT/EXAM/SRC/StdPeriphDriver
StdPeriphDriver_INC_DIR = $(StdPeriphDriver_ROOT)/inc
StdPeriphDriver_SRC_DIR = $(StdPeriphDriver_ROOT)
StdPeriphDriver_SOURCES  = CH57x_sys.c CH57x_gpio.c CH57x_uart0.c CH57x_adc.c CH57x_clk.c CH57x_uart1.c
StdPeriphDriver_OBJECTS = $(addprefix $(OUTPUT_DIR)/, $(StdPeriphDriver_SOURCES:.c=.o))

Startup_ROOT		= ../../EVT/EXAM/SRC/Startup
Startup_SOURCES = startup_CH573.S
Startup_OBJECTS = $(addprefix $(OUTPUT_DIR)/asm/, $(Startup_SOURCES:.S=.o))

APP_ROOT    = ./APP
APP_INC_DIR = $(APP_ROOT)/include
APP_SOURCES = $(notdir $(wildcard $(APP_ROOT)/*.c))
APP_OBJECTS = $(addprefix $(OUTPUT_DIR)/APP/, $(APP_SOURCES:.c=.o))

HAL_ROOT    = ./HAL
HAL_INC_DIR = $(HAL_ROOT)/include
HAL_SOURCES = MCU.c RTC.c KEY.c LED.c
HAL_OBJECTS = $(addprefix $(OUTPUT_DIR)/HAL/, $(HAL_SOURCES:.c=.o))

LINK_FILE = ../../EVT/EXAM/SRC/Ld/Link.ld

BLELIB_ROOT  = ../../EVT/EXAM/BLE/LIB
MESHLIB_ROOT = ../../EVT/EXAM/BLE/MESH/MESH_LIB

INCLUDES = -I $(Project_INC_DIR) -I $(StdPeriphDriver_INC_DIR) -I $(RVMSIS_ROOT) -I $(APP_INC_DIR) -I $(HAL_INC_DIR) -I $(BLELIB_ROOT) -I $(MESHLIB_ROOT)

VPATH = $(Project_SRC_DIR):$(StdPeriphDriver_SRC_DIR):$(Startup_ROOT):$(RVMSIS_ROOT):$(APP_ROOT):$(HAL_ROOT)

LIBS = -lISP573 -lCH57xBLE -lMESH

all: $(OUTPUT_DIR) $(TARGET).hex $(TARGET).siz $(TARGET).lst

version:
	$(BIN)/$(BIN_PREFIX)-gcc --version

lint: $(Project_SOURCES)
	$(foreach f, $^, $(LINTER) $(f);)

$(OUTPUT_DIR):
	$(MAKE) lint
	mkdir -p $(OUTPUT_DIR)/asm
	mkdir -p $(OUTPUT_DIR)/APP
	mkdir -p $(OUTPUT_DIR)/HAL

clean:
	rm -rf $(OUTPUT_DIR)

$(OUTPUT_DIR)/asm/%.o: %.S
	$(BIN)/$(BIN_PREFIX)-gcc $(CFLAGS) -MMD -MP -x assembler-with-cpp -MF "$(@:%.o=%.d)" -MT $@ -c -o "$@" "$<"

$(OUTPUT_DIR)/%.o: %.c
	$(BIN)/$(BIN_PREFIX)-gcc $(CFLAGS) $(INCLUDES) $(DEFINES) -std=gnu99 -MMD -MP -MF "$(@:%.o=%.d)" -MT $@ -c -o "$@" "$<"

$(TARGET).elf: $(Project_OBJECTS) $(StdPeriphDriver_OBJECTS) $(Startup_OBJECTS) $(RVMSIS_OBJECTS) $(APP_OBJECTS) $(HAL_OBJECTS)
	$(BIN)/$(BIN_PREFIX)-gcc $(CFLAGS) \
		-T "$(LINK_FILE)" -nostartfiles -Xlinker --gc-sections \
		-L"$(StdPeriphDriver_ROOT)" \
		-L"$(BLELIB_ROOT)" \
		-L"$(MESHLIB_ROOT)" \
		-Xlinker --print-memory-usage -Wl,-Map,"$(@:%.elf=%.map)" --specs=nano.specs --specs=nosys.specs -o $@ $^ $(LIBS)

$(TARGET).hex: $(TARGET).elf
	$(BIN)/$(BIN_PREFIX)-objcopy -O ihex "$<" "$@"

$(TARGET).siz: $(TARGET).elf
	$(BIN)/$(BIN_PREFIX)-size --format=berkeley $^

$(TARGET).lst: $(TARGET).elf
	$(BIN)/$(BIN_PREFIX)-objdump --source --all-headers --demangle --line-numbers --wide $^ > $@

flash: $(TARGET).hex
	sudo ../../WCHISPTool_CMD -p /dev/ch37x0 -c $(CHIP_INI) -o program -f $<

.PHONY: version clean flash lint
