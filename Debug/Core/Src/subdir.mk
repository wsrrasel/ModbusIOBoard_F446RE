################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Callbacks.c \
../Core/Src/DI.c \
../Core/Src/Flash_Storage.c \
../Core/Src/Scheduler.c \
../Core/Src/Settings.c \
../Core/Src/TaskHandler.c \
../Core/Src/TimeStamp.c \
../Core/Src/main.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c 

OBJS += \
./Core/Src/Callbacks.o \
./Core/Src/DI.o \
./Core/Src/Flash_Storage.o \
./Core/Src/Scheduler.o \
./Core/Src/Settings.o \
./Core/Src/TaskHandler.o \
./Core/Src/TimeStamp.o \
./Core/Src/main.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o 

C_DEPS += \
./Core/Src/Callbacks.d \
./Core/Src/DI.d \
./Core/Src/Flash_Storage.d \
./Core/Src/Scheduler.d \
./Core/Src/Settings.d \
./Core/Src/TaskHandler.d \
./Core/Src/TimeStamp.d \
./Core/Src/main.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"D:/Projects/Outsourcing/Fiverr/Clients/teck2024/PTCTemp/ModbusIO_STM32_FW/ModbusIOBoard_F446RE/Core/Src/Modbus" -I"D:/Projects/Outsourcing/Fiverr/Clients/teck2024/PTCTemp/ModbusIO_STM32_FW/ModbusIOBoard_F446RE/Core/Src/Filters" -I"D:/Projects/Outsourcing/Fiverr/Clients/teck2024/PTCTemp/ModbusIO_STM32_FW/ModbusIOBoard_F446RE/Core/Src/Debug" -I"D:/Projects/Outsourcing/Fiverr/Clients/teck2024/PTCTemp/ModbusIO_STM32_FW/ModbusIOBoard_F446RE/Core/Src/Debounce" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/Callbacks.cyclo ./Core/Src/Callbacks.d ./Core/Src/Callbacks.o ./Core/Src/Callbacks.su ./Core/Src/DI.cyclo ./Core/Src/DI.d ./Core/Src/DI.o ./Core/Src/DI.su ./Core/Src/Flash_Storage.cyclo ./Core/Src/Flash_Storage.d ./Core/Src/Flash_Storage.o ./Core/Src/Flash_Storage.su ./Core/Src/Scheduler.cyclo ./Core/Src/Scheduler.d ./Core/Src/Scheduler.o ./Core/Src/Scheduler.su ./Core/Src/Settings.cyclo ./Core/Src/Settings.d ./Core/Src/Settings.o ./Core/Src/Settings.su ./Core/Src/TaskHandler.cyclo ./Core/Src/TaskHandler.d ./Core/Src/TaskHandler.o ./Core/Src/TaskHandler.su ./Core/Src/TimeStamp.cyclo ./Core/Src/TimeStamp.d ./Core/Src/TimeStamp.o ./Core/Src/TimeStamp.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su

.PHONY: clean-Core-2f-Src

