################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Library/ProcPkt.c \
../Library/QBuf.c \
../Library/RFMProtocol.c \
../Library/VerInfo.c \
../Library/adc.c \
../Library/adpcm.c \
../Library/audio.c \
../Library/bootloader.c \
../Library/cli.c \
../Library/diag.c \
../Library/eeprom.c \
../Library/flash_if.c \
../Library/iap_common.c \
../Library/iap_menu.c \
../Library/keypad.c \
../Library/menu.c \
../Library/rfm.c \
../Library/serial.c \
../Library/unicode.c \
../Library/upgrade.c \
../Library/version.c \
../Library/ymodem.c 

C_DEPS += \
./Library/ProcPkt.d \
./Library/QBuf.d \
./Library/RFMProtocol.d \
./Library/VerInfo.d \
./Library/adc.d \
./Library/adpcm.d \
./Library/audio.d \
./Library/bootloader.d \
./Library/cli.d \
./Library/diag.d \
./Library/eeprom.d \
./Library/flash_if.d \
./Library/iap_common.d \
./Library/iap_menu.d \
./Library/keypad.d \
./Library/menu.d \
./Library/rfm.d \
./Library/serial.d \
./Library/unicode.d \
./Library/upgrade.d \
./Library/version.d \
./Library/ymodem.d 

OBJS += \
./Library/ProcPkt.o \
./Library/QBuf.o \
./Library/RFMProtocol.o \
./Library/VerInfo.o \
./Library/adc.o \
./Library/adpcm.o \
./Library/audio.o \
./Library/bootloader.o \
./Library/cli.o \
./Library/diag.o \
./Library/eeprom.o \
./Library/flash_if.o \
./Library/iap_common.o \
./Library/iap_menu.o \
./Library/keypad.o \
./Library/menu.o \
./Library/rfm.o \
./Library/serial.o \
./Library/unicode.o \
./Library/upgrade.o \
./Library/version.o \
./Library/ymodem.o 


# Each subdirectory must supply rules for building sources it contributes
Library/%.o Library/%.su Library/%.cyclo: ../Library/%.c Library/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DHAVE_CONFIG_H -DDEBUG -DSTM32F407xx -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/radio -I../Drivers/radio/Si446x -I../Drivers/si4463 -I../Library -I../Library/speex -I../Library/speex/include -I../Library/speex/STM32 -I../Library/speex/STM32/include -I../Library/speex/STM32/libspeex/gcc -I../Library/speex/libspeex -I../Library/speex/include/speex -I../Drivers/display -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Library

clean-Library:
	-$(RM) ./Library/ProcPkt.cyclo ./Library/ProcPkt.d ./Library/ProcPkt.o ./Library/ProcPkt.su ./Library/QBuf.cyclo ./Library/QBuf.d ./Library/QBuf.o ./Library/QBuf.su ./Library/RFMProtocol.cyclo ./Library/RFMProtocol.d ./Library/RFMProtocol.o ./Library/RFMProtocol.su ./Library/VerInfo.cyclo ./Library/VerInfo.d ./Library/VerInfo.o ./Library/VerInfo.su ./Library/adc.cyclo ./Library/adc.d ./Library/adc.o ./Library/adc.su ./Library/adpcm.cyclo ./Library/adpcm.d ./Library/adpcm.o ./Library/adpcm.su ./Library/audio.cyclo ./Library/audio.d ./Library/audio.o ./Library/audio.su ./Library/bootloader.cyclo ./Library/bootloader.d ./Library/bootloader.o ./Library/bootloader.su ./Library/cli.cyclo ./Library/cli.d ./Library/cli.o ./Library/cli.su ./Library/diag.cyclo ./Library/diag.d ./Library/diag.o ./Library/diag.su ./Library/eeprom.cyclo ./Library/eeprom.d ./Library/eeprom.o ./Library/eeprom.su ./Library/flash_if.cyclo ./Library/flash_if.d ./Library/flash_if.o ./Library/flash_if.su ./Library/iap_common.cyclo ./Library/iap_common.d ./Library/iap_common.o ./Library/iap_common.su ./Library/iap_menu.cyclo ./Library/iap_menu.d ./Library/iap_menu.o ./Library/iap_menu.su ./Library/keypad.cyclo ./Library/keypad.d ./Library/keypad.o ./Library/keypad.su ./Library/menu.cyclo ./Library/menu.d ./Library/menu.o ./Library/menu.su ./Library/rfm.cyclo ./Library/rfm.d ./Library/rfm.o ./Library/rfm.su ./Library/serial.cyclo ./Library/serial.d ./Library/serial.o ./Library/serial.su ./Library/unicode.cyclo ./Library/unicode.d ./Library/unicode.o ./Library/unicode.su ./Library/upgrade.cyclo ./Library/upgrade.d ./Library/upgrade.o ./Library/upgrade.su ./Library/version.cyclo ./Library/version.d ./Library/version.o ./Library/version.su ./Library/ymodem.cyclo ./Library/ymodem.d ./Library/ymodem.o ./Library/ymodem.su

.PHONY: clean-Library

