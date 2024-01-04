################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Internet/DHCP/dhcp.c 

C_DEPS += \
./Core/Src/Internet/DHCP/dhcp.d 

OBJS += \
./Core/Src/Internet/DHCP/dhcp.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Internet/DHCP/%.o Core/Src/Internet/DHCP/%.su: ../Core/Src/Internet/DHCP/%.c Core/Src/Internet/DHCP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L433xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"E:/naveen Work space/Power Meter/stm32_dev/powermeter_06_05_23_B1/Core/Src" -I"E:/naveen Work space/Power Meter/stm32_dev/powermeter_06_05_23_B1/Core/Src/api" -I"E:/naveen Work space/Power Meter/stm32_dev/powermeter_06_05_23_B1/Core/Src/ESP8266" -I"E:/naveen Work space/Power Meter/stm32_dev/powermeter_06_05_23_B1/Core/Src/Internet/DHCP" -I"E:/naveen Work space/Power Meter/stm32_dev/powermeter_06_05_23_B1/Core/Src/Internet/DNS" -I"E:/naveen Work space/Power Meter/stm32_dev/powermeter_06_05_23_B1/Core/Src/RTC" -I"E:/naveen Work space/Power Meter/stm32_dev/powermeter_06_05_23_B1/Core/Src/W25Q64" -I"E:/naveen Work space/Power Meter/stm32_dev/powermeter_06_05_23_B1/Core/Src/W5500_C" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Internet-2f-DHCP

clean-Core-2f-Src-2f-Internet-2f-DHCP:
	-$(RM) ./Core/Src/Internet/DHCP/dhcp.d ./Core/Src/Internet/DHCP/dhcp.o ./Core/Src/Internet/DHCP/dhcp.su

.PHONY: clean-Core-2f-Src-2f-Internet-2f-DHCP

