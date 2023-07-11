################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Library/QBuf.c \
../Library/VerInfo.c \
../Library/adc.c \
../Library/bootloader.c \
../Library/cli.c \
../Library/flash_if.c \
../Library/iap_common.c \
../Library/iap_menu.c \
../Library/nvram.c \
../Library/occpa.c \
../Library/rs485.c \
../Library/serial.c \
../Library/version.c \
../Library/ymodem.c 

OBJS += \
./Library/QBuf.o \
./Library/VerInfo.o \
./Library/adc.o \
./Library/bootloader.o \
./Library/cli.o \
./Library/flash_if.o \
./Library/iap_common.o \
./Library/iap_menu.o \
./Library/nvram.o \
./Library/occpa.o \
./Library/rs485.o \
./Library/serial.o \
./Library/version.o \
./Library/ymodem.o 

C_DEPS += \
./Library/QBuf.d \
./Library/VerInfo.d \
./Library/adc.d \
./Library/bootloader.d \
./Library/cli.d \
./Library/flash_if.d \
./Library/iap_common.d \
./Library/iap_menu.d \
./Library/nvram.d \
./Library/occpa.d \
./Library/rs485.d \
./Library/serial.d \
./Library/version.d \
./Library/ymodem.d 


# Each subdirectory must supply rules for building sources it contributes
Library/%.o Library/%.su Library/%.cyclo: ../Library/%.c Library/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32F103xE -DUSE_HAL_DRIVER -DDEBUG -c -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Library -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Library

clean-Library:
	-$(RM) ./Library/QBuf.cyclo ./Library/QBuf.d ./Library/QBuf.o ./Library/QBuf.su ./Library/VerInfo.cyclo ./Library/VerInfo.d ./Library/VerInfo.o ./Library/VerInfo.su ./Library/adc.cyclo ./Library/adc.d ./Library/adc.o ./Library/adc.su ./Library/bootloader.cyclo ./Library/bootloader.d ./Library/bootloader.o ./Library/bootloader.su ./Library/cli.cyclo ./Library/cli.d ./Library/cli.o ./Library/cli.su ./Library/flash_if.cyclo ./Library/flash_if.d ./Library/flash_if.o ./Library/flash_if.su ./Library/iap_common.cyclo ./Library/iap_common.d ./Library/iap_common.o ./Library/iap_common.su ./Library/iap_menu.cyclo ./Library/iap_menu.d ./Library/iap_menu.o ./Library/iap_menu.su ./Library/nvram.cyclo ./Library/nvram.d ./Library/nvram.o ./Library/nvram.su ./Library/occpa.cyclo ./Library/occpa.d ./Library/occpa.o ./Library/occpa.su ./Library/rs485.cyclo ./Library/rs485.d ./Library/rs485.o ./Library/rs485.su ./Library/serial.cyclo ./Library/serial.d ./Library/serial.o ./Library/serial.su ./Library/version.cyclo ./Library/version.d ./Library/version.o ./Library/version.su ./Library/ymodem.cyclo ./Library/ymodem.d ./Library/ymodem.o ./Library/ymodem.su

.PHONY: clean-Library

