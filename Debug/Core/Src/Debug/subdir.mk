################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Debug/Debug.c 

OBJS += \
./Core/Src/Debug/Debug.o 

C_DEPS += \
./Core/Src/Debug/Debug.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Debug/%.o Core/Src/Debug/%.su Core/Src/Debug/%.cyclo: ../Core/Src/Debug/%.c Core/Src/Debug/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"D:/Projects/Outsourcing/Fiverr/Clients/teck2024/PTCTemp/ModbusIO_STM32_FW/ModbusIOBoard_F446RE/Core/Src/Modbus" -I"D:/Projects/Outsourcing/Fiverr/Clients/teck2024/PTCTemp/ModbusIO_STM32_FW/ModbusIOBoard_F446RE/Core/Src/Filters" -I"D:/Projects/Outsourcing/Fiverr/Clients/teck2024/PTCTemp/ModbusIO_STM32_FW/ModbusIOBoard_F446RE/Core/Src/Debug" -I"D:/Projects/Outsourcing/Fiverr/Clients/teck2024/PTCTemp/ModbusIO_STM32_FW/ModbusIOBoard_F446RE/Core/Src/Debounce" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Debug

clean-Core-2f-Src-2f-Debug:
	-$(RM) ./Core/Src/Debug/Debug.cyclo ./Core/Src/Debug/Debug.d ./Core/Src/Debug/Debug.o ./Core/Src/Debug/Debug.su

.PHONY: clean-Core-2f-Src-2f-Debug

