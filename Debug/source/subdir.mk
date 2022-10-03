################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/App.c \
../source/ComunicacionCAN.c \
../source/SysTick.c \
../source/can.c \
../source/data_manager.c \
../source/fifo.c \
../source/fifo_i2c.c \
../source/gpio.c \
../source/i2cm.c \
../source/orientation.c \
../source/timer.c \
../source/uart.c 

OBJS += \
./source/App.o \
./source/ComunicacionCAN.o \
./source/SysTick.o \
./source/can.o \
./source/data_manager.o \
./source/fifo.o \
./source/fifo_i2c.o \
./source/gpio.o \
./source/i2cm.o \
./source/orientation.o \
./source/timer.o \
./source/uart.o 

C_DEPS += \
./source/App.d \
./source/ComunicacionCAN.d \
./source/SysTick.d \
./source/can.d \
./source/data_manager.d \
./source/fifo.d \
./source/fifo_i2c.d \
./source/gpio.d \
./source/i2cm.d \
./source/orientation.d \
./source/timer.d \
./source/uart.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DDEBUG -DSDK_DEBUGCONSOLE=1 -I../source -I../ -I../SDK/CMSIS -I../SDK/startup -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


