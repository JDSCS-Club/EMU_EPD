################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/display/fonts/PHanFont.c \
../Drivers/display/fonts/PHanFontEng.c \
../Drivers/display/fonts/PHan_Lib.c 

C_DEPS += \
./Drivers/display/fonts/PHanFont.d \
./Drivers/display/fonts/PHanFontEng.d \
./Drivers/display/fonts/PHan_Lib.d 

OBJS += \
./Drivers/display/fonts/PHanFont.o \
./Drivers/display/fonts/PHanFontEng.o \
./Drivers/display/fonts/PHan_Lib.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/display/fonts/%.o Drivers/display/fonts/%.su Drivers/display/fonts/%.cyclo: ../Drivers/display/fonts/%.c Drivers/display/fonts/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DHAVE_CONFIG_H -DDEBUG -DSTM32F407xx -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/radio -I../Drivers/radio/Si446x -I../Drivers/si4463 -I../Library -I../Library/speex -I../Library/speex/include -I../Library/speex/STM32 -I../Library/speex/STM32/include -I../Library/speex/STM32/libspeex/gcc -I../Library/speex/libspeex -I../Library/speex/include/speex -I../Drivers/display -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-display-2f-fonts

clean-Drivers-2f-display-2f-fonts:
	-$(RM) ./Drivers/display/fonts/PHanFont.cyclo ./Drivers/display/fonts/PHanFont.d ./Drivers/display/fonts/PHanFont.o ./Drivers/display/fonts/PHanFont.su ./Drivers/display/fonts/PHanFontEng.cyclo ./Drivers/display/fonts/PHanFontEng.d ./Drivers/display/fonts/PHanFontEng.o ./Drivers/display/fonts/PHanFontEng.su ./Drivers/display/fonts/PHan_Lib.cyclo ./Drivers/display/fonts/PHan_Lib.d ./Drivers/display/fonts/PHan_Lib.o ./Drivers/display/fonts/PHan_Lib.su

.PHONY: clean-Drivers-2f-display-2f-fonts

