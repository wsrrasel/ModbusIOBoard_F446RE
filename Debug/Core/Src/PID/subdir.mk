################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/PID/PID_Controller.c 

OBJS += \
./Core/Src/PID/PID_Controller.o 

C_DEPS += \
./Core/Src/PID/PID_Controller.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/PID/%.o Core/Src/PID/%.su Core/Src/PID/%.cyclo: ../Core/Src/PID/%.c Core/Src/PID/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"D:/Work/ProjectWork/ModbusRTUSlave/SourceCode/ModbusRTUSlave_V1.0/Core/Src/Modbus" -I"D:/Work/ProjectWork/ModbusRTUSlave/SourceCode/ModbusRTUSlave_V1.0/Core/Src/Filters" -I"D:/Work/ProjectWork/ModbusRTUSlave/SourceCode/ModbusRTUSlave_V1.0/Core/Src/Encoder" -I"D:/Work/ProjectWork/ModbusRTUSlave/SourceCode/ModbusRTUSlave_V1.0/Core/Src/Debug" -I"D:/Work/ProjectWork/ModbusRTUSlave/SourceCode/ModbusRTUSlave_V1.0/Core/Src/EEPROM" -I"D:/Work/ProjectWork/ModbusRTUSlave/SourceCode/ModbusRTUSlave_V1.0/Core/Src/PID" -I"D:/Work/ProjectWork/ModbusRTUSlave/SourceCode/ModbusRTUSlave_V1.0/Core/Src/PWM" -I"D:/Work/ProjectWork/ModbusRTUSlave/SourceCode/ModbusRTUSlave_V1.0/Core/Src/Delay_us" -I"D:/Work/ProjectWork/ModbusRTUSlave/SourceCode/ModbusRTUSlave_V1.0/Core/Src/PLock" -I"D:/Work/ProjectWork/ModbusRTUSlave/SourceCode/ModbusRTUSlave_V1.0/Core/Src/Lift" -I"D:/Work/ProjectWork/ModbusRTUSlave/SourceCode/ModbusRTUSlave_V1.0/Core/Src/Debounce" -I"D:/Work/ProjectWork/ModbusRTUSlave/SourceCode/ModbusRTUSlave_V1.0/Core/Src/MAB" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-PID

clean-Core-2f-Src-2f-PID:
	-$(RM) ./Core/Src/PID/PID_Controller.cyclo ./Core/Src/PID/PID_Controller.d ./Core/Src/PID/PID_Controller.o ./Core/Src/PID/PID_Controller.su

.PHONY: clean-Core-2f-Src-2f-PID

