# CH32V003_SPWM_DMA-ST7735_SPI_DMA

Unit price 10 cents CH32V003F4U6 32 bit RISC-V TSSOP-20pin MPU generate 1 phase unipolar sine PWM pulse used TIM1 DMA and ST7735 SPI display used SPI DMA for WCH MounRiver Studio V1.92

Normally need 4 PWM waveform for the 1 phase unipolar bridge driver of the DC-AC inverter.
There are 4 pulse output of CH1, CH1N and CH2, CH2N from CH32V003 TIM1 CH1 and CH2 PWM outputs.

SPWM made by 120 step 15KHz sine table and TIM1 60Hz unipolar output ports are CH1 =PD2, CH1N =PD0, CH2 =PA1, CH2N =PA2, BRKIN =PC2 High for the OCP (Over Current Protection Control) and DMA status LED =PC1 for the debugging. (PWM2 used 20us dead time)

ST7735 128x128 TFT LCD connections are pin1 VCC =3V3, pin2 GND =GND, pin3 CS =PC4, pin4 RESET =3V3, pin5 DC =PC3, pin6 SDA =PC6, pin7 SCK =PC5, pin8 LED =3V3, if your ST7735 has pin9 SDO =not connect.

This source code made from TIM-DMA of CH32V003 EVK sample code and ST7735 library used platformIO code of https://github.com/limingjie/CH32V003-ST7735-Driver, But It does not working after build at platformIO in VSCode.

My flash tool is WCH-LinkE Mini (GND, SWD, VCC connect to CH32V003F4U6 test board).
Some time does not clearly spwm waveform continue generation, I have to toggle option SDI printf at Option of Flash Tool in MounRiver Studio.

I was modified st7735.c and st7735.h for the good working LCD display of ST7735 TFT LCD module.
I attached some screen shot for ST7735 demo screen and TIM1 Sine PWM output wave forms available in Image folder.
