################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/module/timmer_model.c \
../Core/module/uart_module.c 

OBJS += \
./Core/module/timmer_model.o \
./Core/module/uart_module.o 

C_DEPS += \
./Core/module/timmer_model.d \
./Core/module/uart_module.d 


# Each subdirectory must supply rules for building sources it contributes
Core/module/%.o: ../Core/module/%.c Core/module/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103x6 -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/can -I../Core/module -I../Core/Includes -I../Core/protocol -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

