################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MCAL/external_interrupts.c \
../MCAL/gpio.c \
../MCAL/timer0.c \
../MCAL/timer1.c 

OBJS += \
./MCAL/external_interrupts.o \
./MCAL/gpio.o \
./MCAL/timer0.o \
./MCAL/timer1.o 

C_DEPS += \
./MCAL/external_interrupts.d \
./MCAL/gpio.d \
./MCAL/timer0.d \
./MCAL/timer1.d 


# Each subdirectory must supply rules for building sources it contributes
MCAL/%.o: ../MCAL/%.c MCAL/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


