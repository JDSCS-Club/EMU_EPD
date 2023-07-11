################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Library/VerInfo.c \
../Library/bootloader.c \
../Library/flash_if.c \
../Library/nvram.c \
../Library/version.c 

OBJS += \
./Library/VerInfo.o \
./Library/bootloader.o \
./Library/flash_if.o \
./Library/nvram.o \
./Library/version.o 

C_DEPS += \
./Library/VerInfo.d \
./Library/bootloader.d \
./Library/flash_if.d \
./Library/nvram.d \
./Library/version.d 


# Each subdirectory must supply rules for building sources it contributes
Library/%.o Library/%.su Library/%.cyclo: ../Library/%.c Library/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DUSE_BOOTLOADER -DSTM32F103xE -DUSE_HAL_DRIVER -c -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Library -I../Core/Inc_Boot -I../Core/Inc -O3 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Library

clean-Library:
	-$(RM) ./Library/VerInfo.cyclo ./Library/VerInfo.d ./Library/VerInfo.o ./Library/VerInfo.su ./Library/bootloader.cyclo ./Library/bootloader.d ./Library/bootloader.o ./Library/bootloader.su ./Library/flash_if.cyclo ./Library/flash_if.d ./Library/flash_if.o ./Library/flash_if.su ./Library/nvram.cyclo ./Library/nvram.d ./Library/nvram.o ./Library/nvram.su ./Library/version.cyclo ./Library/version.d ./Library/version.o ./Library/version.su

.PHONY: clean-Library

