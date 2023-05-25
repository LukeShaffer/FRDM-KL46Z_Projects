################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/mtb.c \
../source/semihost_hardfault.c \
../source/slcd.c \
../source/touchsensor.c \
../source/ttt.c 

OBJS += \
./source/mtb.o \
./source/semihost_hardfault.o \
./source/slcd.o \
./source/touchsensor.o \
./source/ttt.o 

C_DEPS += \
./source/mtb.d \
./source/semihost_hardfault.d \
./source/slcd.d \
./source/touchsensor.d \
./source/ttt.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MKL46Z256VLL4 -DCPU_MKL46Z256VLL4_cm0plus -DSDK_OS_BAREMETAL -DFSL_RTOS_BM -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I../board -I../source -I../ -I../drivers -I../CMSIS -I../utilities -I../startup -I"C:\Users\adity\Documents\MCUXpressoIDE_11.0.0_2516\workspace\slcd\CMSIS" -I"C:\Users\adity\Documents\MCUXpressoIDE_11.0.0_2516\workspace\slcd\source" -I"C:\Users\adity\Documents\MCUXpressoIDE_11.0.0_2516\workspace\slcd\utilities" -I"C:\Users\adity\Documents\MCUXpressoIDE_11.0.0_2516\workspace\slcd\drivers" -I"C:\Users\adity\Documents\MCUXpressoIDE_11.0.0_2516\workspace\slcd\board" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

source/ttt.o: ../source/ttt.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MKL46Z256VLL4 -DCPU_MKL46Z256VLL4_cm0plus -DSDK_OS_BAREMETAL -DFSL_RTOS_BM -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I../board -I../source -I../ -I../drivers -I../CMSIS -I../utilities -I../startup -I"C:\Users\adity\Documents\MCUXpressoIDE_11.0.0_2516\workspace\slcd\CMSIS" -I"C:\Users\adity\Documents\MCUXpressoIDE_11.0.0_2516\workspace\slcd\source" -I"C:\Users\adity\Documents\MCUXpressoIDE_11.0.0_2516\workspace\slcd\utilities" -I"C:\Users\adity\Documents\MCUXpressoIDE_11.0.0_2516\workspace\slcd\drivers" -I"C:\Users\adity\Documents\MCUXpressoIDE_11.0.0_2516\workspace\slcd\board" -I"C:\nxp\MCUXpressoIDE_11.0.0_2516\x86_64-w64-mingw32\include" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"source/ttt.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


