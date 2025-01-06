################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Modbus/MB_CRC.c \
../Core/Src/Modbus/MB_Handler.c \
../Core/Src/Modbus/MB_RTUSlave.c \
../Core/Src/Modbus/MB_Serial.c 

OBJS += \
./Core/Src/Modbus/MB_CRC.o \
./Core/Src/Modbus/MB_Handler.o \
./Core/Src/Modbus/MB_RTUSlave.o \
./Core/Src/Modbus/MB_Serial.o 

C_DEPS += \
./Core/Src/Modbus/MB_CRC.d \
./Core/Src/Modbus/MB_Handler.d \
./Core/Src/Modbus/MB_RTUSlave.d \
./Core/Src/Modbus/MB_Serial.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Modbus/%.o Core/Src/Modbus/%.su Core/Src/Modbus/%.cyclo: ../Core/Src/Modbus/%.c Core/Src/Modbus/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"D:/Projects/Outsourcing/Fiverr/Clients/teck2024/PTCTemp/ModbusIO_STM32_FW/ModbusIOBoard_F446RE/Core/Src/Modbus" -I"D:/Projects/Outsourcing/Fiverr/Clients/teck2024/PTCTemp/ModbusIO_STM32_FW/ModbusIOBoard_F446RE/Core/Src/Filters" -I"D:/Projects/Outsourcing/Fiverr/Clients/teck2024/PTCTemp/ModbusIO_STM32_FW/ModbusIOBoard_F446RE/Core/Src/Debug" -I"D:/Projects/Outsourcing/Fiverr/Clients/teck2024/PTCTemp/ModbusIO_STM32_FW/ModbusIOBoard_F446RE/Core/Src/Debounce" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Modbus

clean-Core-2f-Src-2f-Modbus:
	-$(RM) ./Core/Src/Modbus/MB_CRC.cyclo ./Core/Src/Modbus/MB_CRC.d ./Core/Src/Modbus/MB_CRC.o ./Core/Src/Modbus/MB_CRC.su ./Core/Src/Modbus/MB_Handler.cyclo ./Core/Src/Modbus/MB_Handler.d ./Core/Src/Modbus/MB_Handler.o ./Core/Src/Modbus/MB_Handler.su ./Core/Src/Modbus/MB_RTUSlave.cyclo ./Core/Src/Modbus/MB_RTUSlave.d ./Core/Src/Modbus/MB_RTUSlave.o ./Core/Src/Modbus/MB_RTUSlave.su ./Core/Src/Modbus/MB_Serial.cyclo ./Core/Src/Modbus/MB_Serial.d ./Core/Src/Modbus/MB_Serial.o ./Core/Src/Modbus/MB_Serial.su

.PHONY: clean-Core-2f-Src-2f-Modbus

