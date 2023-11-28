################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/srccpp/WifiEsp8266/ESP8266.cpp 

OBJS += \
./Core/srccpp/WifiEsp8266/ESP8266.o 

CPP_DEPS += \
./Core/srccpp/WifiEsp8266/ESP8266.d 


# Each subdirectory must supply rules for building sources it contributes
Core/srccpp/WifiEsp8266/%.o Core/srccpp/WifiEsp8266/%.su: ../Core/srccpp/WifiEsp8266/%.cpp Core/srccpp/WifiEsp8266/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -DUSE_HAL_DRIVER -DSTM32L433xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/PROGRAM/Naveen/Powermeter/Sandfits_9-08-2023/Sandfits_09-08-2023/Core/srccpp/BussinessLogic" -I"D:/PROGRAM/Naveen/Powermeter/Sandfits_9-08-2023/Sandfits_09-08-2023/Core/srccpp/Common" -I"D:/PROGRAM/Naveen/Powermeter/Sandfits_9-08-2023/Sandfits_09-08-2023/Core/srccpp/DisplayCPP" -I"D:/PROGRAM/Naveen/Powermeter/Sandfits_9-08-2023/Sandfits_09-08-2023/Core/srccpp/Ethernet" -I"D:/PROGRAM/Naveen/Powermeter/Sandfits_9-08-2023/Sandfits_09-08-2023/Core/srccpp/Modbus" -I"D:/PROGRAM/Naveen/Powermeter/Sandfits_9-08-2023/Sandfits_09-08-2023/Core/srccpp/OfflineStorage" -I"D:/PROGRAM/Naveen/Powermeter/Sandfits_9-08-2023/Sandfits_09-08-2023/Core/srccpp/Shift" -I"D:/PROGRAM/Naveen/Powermeter/Sandfits_9-08-2023/Sandfits_09-08-2023/Core/srccpp/WifiEsp8266" -I"D:/PROGRAM/Naveen/Powermeter/Sandfits_9-08-2023/Sandfits_09-08-2023/Core/srccpp" -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-srccpp-2f-WifiEsp8266

clean-Core-2f-srccpp-2f-WifiEsp8266:
	-$(RM) ./Core/srccpp/WifiEsp8266/ESP8266.d ./Core/srccpp/WifiEsp8266/ESP8266.o ./Core/srccpp/WifiEsp8266/ESP8266.su

.PHONY: clean-Core-2f-srccpp-2f-WifiEsp8266

