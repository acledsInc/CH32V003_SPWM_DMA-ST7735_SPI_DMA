# CH32V003_SPWM_DMA-ST7735_SPI_DMA

CH32V003F4U6 32 bit RISC-V TSSOP-20pin MPU generate 1 phase unipolar sine PWM pulse used TIM1 DMA and ST7735 SPI display used SPI DMA for WCH MounRiver Studio V1.92

Normally need 4 PWM waveform for the 1 phase unipolar bridge driver of the DC-AC inverter.
There are 4 pulse output of CH1, CH1N and CH2, CH2N from CH32V003 TIM1 CH1 and CH2 PWM outputs.

SPWM made by 120 step sine table and TIM1 output ports are CH1 =PD2, CH1N =PD0, CH2 =PA1, CH2N =PA2, BRKIN =PC2 High and DMA status LED =PC1

ST7735 128x128 TFT LCD connections are pin1 VCC =3V3, pin2 GND =GND, pin3 CS =PC4, pin4 RESET =3V3, pin5 DC =PC3, pin6 SDI =PC6, pin7 SCK =PC5, pin7 SDA =PC6, pin8 LED =3V3, if your ST7735 has pin9 SDO =not connect.

This source code made from TIM_DMA at CH32V003 EVK sample code and ST7725 library used platformIO code at https://github.com/limingjie/CH32V003-ST7735-Driver, But It was much bug and does not working.

I was successly modify st7735.c and st7735.h for good working LCD display of ST7735 TFT LCD module.
I attached some screen shot for ST7735 demo screen and TIM1 SPM output wave forms in Image folder.
