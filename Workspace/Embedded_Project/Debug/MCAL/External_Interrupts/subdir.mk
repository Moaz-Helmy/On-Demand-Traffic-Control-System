################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MCAL/External_Interrupts/external_interrupts.c 

OBJS += \
./MCAL/External_Interrupts/external_interrupts.o 

C_DEPS += \
./MCAL/External_Interrupts/external_interrupts.d 


# Each subdirectory must supply rules for building sources it contributes
MCAL/External_Interrupts/%.o: ../MCAL/External_Interrupts/%.c MCAL/External_Interrupts/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


