/*
********************************* (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/08/08
 * Description        : Main program body.
 ******************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * This software (modified or not) and binary are used for WCH MPU
 ******************************************************************************
 */

 #include "debug.h"

///--------------------------------------------------------------|
/// | CH32V003       | ST7735    | Description                   |
//---------------------------------------------------------------|
/// |                | 1 - VDD   | 3V3                           |
/// |                | 2 - GND   | GND                           |
/// | PC4            | 3 - CS    | SPI SS (Chip Select)          |
/// |                | 4 - RESET | 3V3                           |
/// | PC3            | 5 - RS    | DC (Data/Command)             |
/// | PC6 (SPI MOSI) | 6 - SDA   | MOSI (Master Output Slave In) |
/// | PC5 (SPI SCLK) | 7 - SCK   | SCLK (Serial Clock)           |
/// |                | 8 - LED   | 3V3                           |
///--------------------------------------------------------------|

#include "st7735.h"
#include <stdint.h>
#include <stdbool.h>

#define LCD_WIDTH    128
#define LCD_HEIGHT   128

//--------------------------------------------------------
/* TIM1 CHxCVR register Definition */
//--------------------------------------------------------
#define TIM1_CH1CVR_ADDRESS 0x40012C34  // CCR1 for CH1
#define TIM1_CH2CVR_ADDRESS 0x40012C38  // CCR2 for CH2
#define TIM1_CH3CVR_ADDRESS 0x40012C3C  // CCR3 for CH3
#define TIM1_CH4CVR_ADDRESS 0x40012C40  // CCR4 for CH4
//#define AFIO_PCFR1_ADDRESS 0x40010004

/* Private variables */
// 48MHz /(16 *100 *120 *2) = 120Hz
#define TIM1_PSC    8   // Clock = 3MHz
#define TIM1_ARR    200 // SPWM = 15KHz, Amplitude = 0~200

#define buf_size 120    // Sine Resolution
// Sine amplitude = 0~100 (reference)
u16 sine_tbl[buf_size] = {
      2,  5,  7, 10, 12, 15, 17, 20, 22, 25,
     27, 30, 32, 34, 37, 39, 41, 44, 46, 48,
     50, 52, 54, 57, 59, 61, 63, 65, 67, 68,
     70, 72, 74, 75, 77, 79, 81, 83, 84, 85,
     86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
     96, 96, 97, 97, 98, 99, 99, 99,100,100,
    100, 99, 99, 99, 98, 97, 97, 96, 96, 95,
     94, 93, 92, 91, 90, 89, 88, 87, 86, 85,
     84, 83, 81, 79, 77, 75, 74, 72, 70, 68,
     67, 65, 63, 61, 59, 57, 54, 52, 50, 48,
     46, 44, 41, 39, 37, 34, 32, 30, 27, 25,
     22, 20, 17, 15, 12, 10,  7,  5,  2,  0
};

// sine amplitude = 0~200 (feedback)
u16 sine_fdb[buf_size] = {
      2,  5,  7, 10, 12, 15, 17, 20, 22, 25,
     27, 30, 32, 34, 37, 39, 41, 44, 46, 48,
     50, 52, 54, 57, 59, 61, 63, 65, 67, 68,
     70, 72, 74, 75, 77, 79, 81, 83, 84, 85,
     86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
     96, 96, 97, 97, 98, 99, 99, 99,100,100,
    100, 99, 99, 99, 98, 97, 97, 96, 96, 95,
     94, 93, 92, 91, 90, 89, 88, 87, 86, 85,
     84, 83, 81, 79, 77, 75, 74, 72, 70, 68,
     67, 65, 63, 61, 59, 57, 54, 52, 50, 48,
     46, 44, 41, 39, 37, 34, 32, 30, 27, 25,
     22, 20, 17, 15, 12, 10,  7,  5,  2,  0
};

//--------------------------------------------------------
// Port of the Sine PWM
//--------------------------------------------------------
// PD2 =CH1 (0~180deg High Side)
// PD0 =CH1N (0~180deg LOw Side)
// PA1 =CH2 (180~360deg High Side)
// PA2 =CH2N (180~360deg LOwh Side)
// PC2 =BRKIN (High for OCP)
// PC1 =LED for DMA Transfer Status
//--------------------------------------------------------

#define DMA_LED GPIO_Pin_1      // PC1
#define TIM1_BKIN GPIO_Pin_2    // PC2

#define TIM1_CH1 GPIO_Pin_2     // PD2
#define TIM1_CH1N GPIO_Pin_0    // PD0

#define TIM1_CH2 GPIO_Pin_1     // PA1
#define TIM1_CH2N GPIO_Pin_2    // PA2

//--------------------------------------------------------
// TIM1 init
// Timer DMA routines: TIM1_CH1(PD2) + TIM1_Dead_Time_Init
// PWM output use DMA to TIM1_CH1(CH1 =PD2, CH1N =PD0)
// (complementary output and dead time)
//--------------------------------------------------------
void TIM1_PWMOut_Init(u16 arr, u16 psc, u16 ccp)
{
    GPIO_InitTypeDef        GPIO_InitStructure = {0};
    TIM_OCInitTypeDef       TIM_OCInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    TIM_BDTRInitTypeDef     TIM_BDTRInitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    // DMA LED =PC1, TIM1 BRKIN =PC2
    GPIO_InitStructure.GPIO_Pin = DMA_LED;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC, DMA_LED);   // DMA_LED =ON

    // TIM1 BRKIN =PC2
    GPIO_InitStructure.GPIO_Pin = TIM1_BKIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // CH1N =PD0, CH1 =PD2,
    GPIO_InitStructure.GPIO_Pin = TIM1_CH1N | TIM1_CH1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // TIM1 CH2 =PA1, CH2N =PA2
    GPIO_InitStructure.GPIO_Pin = TIM1_CH2 | TIM1_CH2N;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // TIM1 Clock
    TIM_DeInit(TIM1);
    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    // TIM1 CH1 Output =CH1 + CH1N =PD2, PD0
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;   
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;

    TIM_OCInitStructure.TIM_Pulse = ccp;  // start duty =50%
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;  
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);

    // TIM1 BRKIN =PC2, TIM1_Dead_Time
    TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Disable;
    TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Disable;
    TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;

    TIM_BDTRInitStructure.TIM_DeadTime = 200;   // dead time =20us
    TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable; // external break =PC2
    TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
    TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
    TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);   // Enable PWM output
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Disable);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Disable);

    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE); //  Start TIM1
}

//--------------------------------------------------------
// TIM1_DMA_Init
// Initializes the TIM DMAy Channelx configuration.
//--------------------------------------------------------
void TIM1_DMA_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA_CHx);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_MemoryBaseAddr = memadr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = bufsize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;

    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    //DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;

    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);
    DMA_Cmd(DMA_CHx, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE );
}

//--------------------------------------------------------
// interrupt for End of DMA Transfer
//--------------------------------------------------------
void DMA1_Channel5_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel5_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC5) != RESET )
    {
        if (GPIO_ReadOutputDataBit(GPIOC, DMA_LED) ==SET)
        {
            GPIO_ResetBits(GPIOC, DMA_LED);  // DMA LED off

            // 180~360deg Sine PWM by Sine Table
            TIM1_DMA_Init(DMA1_Channel5, (u32)TIM1_CH2CVR_ADDRESS, (u32)sine_fdb, buf_size);
        }

        else
        {
            GPIO_SetBits(GPIOC, DMA_LED);   // DMA LED on

            // 0~180deg Sine PWM by Sine Table
            TIM1_DMA_Init(DMA1_Channel5, (u32)TIM1_CH1CVR_ADDRESS, (u32)sine_fdb, buf_size);
        }
        DMA_ClearITPendingBit(DMA1_IT_TC5);
    }
}

//--------------------------------------------------------
// TIM2_INT_Init(USER_DELAY -1, 48 -1);
// TIM2 Clock =1us
//--------------------------------------------------------
void TIM2_INT_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIMBase_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_DeInit(TIM2);
    TIMBase_InitStruct.TIM_Period = arr;
    TIMBase_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIMBase_InitStruct.TIM_Prescaler = psc;
    TIMBase_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2, &TIMBase_InitStruct);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 5;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    //TIM_Cmd(TIM2, ENABLE);  //  Start TIM2
}

// TIM1_IRQHandler handles of TIM1 interrupt
void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM1_UP_IRQHandler(void)
{
   if( TIM_GetITStatus(TIM1, TIM_IT_Update ) ==SET)
   {
       //printf( "TIM1\r\n" );
   }
   // Clear TIM1 flag
   TIM_ClearITPendingBit(TIM1, TIM_IT_Update );
}

// TIM2_IRQHandler handles of TIM2 interrupt
// Interrupt flag is set
void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM2_IRQHandler(void)
{
   if (TIM_GetITStatus(TIM2, TIM_IT_Update) ==SET)
   {
       TIM_Cmd(TIM2, DISABLE);  // Stop TIM2

       // this can be replaced with your code of flag set
       // so that in main's that flag can be handled

       // Clear TIM2 flag
       TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
   }
}

//---------------------------------------------------------------------
// White Noise Generator State
//---------------------------------------------------------------------
#define NOISE_BITS      8
#define NOISE_MASK      ((1 << NOISE_BITS) - 1)
#define NOISE_POLY_TAP0 31
#define NOISE_POLY_TAP1 21
#define NOISE_POLY_TAP2 1
#define NOISE_POLY_TAP3 0

//---------------------------------------------------------------------
// random byte generator
//---------------------------------------------------------------------
u32 lfsr = 1;
uint8_t rand8(void)
{
    u8  bit;
    u32 new_data;

    for (bit = 0; bit < NOISE_BITS; bit++)
    {
        new_data = ((lfsr >> NOISE_POLY_TAP0) ^ (lfsr >> NOISE_POLY_TAP1) ^ (lfsr >> NOISE_POLY_TAP2) ^
                    (lfsr >> NOISE_POLY_TAP3));
        lfsr     = (lfsr << 1) | (new_data & 1);
    }
    return lfsr & NOISE_MASK;
}

//---------------------------------------------------------------------
// draw random Dot
//---------------------------------------------------------------------
uint32_t frame = 0;
uint16_t colors[] =
{
    BLACK, NAVY, DARKGREEN, DARKCYAN, MAROON,
    PURPLE, OLIVE, LIGHTGREY, DARKGREY, BLUE,
    GREEN, CYAN, RED, MAGENTA, YELLOW, WHITE,
    ORANGE, GREENYELLOW, PINK,
};

void draw_point()
{
    tft_fill_rect(0, 0, LCD_WIDTH+1, LCD_HEIGHT+1, BLACK);

    tft_set_background_color(BLACK);
    tft_set_color(PINK);
    tft_set_cursor(0, 1);
    tft_print("1. Random Dot");
    Delay_Ms(1000);

    frame = 10000;
    while (frame-- >0)
    {
        tft_draw_pixel(rand8() % LCD_WIDTH, rand8() % LCD_HEIGHT, colors[rand8() % 19]);
    }
}

//---------------------------------------------------------------------
// Scan H-Line
//---------------------------------------------------------------------
void scan_hline()
{
    tft_fill_rect(0, 0, LCD_WIDTH+1, LCD_HEIGHT+1, BLACK);

    tft_set_background_color(BLACK);
    tft_set_color(GREEN);
    tft_set_cursor(0, 1);
    tft_print("2. Horizon Line");
    Delay_Ms(1000);

    frame = 20;
    while (frame-- >0)
    {
        for (uint8_t i = 0; i < LCD_WIDTH; i++)
        {
            tft_draw_line(0, i, LCD_HEIGHT, i, colors[rand8() % 19]);
        }
    }
}

//---------------------------------------------------------------------
// Scan V-Line
//---------------------------------------------------------------------
void scan_vline()
{
    tft_fill_rect(0, 0, LCD_WIDTH+1, LCD_HEIGHT+1, BLACK);

    tft_set_background_color(BLACK);
    tft_set_color(ORANGE);
    tft_set_cursor(0, 1);
    tft_print("3. Vertical Line");
    Delay_Ms(1000);

    frame = 20;
    while (frame-- >0)
    {
        for (uint8_t i = 0; i < LCD_HEIGHT; i++)
        {
            tft_draw_line(i, 0, i, LCD_WIDTH, colors[rand8() % 19]);
        }
    }
}

//---------------------------------------------------------------------
// Random Line
//---------------------------------------------------------------------
void draw_line(void)
{
    tft_fill_rect(0, 0, LCD_WIDTH+1, LCD_HEIGHT+1, BLACK);

    tft_set_background_color(BLACK);
    tft_set_color(CYAN);
    tft_set_cursor(0, 1);
    tft_print("4. Random Line");
    Delay_Ms(1000);

    frame = 500;
    while (frame-- >0)
    {
        tft_draw_line(rand8() %128, rand8() %128, rand8() %128, rand8() %128, colors[rand8() % 19]);
    }
}

//---------------------------------------------------------------------
// Centered Rectangle
//---------------------------------------------------------------------
void scan_rect(void)
{
    tft_fill_rect(0, 0, LCD_WIDTH+1, LCD_HEIGHT+1, BLACK);

    tft_set_background_color(BLACK);
    tft_set_color(YELLOW);
    tft_set_cursor(0, 1);
    tft_print("5. Center Rectangle");
    Delay_Ms(1000);

    frame = 25;
    while (frame-- >0)
    {
        for (uint8_t i = 0; i < 40; i++)
        {
            tft_draw_rect(i, i, 128 - (i << 1), 128 - (i << 1), colors[rand8() % 19]);
        }
    }
}

//---------------------------------------------------------------------
// Random Rectangle
//---------------------------------------------------------------------
void draw_rect(void)
{
    tft_fill_rect(0, 0, LCD_WIDTH+1, LCD_HEIGHT+1, BLACK);

    tft_set_background_color(BLACK);
    tft_set_color(MAGENTA);
    tft_set_cursor(0, 1);
    tft_print("6. Random Rectangle");
    Delay_Ms(1000);

    frame = 3000;
    while (frame-- >0)
    {
        tft_draw_rect(rand8() % 128, rand8() % 128, 20, 20, colors[rand8() % 19]);
    }
}

//---------------------------------------------------------------------
// Fill Rectangle
//---------------------------------------------------------------------
void fill_rect(void)
{
    tft_fill_rect(0, 0, LCD_WIDTH+1, LCD_HEIGHT+1, BLACK);

    tft_set_background_color(BLACK);
    tft_set_color(GREENYELLOW);
    tft_set_cursor(0, 1);
    tft_print("7. Fill Rectangle");
    Delay_Ms(1000);

    frame = 2000;
    while (frame-- >0)
    {
        tft_fill_rect(rand8() %128, rand8() %128, 20, 20, colors[rand8() %19]);
    }
}

//---------------------------------------------------------------------
// Move Rectangle
//---------------------------------------------------------------------
void move_text(void)
{
    tft_fill_rect(0, 0, LCD_WIDTH+1, LCD_HEIGHT+1, BLACK);

    tft_set_background_color(BLACK);
    tft_set_color(WHITE);
    tft_set_cursor(0, 1);
    tft_print("8. Move Rectangle");
    Delay_Ms(1000);

    frame =200;
    uint8_t x =0, y =0, step_x =1, step_y =1;
    while (frame-- >0)
    {
        uint16_t bg = colors[rand8() % 19];
        tft_fill_rect(x, y, 80, 16, bg);
        tft_set_color(colors[rand8() %19]);

        tft_set_background_color(bg);
        tft_set_cursor(x +5, y +5);
        tft_print("Hello, World");
        Delay_Ms(1);

        x += step_x;
        if (x >= 48)
        {
            step_x = -step_x;
        }
        y += step_y;
        if (y >= 124)
        {
            step_y = -step_y;
        }
    }
}

//---------------------------------------------------------------------
// ST7735 demo display
//---------------------------------------------------------------------
void demo_LCD(void)
{
    draw_point();

    scan_hline();
    scan_vline();
    draw_line();

    scan_rect();
    draw_rect();

    fill_rect();
    move_text();
}

//---------------------------------------------------------------------
// Init AD7 =PD4
//---------------------------------------------------------------------
#include "ch32v00x_adc.h"

void init_ADC(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    ADC_InitTypeDef  ADC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    // AD7 =PD4
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    //ADC_BufferCmd(ADC1, DISABLE);    //disable buffer
    ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_30Cycles);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    Delay_Ms(50);
    //ADC_SoftwareStartConvCmd(ADC1, DISABLE);
}

#include <stdio.h>
u16 bin_val;
float fdb_val;
char disp_str[5];

u16 get_ADC(void)
{
    u16 val;    // result of ADC_Channel_7

    ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_30Cycles);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    val = ADC_GetConversionValue(ADC1);
    return val;
}

void read_ADC(void)
{
    bin_val =get_ADC();
    fdb_val =(float)(bin_val);

    tft_set_color(WHITE);
    tft_set_cursor(0, 88);
    tft_print("ADC1-CH7: ");

    // erase value area (not use right align)
    //tft_fill_rect(54, 88, 128, 16, BLACK);

    // binary convert to decimal as right align
    sprintf(disp_str, "%4d", bin_val);
    tft_set_color(YELLOW);
    tft_set_cursor(54, 88);
    tft_print(disp_str);

    Delay_Ms(50);
}

//---------------------------------------------------------------------
// Main program.
//---------------------------------------------------------------------
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();    // HSI 48MHz
    Delay_Init();

    // all clear AF of GPIO
    GPIO_AFIODeInit();

#if (SDI_PRINT == SDI_PR_OPEN)
    SDI_Printf_Enable();
#else
    USART_Printf_Init(115200);
#endif
    //printf("SystemClk:%d\r\n", SystemCoreClock);
    //printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

    // (psc, arr*2 , ccr) for 15.0KHz PWM / 120 Step =120Hz
    TIM1_PWMOut_Init(TIM1_ARR, TIM1_PSC -1, 0);

    // Sine PWM to CH1, CH1N,
    TIM1_DMA_Init(DMA1_Channel5, (u32)TIM1_CH1CVR_ADDRESS, (u32)sine_fdb, buf_size);
    //TIM1_DMA_Init(DMA1_Channel5, (u32)TIM1_CH2CVR_ADDRESS, (u32)sine_fdb buf_size);

    TIM_DMACmd(TIM1, TIM_DMA_Update, ENABLE);   // Start DMA
    TIM_Cmd(TIM1, ENABLE);  //  Start TIM1

    // Start TIM2 for user timer
    //TIM2_INT_Init(USER_DELAY -1, 48 -1);   // TIM2 Clock =1us
    //TIM_Cmd(TIM2, ENABLE);
    // check TIM2_IRQHandler(void)

    // Init ST7735 TFT LCD
    tft_init();
    Delay_Ms(100);

    tft_fill_rect(0, 0, LCD_WIDTH+1, LCD_HEIGHT+1, BLACK);
    tft_set_color(WHITE);
    tft_set_background_color(BLACK);

    demo_LCD();

    tft_fill_rect(0, 0, LCD_WIDTH+1, LCD_HEIGHT+1, BLACK);
    tft_set_background_color(BLACK);
    tft_set_color(GREEN);
    tft_set_cursor(0, 1);
    tft_print("CH32V003 ST7735 Demo");

    tft_set_color(WHITE);
    tft_set_cursor(0, 16);
    tft_print("1. Random dot");
    tft_set_cursor(0, 24);
    tft_print("2. Horizon line");
    tft_set_cursor(0, 32);
    tft_print("3. Vertical line");
    tft_set_cursor(0, 40);
    tft_print("4. Random line");
    tft_set_cursor(0, 48);
    tft_print("5. Center Rectangle");
    tft_set_cursor(0, 56);
    tft_print("6. Random Rectangle");
    tft_set_cursor(0, 64);
    tft_print("7. Fill rectangle");
    tft_set_cursor(0, 72);
    tft_print("8. Move Rectangle");

    init_ADC();
    Delay_Ms(100);

    // Start of User Code.
    while(1)
    {
        read_ADC();

    }   // End ofUser Code.
}   // End of main()
//---------------------------------------------------------------------

