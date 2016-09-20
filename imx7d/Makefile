include ../py/mkenv.mk

CROSS = 0

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk

ifeq ($(CROSS), 1)
CROSS_COMPILE = arm-none-eabi-
endif

INC += -I.
INC += -I..
INC += -I../lib/mp-readline
INC += -I./boards/imx7_colibri_m4/
INC += -I./platform/devices/MCIMX7D/startup/
INC += -I./platform/devices/
INC += -I./platform/CMSIS/Include/
INC += -I./platform/drivers/inc/
INC += -I./platform/utilities/inc/
INC += -I$(BUILD)

ifeq ($(CROSS), 1)
DFU = ../tools/dfu.py
PYDFU = ../tools/pydfu.py
CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mabi=aapcs-linux -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fsingle-precision-constant -Wdouble-promotion
CFLAGS_CORTEX_M4 += -DCPU_MCIMX7D_M4
CFLAGS = $(INC) -Wall -Werror -ansi -std=gnu99 -nostdlib $(CFLAGS_CORTEX_M4) $(COPT)
else
CFLAGS = -m32 $(INC) -Wall -Werror -ansi -std=gnu99 $(COPT)
endif

#Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -ggdb
else
CFLAGS += -Os -DNDEBUG
endif

ifeq ($(CROSS), 1)
LDFLAGS = -nostdlib -T imx7d_ocram.ld -Map=$@.map --cref
else
LD = gcc
LDFLAGS = -m32 -Wl,-Map=$@.map,--cref
endif
LIBS =

SRC_C = \
	main.c \
	uart_core.c \
	boards/imx7_colibri_m4/board.c \
	boards/imx7_colibri_m4/pin_mux.c \
	boards/imx7_colibri_m4/clock_freq.c \
	lib/utils/stdout_helpers.c \
	lib/utils/pyexec.c \
	lib/libc/string0.c \
	lib/mp-readline/readline.c \
	$(BUILD)/_frozen_mpy.c \

SRC_DRV_C = \
	platform/drivers/src/ccm_analog_imx7d.c \
	platform/drivers/src/ccm_imx7d.c \
	platform/drivers/src/uart_imx.c \
	platform/utilities/src/debug_console_imx.c \
	platform/utilities/src/print_scan.c \

OBJ = $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_DRV_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

ifeq ($(CROSS), 1)
all: $(BUILD)/firmware.dfu
else
all: $(BUILD)/firmware.elf
endif

$(BUILD)/_frozen_mpy.c: frozentest.mpy $(BUILD)/genhdr/qstrdefs.generated.h
	$(ECHO) "MISC freezing bytecode"
	$(Q)../tools/mpy-tool.py -f -q $(BUILD)/genhdr/qstrdefs.preprocessed.h -mlongint-impl=none $< > $@

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.dfu: $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary -j .isr_vector -j .text -j .data $^ $(BUILD)/firmware.bin
	$(Q)$(PYTHON) $(DFU) -b 0x08000000:$(BUILD)/firmware.bin $@

deploy: $(BUILD)/firmware.dfu
	$(ECHO) "Writing $< to the board"
	$(Q)$(PYTHON) $(PYDFU) -u $<

# Run emulation build on a POSIX system with suitable terminal settings
run:
	stty raw opost -echo
	build/firmware.elf
	@echo Resetting terminal...
# This sleep is useful to spot segfaults
	sleep 1
	reset

test: $(BUILD)/firmware.elf
	$(Q)/bin/echo -e "print('hello world!', list(x+1 for x in range(10)), end='eol\\\\n')\\r\\n\\x04" | $(BUILD)/firmware.elf | tail -n2 | grep "^hello world! \\[1, 2, 3, 4, 5, 6, 7, 8, 9, 10\\]eol"

include ../py/mkrules.mk
