################################################################################
# MRS Version: 2.1.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/ch32v00x_it.c \
../User/delay.c \
../User/main.c \
../User/st7735.c \
../User/system_ch32v00x.c \
../User/uart.c 

C_DEPS += \
./User/ch32v00x_it.d \
./User/delay.d \
./User/main.d \
./User/st7735.d \
./User/system_ch32v00x.d \
./User/uart.d 

OBJS += \
./User/ch32v00x_it.o \
./User/delay.o \
./User/main.o \
./User/st7735.o \
./User/system_ch32v00x.o \
./User/uart.o 



# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"d:/CH32V003/ch32v003-timer/TIM-DMA/Debug" -I"d:/CH32V003/ch32v003-timer/TIM-DMA/Core" -I"d:/CH32V003/ch32v003-timer/TIM-DMA/User" -I"d:/CH32V003/ch32v003-timer/TIM-DMA/Peripheral/inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@
