################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32L4xx_HAL_Driver/Src/subdir.mk
-include Core/srccpp/WifiEsp8266/subdir.mk
-include Core/srccpp/Shift/subdir.mk
-include Core/srccpp/OfflineStorage/subdir.mk
-include Core/srccpp/Modbus/subdir.mk
-include Core/srccpp/Ethernet/subdir.mk
-include Core/srccpp/DisplayCPP/subdir.mk
-include Core/srccpp/Common/subdir.mk
-include Core/srccpp/BussinessLogic/subdir.mk
-include Core/srccpp/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/W5500_C/W5500/subdir.mk
-include Core/Src/W5500_C/subdir.mk
-include Core/Src/W25Q64/subdir.mk
-include Core/Src/RTC/subdir.mk
-include Core/Src/Internet/DNS/subdir.mk
-include Core/Src/Internet/DHCP/subdir.mk
-include Core/Src/ESP8266/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := powermeter
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
powermeter.elf \

MAP_FILES += \
powermeter.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
powermeter.list \


# All Target
all: main-build

# Main-build Target
main-build: powermeter.elf secondary-outputs

# Tool invocations
powermeter.elf powermeter.map: $(OBJS) $(USER_OBJS) D:\PROGRAM\Naveen\Powermeter\Sandfits_9-08-2023\Sandfits_09-08-2023\STM32L433RCTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-g++ -o "powermeter.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"D:\PROGRAM\Naveen\Powermeter\Sandfits_9-08-2023\Sandfits_09-08-2023\STM32L433RCTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="powermeter.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

powermeter.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "powermeter.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) default.size.stdout powermeter.elf powermeter.list powermeter.map
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
