################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Library/QBuf.c \
../Library/VerInfo.c \
../Library/adc.c \
../Library/adpcm.c \
../Library/audio.c \
../Library/bootloader.c \
../Library/cli.c \
../Library/eeprom.c \
../Library/flash_if.c \
../Library/iap_common.c \
../Library/iap_menu.c \
../Library/serial.c \
../Library/unicode.c \
../Library/version.c \
../Library/ymodem.c 

C_DEPS += \
./Library/QBuf.d \
./Library/VerInfo.d \
./Library/adc.d \
./Library/adpcm.d \
./Library/audio.d \
./Library/bootloader.d \
./Library/cli.d \
./Library/eeprom.d \
./Library/flash_if.d \
./Library/iap_common.d \
./Library/iap_menu.d \
./Library/serial.d \
./Library/unicode.d \
./Library/version.d \
./Library/ymodem.d 

OBJS += \
./Library/QBuf.o \
./Library/VerInfo.o \
./Library/adc.o \
./Library/adpcm.o \
./Library/audio.o \
./Library/bootloader.o \
./Library/cli.o \
./Library/eeprom.o \
./Library/flash_if.o \
./Library/iap_common.o \
./Library/iap_menu.o \
./Library/serial.o \
./Library/unicode.o \
./Library/version.o \
./Library/ymodem.o 


# Each subdirectory must supply rules for building sources it contributes
Library/%.o Library/%.su Library/%.cyclo: ../Library/%.c Library/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DHAVE_CONFIG_H -DDEBUG -DSTM32F407xx -DUSE_BOOTLOADER -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Library -I../Drivers/display -I../Drivers/display/fonts -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Library

clean-Library:
	-$(RM) ./Library/QBuf.cyclo ./Library/QBuf.d ./Library/QBuf.o ./Library/QBuf.su ./Library/VerInfo.cyclo ./Library/VerInfo.d ./Library/VerInfo.o ./Library/VerInfo.su ./Library/adc.cyclo ./Library/adc.d ./Library/adc.o ./Library/adc.su ./Library/adpcm.cyclo ./Library/adpcm.d ./Library/adpcm.o ./Library/adpcm.su ./Library/audio.cyclo ./Library/audio.d ./Library/audio.o ./Library/audio.su ./Library/bootloader.cyclo ./Library/bootloader.d ./Library/bootloader.o ./Library/bootloader.su ./Library/cli.cyclo ./Library/cli.d ./Library/cli.o ./Library/cli.su ./Library/eeprom.cyclo ./Library/eeprom.d ./Library/eeprom.o ./Library/eeprom.su ./Library/flash_if.cyclo ./Library/flash_if.d ./Library/flash_if.o ./Library/flash_if.su ./Library/iap_common.cyclo ./Library/iap_common.d ./Library/iap_common.o ./Library/iap_common.su ./Library/iap_menu.cyclo ./Library/iap_menu.d ./Library/iap_menu.o ./Library/iap_menu.su ./Library/serial.cyclo ./Library/serial.d ./Library/serial.o ./Library/serial.su ./Library/unicode.cyclo ./Library/unicode.d ./Library/unicode.o ./Library/unicode.su ./Library/version.cyclo ./Library/version.d ./Library/version.o ./Library/version.su ./Library/ymodem.cyclo ./Library/ymodem.d ./Library/ymodem.o ./Library/ymodem.su

.PHONY: clean-Library

