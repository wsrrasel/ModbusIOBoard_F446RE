################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/Modbus/subdir.mk
-include Core/Src/MH_Timer/subdir.mk
-include Core/Src/Filters/subdir.mk
-include Core/Src/Debug/subdir.mk
-include Core/Src/Debounce/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := ModbusIOBoard_F446RE
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
ModbusIOBoard_F446RE.elf \

MAP_FILES += \
ModbusIOBoard_F446RE.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
ModbusIOBoard_F446RE.list \

OBJCOPY_HEX += \
ModbusIOBoard_F446RE.hex \


# All Target
all: main-build

# Main-build Target
main-build: ModbusIOBoard_F446RE.elf secondary-outputs

# Tool invocations
ModbusIOBoard_F446RE.elf ModbusIOBoard_F446RE.map: $(OBJS) $(USER_OBJS) D:\Work\ProjectWork\ModbusIO\ModbusIOBoard_F446RE_FW\ModbusIOBoard_F446RE\STM32F446RETX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "ModbusIOBoard_F446RE.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"D:\Work\ProjectWork\ModbusIO\ModbusIOBoard_F446RE_FW\ModbusIOBoard_F446RE\STM32F446RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="ModbusIOBoard_F446RE.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -u _printf_float -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

ModbusIOBoard_F446RE.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "ModbusIOBoard_F446RE.list"
	@echo 'Finished building: $@'
	@echo ' '

ModbusIOBoard_F446RE.hex: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "ModbusIOBoard_F446RE.hex"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) ModbusIOBoard_F446RE.elf ModbusIOBoard_F446RE.hex ModbusIOBoard_F446RE.list ModbusIOBoard_F446RE.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
