################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/radio/Si446x/si446x_api_lib.c \
../Drivers/radio/Si446x/si446x_nirq.c 

C_DEPS += \
./Drivers/radio/Si446x/si446x_api_lib.d \
./Drivers/radio/Si446x/si446x_nirq.d 

OBJS += \
./Drivers/radio/Si446x/si446x_api_lib.o \
./Drivers/radio/Si446x/si446x_nirq.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/radio/Si446x/%.o Drivers/radio/Si446x/%.su Drivers/radio/Si446x/%.cyclo: ../Drivers/radio/Si446x/%.c Drivers/radio/Si446x/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DHAVE_CONFIG_H -DDEBUG -DSTM32F407xx -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/radio -I../Drivers/radio/Si446x -I../Drivers/si4463 -I../Library -I../Library/speex -I../Library/speex/include -I../Library/speex/STM32 -I../Library/speex/STM32/include -I../Library/speex/STM32/libspeex/gcc -I../Library/speex/libspeex -I../Library/speex/include/speex -I../Drivers/display -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-radio-2f-Si446x

clean-Drivers-2f-radio-2f-Si446x:
	-$(RM) ./Drivers/radio/Si446x/si446x_api_lib.cyclo ./Drivers/radio/Si446x/si446x_api_lib.d ./Drivers/radio/Si446x/si446x_api_lib.o ./Drivers/radio/Si446x/si446x_api_lib.su ./Drivers/radio/Si446x/si446x_nirq.cyclo ./Drivers/radio/Si446x/si446x_nirq.d ./Drivers/radio/Si446x/si446x_nirq.o ./Drivers/radio/Si446x/si446x_nirq.su

.PHONY: clean-Drivers-2f-radio-2f-Si446x

