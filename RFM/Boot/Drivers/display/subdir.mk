################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Drivers/display/Adafruit_GFX.cpp \
../Drivers/display/Adafruit_SSD1306.cpp 

OBJS += \
./Drivers/display/Adafruit_GFX.o \
./Drivers/display/Adafruit_SSD1306.o 

CPP_DEPS += \
./Drivers/display/Adafruit_GFX.d \
./Drivers/display/Adafruit_SSD1306.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/display/%.o Drivers/display/%.su Drivers/display/%.cyclo: ../Drivers/display/%.cpp Drivers/display/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DUSE_HAL_DRIVER -DUSE_BOOTLOADER -DHAVE_CONFIG_H -DDEBUG -DSTM32F407xx -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Library -I../Drivers/display -I../Drivers/display/fonts -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-display

clean-Drivers-2f-display:
	-$(RM) ./Drivers/display/Adafruit_GFX.cyclo ./Drivers/display/Adafruit_GFX.d ./Drivers/display/Adafruit_GFX.o ./Drivers/display/Adafruit_GFX.su ./Drivers/display/Adafruit_SSD1306.cyclo ./Drivers/display/Adafruit_SSD1306.d ./Drivers/display/Adafruit_SSD1306.o ./Drivers/display/Adafruit_SSD1306.su

.PHONY: clean-Drivers-2f-display

