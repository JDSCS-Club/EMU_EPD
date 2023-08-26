################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/radio/radio.c \
../Drivers/radio/radio_comm.c \
../Drivers/radio/radio_hal.c 

C_DEPS += \
./Drivers/radio/radio.d \
./Drivers/radio/radio_comm.d \
./Drivers/radio/radio_hal.d 

OBJS += \
./Drivers/radio/radio.o \
./Drivers/radio/radio_comm.o \
./Drivers/radio/radio_hal.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/radio/%.o Drivers/radio/%.su Drivers/radio/%.cyclo: ../Drivers/radio/%.c Drivers/radio/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DHAVE_CONFIG_H -DDEBUG -DSTM32F407xx -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/radio -I../Drivers/radio/Si446x -I../Drivers/si4463 -I../Library -I../Library/speex -I../Library/speex/include -I../Library/speex/STM32 -I../Library/speex/STM32/include -I../Library/speex/STM32/libspeex/gcc -I../Library/speex/libspeex -I../Library/speex/include/speex -I../Drivers/display -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-radio

clean-Drivers-2f-radio:
	-$(RM) ./Drivers/radio/radio.cyclo ./Drivers/radio/radio.d ./Drivers/radio/radio.o ./Drivers/radio/radio.su ./Drivers/radio/radio_comm.cyclo ./Drivers/radio/radio_comm.d ./Drivers/radio/radio_comm.o ./Drivers/radio/radio_comm.su ./Drivers/radio/radio_hal.cyclo ./Drivers/radio/radio_hal.d ./Drivers/radio/radio_hal.o ./Drivers/radio/radio_hal.su

.PHONY: clean-Drivers-2f-radio

