################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/drivers/src/encoder.c \
../source/drivers/src/leds.c \
../source/drivers/src/magDriver.c 

OBJS += \
./source/drivers/src/encoder.o \
./source/drivers/src/leds.o \
./source/drivers/src/magDriver.o 

C_DEPS += \
./source/drivers/src/encoder.d \
./source/drivers/src/leds.d \
./source/drivers/src/magDriver.d 


# Each subdirectory must supply rules for building sources it contributes
source/drivers/src/%.o: ../source/drivers/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DDEBUG -DSDK_DEBUGCONSOLE=0 -I../source -I../ -I../SDK/CMSIS -I../SDK/startup -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


