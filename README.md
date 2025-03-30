# CH32V003_SPWM_DMA-ST7735_SPI_DMA

Unit price only 10 cents CH32V003F4P6 is 32 bit RISC-V MPU by TSSOP-20pin with 48MHz internal clock speed. 
MPU can be generate single phase sine PWM pulses whith TIM1 DMA and ST7735 SPI display with SPI DMA at WCH MounRiver Studio V1.92,
CH32V003 MPU has advanced TIM1, TIM2 and independent DMA groups are almost compatible STM32F030 hardware performance.  

![ch32v003f4u6-tssop20-dev-kit-pcb](https://github.com/user-attachments/assets/070066ac-698e-4078-a72c-f93470adbbd0)

CH32V003F4P6 GPIO and AF Map. (PC2 can be use external break-in signal to stop all PWM output when detect over current load.)

4 PWM pulses are usualy required for the single phase unipolar bridge driver of the DC-AC inverter.
There are 4 pulse outputs of CH1, CH1N and CH2, CH2N from CH32V003 TIM1-CH1 and TIM1-CH2 PWM2 outputs.

SPWM made by 120 step 15KHz sine table and TIM1 60Hz PWM2 output port pins are CH1 =PD2, CH1N =PD0, CH2 =PA1, CH2N =PA2, BRKIN =PC2 
(High for the OCP -Over Current Protection-) and DMA status LED =PC1 for the debugging. (TIM1 used 20us dead time of center aligned PWM2 output.)

![CH32V003-TIM1-SPWM-Output](https://github.com/user-attachments/assets/847d9bcb-5ec9-4392-903b-4ec198b7937d)

SPWM CH1 (0 - 180deg) and CH2 (180 - 360deg) Waveform After R-C (4K7 + 47nF) Intergrated.

![SPWM-CH1-PD2-Wave](https://github.com/user-attachments/assets/dda8d39b-4f17-4b54-8369-0064a6850676)

TIM1 High Side Driver SPWM CH1 (PD2) Waveform.

![SPWM-CH1N-PD0-Wave](https://github.com/user-attachments/assets/d351f686-6bd4-417a-93d7-0c9b21f16232)

TIM1 Low Side Driver SPWM CH1N (PD0) Waveform.

![intergrated-pd2+pa1-wave](https://github.com/user-attachments/assets/920dce32-cd3b-4efb-bc16-92ac689e972a)

Intergrated TIM1 High Side SPWM (PD2) and TIM1 CH2 (PA1) High Side waveform.

![intergrated-pd2+pa2-wave](https://github.com/user-attachments/assets/fc19f20f-d2bb-4c9b-a646-12534a298f61)

Intergrated TIM1 High Side SPWM (PD2) and TIM1 CH2N (PA2) Low Side waveform.

ST7735 128x128 TFT LCD connections are pin1 VCC =3V3, pin2 GND =GND, pin3 CS =PC4, pin4 RESET =3V3, pin5 DC =PC3, pin6 SDA =PC6, pin7 SCK =PC5, pin8 LED =3V3, if your ST7735 has pin9 SDO =not connect.

![ch32v003-st7735-graphic-demo](https://github.com/user-attachments/assets/2b6b54f7-cbb7-4d21-8d7b-d96b9b8844a6)

CH32V003+ST7735 Demo Graphic of Centered Rectagle.

I was add read ADC1_CH7 (port PD4) and display to last line of ST7735 LCD screen for the prepair SPWM duty control. 
(just read and display without SPWM duty control, feedback control not implemented yet).

![ST7735-Demo-Screen](https://github.com/user-attachments/assets/59abe810-4266-42f3-a2cc-388b9d58cfca)

CH32V003+ST7735 Main Screen Shot added ADC1_CH7 (PD4) read value (0 - 1023) to Display (Yellow) at Last Line 

This source code made from TIM_DMA of CH32V003 EVT sample code and ST7735 library for platformIO at https://github.com/limingjie/CH32V003-ST7735-Driver,
But It does not working with SPWM 4 pulses output after build for platformIO in VSCode.

![ch32v003f4u6-tssop20-dev-kit-sch](https://github.com/user-attachments/assets/0c5feaf8-160c-4663-a4fd-11452de821da)

CH32V003F4P6 Development Board Schematic Diagram.

My flash tool is WCH-LinkE-Mini (GND, SWD, VCC) connect to CH32V003F4P6 test board (GND, PD1, VCC)
Some time does not clearly SPWM waveform continue generation, I have to toggle option SDI printf at Configure of Flash Tool in MounRiver Studio. 
(If you connected 3.3V device without 3.3V LDO to test board, Do not use USB-C power on test board)

![ch32v003f4u6-LinkE-Mini-pcb](https://github.com/user-attachments/assets/9f31f482-edbf-40e4-b789-150568d85ed8)

WCH-LinkE-Mini PCB Product without Encloser. (WCH-LinkE or WCH-LinkRV are Default Flash Tool at MounRiver Studio.)

I was modified st7735.c and st7735.h for the good working LCD display of ST7735 TFT LCD module as source code like standard HAL library instead mod non standard HAL libray style.
I attached some screen shot for ST7735 demo screen and TIM1 sine PWM output waveforms available in image folder.
