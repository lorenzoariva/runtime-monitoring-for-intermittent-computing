################################################################################
# Automatically-generated file. Do not edit!
################################################################################

CG_TOOL_ROOT := /Users/lorenzo/ti/msp430-gcc-9.3.0.31_macos

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./application.o" \
"./main.o" \
"./monitor/monitor.o" \
"./tiny-ink/boot.o" \
"./tiny-ink/mcu/msp430/dma.o" \
"./tiny-ink/mcu/msp430/msp430fr5969.o" \
"./tiny-ink/scheduler/priority.o" \
"./tiny-ink/scheduler/scheduler.o" \
"./tiny-ink/scheduler/thread.o" \
$(GEN_CMDS__FLAG) \
-Wl,--start-group \
-lgcc \
-lc \
-Wl,--end-group \

-include ../makefile.init

RM := rm -rf
RMDIR := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include monitor/subdir_vars.mk
-include tiny-ink/subdir_vars.mk
-include tiny-ink/mcu/msp430/subdir_vars.mk
-include tiny-ink/scheduler/subdir_vars.mk
-include subdir_rules.mk
-include monitor/subdir_rules.mk
-include tiny-ink/subdir_rules.mk
-include tiny-ink/mcu/msp430/subdir_rules.mk
-include tiny-ink/scheduler/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
ink-thesis.out \

EXE_OUTPUTS__QUOTED += \
"ink-thesis.out" \

BIN_OUTPUTS += \
ink-thesis.hex \

BIN_OUTPUTS__QUOTED += \
"ink-thesis.hex" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "ink-thesis.out"

# Tool invocations
ink-thesis.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: GNU Linker'
	"/Users/lorenzo/ti/msp430-gcc-9.3.0.31_macos/bin/msp430-elf-gcc-9.3.0" -mhwmult=f5series -Og -g -gdwarf-3 -gstrict-dwarf -Wall -mcode-region=none -mlarge -Wl,-Map,"ink-thesis.map" -Wl,--gc-sections -L"/Applications/ti/ccs1200/ccs/ccs_base/msp430/include_gcc" -mmcu=msp430fr5969 -o"ink-thesis.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

ink-thesis.hex: $(EXE_OUTPUTS)
	@echo 'Building secondary target: "$@"'
	@echo 'Invoking: GNU Objcopy Utility'
	"/Users/lorenzo/ti/msp430-gcc-9.3.0.31_macos/bin/msp430-elf-objcopy"  $(EXE_OUTPUTS__QUOTED) "ink-thesis.hex"
	@echo 'Finished building secondary target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "application.o" "main.o" "monitor/monitor.o" "tiny-ink/boot.o" "tiny-ink/mcu/msp430/dma.o" "tiny-ink/mcu/msp430/msp430fr5969.o" "tiny-ink/scheduler/priority.o" "tiny-ink/scheduler/scheduler.o" "tiny-ink/scheduler/thread.o" 
	-$(RM) "application.d" "main.d" "monitor/monitor.d" "tiny-ink/boot.d" "tiny-ink/mcu/msp430/dma.d" "tiny-ink/mcu/msp430/msp430fr5969.d" "tiny-ink/scheduler/priority.d" "tiny-ink/scheduler/scheduler.d" "tiny-ink/scheduler/thread.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

