################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
monitor/%.o: ../monitor/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/Users/lorenzo/ti/msp430-gcc-9.3.0.31_macos/bin/msp430-elf-gcc-9.3.0" -c -mmcu=msp430fr5969 -mhwmult=f5series -I"/Applications/ti/ccs1200/ccs/ccs_base/msp430/include_gcc" -I"/Users/lorenzo/Downloads/UNITN/DIDATTICA/Thesis/CCS/wspacedir/ink-thesis" -I"/Users/lorenzo/ti/msp430-gcc-9.3.0.31_macos/msp430-elf/include" -Og -g -gdwarf-3 -gstrict-dwarf -Wall -mlarge -mcode-region=none -mdata-region=lower -MMD -MP -MF"monitor/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


