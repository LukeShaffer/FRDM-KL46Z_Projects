################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include utilities/subdir.mk
-include startup/subdir.mk
-include source/subdir.mk
-include drivers/subdir.mk
-include board/subdir.mk
-include CMSIS/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: MKL46Z256xxx4_Project3.axf

# Tool invocations
MKL46Z256xxx4_Project3.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -Xlinker -Map="MKL46Z256xxx4_Project3.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -Xlinker --sort-section=alignment -Xlinker --cref -mcpu=cortex-m0plus -mthumb -T MKL46Z256xxx4_Project3_Debug.ld -o "MKL46Z256xxx4_Project3.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) MKL46Z256xxx4_Project3.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "MKL46Z256xxx4_Project3.axf"; # arm-none-eabi-objcopy -v -O binary "MKL46Z256xxx4_Project3.axf" "MKL46Z256xxx4_Project3.bin" ; # checksum -p MKL46Z256xxx4 -d "MKL46Z256xxx4_Project3.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
