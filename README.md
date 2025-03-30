# CH32V003_SPWM_DMA-ST7735_SPI_DMA

Unit price only 10 cents CH32V003F4U6 is 32 bit RISC-V MPU by TSSOP-20pin with 48MHz internal clock speed. 
MPU can be generate single phase sine PWM pulses whith TIM1 DMA and ST7735 SPI display with SPI DMA at WCH MounRiver Studio V1.92,
CH32V003 MPU has advanced TIM1, TIM2 and independent DMA groups are almost compatible STM32F030 hardware performance.  

4 PWM pulses are usualy required for the single phase unipolar bridge driver of the DC-AC inverter.
There are 4 pulse outputs of CH1, CH1N and CH2, CH2N from CH32V003 TIM1-CH1 and TIM1-CH2 PWM2 outputs.

SPWM made by 120 step 15KHz sine table and TIM1 60Hz unipolar output port pins are CH1 =PD2, CH1N =PD0, CH2 =PA1, CH2N =PA2, BRKIN =PC2 (High for the OCP -Over Current Protection-) and DMA status LED =PC1 for the debugging. (TIM1 used 20us dead time of center aligned PWM2 output)

ST7735 128x128 TFT LCD connections are pin1 VCC =3V3, pin2 GND =GND, pin3 CS =PC4, pin4 RESET =3V3, pin5 DC =PC3, pin6 SDA =PC6, pin7 SCK =PC5, pin8 LED =3V3, if your ST7735 has pin9 SDO =not connect.

I was add read ADC1_CH7 (port PD4) and display to last line of ST7735 LCD screen for the prepair SPWM duty control. (just read and display without SPWM duty control, feedback control not implemented yet, see screen shot ST7735-demo-LCD in image folder)  

This source code made from TIM_DMA of CH32V003 EVT sample code and ST7735 library for platformIO at https://github.com/limingjie/CH32V003-ST7735-Driver, But It does not working after build for platformIO in VSCode.

My flash tool is WCH-LinkE Mini (GND, SWD, VCC connect to CH32V003F4U6 test board).
Some time does not clearly SPWM waveform continue generation, I have to toggle option SDI printf at Configure of Flash Tool in MounRiver Studio. (If you connected 3.3V device without 3.3V LDO to test board, Do not use USB-C power on test board)

I was modified st7735.c and st7735.h for the good working LCD display of ST7735 TFT LCD module as source code like standard HAL library instead mod non standard HAL libray style.
I attached some screen shot for ST7735 demo screen and TIM1 sine PWM output waveforms available in image folder.
