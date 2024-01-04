################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Dwin/dwin_receive_decode.c 

C_DEPS += \
./Core/Src/Dwin/dwin_receive_decode.d 

OBJS += \
./Core/Src/Dwin/dwin_receive_decode.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Dwin/%.o Core/Src/Dwin/%.su: ../Core/Src/Dwin/%.c Core/Src/Dwin/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L433xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/SourceTree/onlinepowermerter/Core/Src" -I"D:/SourceTree/onlinepowermerter/Core/Src/ESP8266" -I"D:/SourceTree/onlinepowermerter/Core/Src/Internet/DHCP" -I"D:/SourceTree/onlinepowermerter/Core/Src/Internet/DNS" -I"D:/SourceTree/onlinepowermerter/Core/Src/RTC" -I"D:/SourceTree/onlinepowermerter/Core/Src/W25Q64" -I"D:/SourceTree/onlinepowermerter/Core/Src/W5500_C" -I"D:/SourceTree/onlinepowermerter/Core/Src/api" -I"D:/SourceTree/onlinepowermerter/Core/Src/Dwin" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Dwin

clean-Core-2f-Src-2f-Dwin:
	-$(RM) ./Core/Src/Dwin/dwin_receive_decode.d ./Core/Src/Dwin/dwin_receive_decode.o ./Core/Src/Dwin/dwin_receive_decode.su

.PHONY: clean-Core-2f-Src-2f-Dwin

