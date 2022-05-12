################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/protocol/bat_can.c \
../Core/protocol/single_wire.c 

OBJS += \
./Core/protocol/bat_can.o \
./Core/protocol/single_wire.o 

C_DEPS += \
./Core/protocol/bat_can.d \
./Core/protocol/single_wire.d 


# Each subdirectory must supply rules for building sources it contributes
Core/protocol/bat_can.o: ../Core/protocol/bat_can.c Core/protocol/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103x6 -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/can -I../Core/module -I../Core/Includes -I../Core/protocol -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/protocol/bat_can.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Core/protocol/single_wire.o: ../Core/protocol/single_wire.c Core/protocol/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103x6 -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/can -I../Core/module -I../Core/Includes -I../Core/protocol -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/protocol/single_wire.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

