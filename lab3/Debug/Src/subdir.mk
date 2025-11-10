################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Src/maths.s 

C_SRCS += \
../Src/alarm.c \
../Src/bin.c \
../Src/calc.c \
../Src/debug.c \
../Src/display.c \
../Src/game.c \
../Src/gpio.c \
../Src/i2c.c \
../Src/main.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/systick.c \
../Src/touchpad.c 

OBJS += \
./Src/alarm.o \
./Src/bin.o \
./Src/calc.o \
./Src/debug.o \
./Src/display.o \
./Src/game.o \
./Src/gpio.o \
./Src/i2c.o \
./Src/main.o \
./Src/maths.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/systick.o \
./Src/touchpad.o 

S_DEPS += \
./Src/maths.d 

C_DEPS += \
./Src/alarm.d \
./Src/bin.d \
./Src/calc.d \
./Src/debug.d \
./Src/display.d \
./Src/game.d \
./Src/gpio.d \
./Src/i2c.d \
./Src/main.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/systick.d \
./Src/touchpad.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DSTM32L552xx -DSTM32 -DSTM32L5 -DSTM32L552ZETxQ -c -I../Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32L5xx/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/%.o: ../Src/%.s Src/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m33 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/alarm.cyclo ./Src/alarm.d ./Src/alarm.o ./Src/alarm.su ./Src/bin.cyclo ./Src/bin.d ./Src/bin.o ./Src/bin.su ./Src/calc.cyclo ./Src/calc.d ./Src/calc.o ./Src/calc.su ./Src/debug.cyclo ./Src/debug.d ./Src/debug.o ./Src/debug.su ./Src/display.cyclo ./Src/display.d ./Src/display.o ./Src/display.su ./Src/game.cyclo ./Src/game.d ./Src/game.o ./Src/game.su ./Src/gpio.cyclo ./Src/gpio.d ./Src/gpio.o ./Src/gpio.su ./Src/i2c.cyclo ./Src/i2c.d ./Src/i2c.o ./Src/i2c.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/maths.d ./Src/maths.o ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/systick.cyclo ./Src/systick.d ./Src/systick.o ./Src/systick.su ./Src/touchpad.cyclo ./Src/touchpad.d ./Src/touchpad.o ./Src/touchpad.su

.PHONY: clean-Src

