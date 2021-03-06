//#include "stm32f10x.h"
//#include "misc.h"
//#include <stdint.h>
//#include <stdlib.h>
#include "peripheral.h"

void Periph_Init(void)
{
	TIMERS_Configuration();
	GPIO_Configuration();
}
void GPIO_Configuration(void)
{
    /* Port PB3, PA15 JTAG pins, mast be disabled */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		//JTAG disabling
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//JTAG disabling

    /* GPIOA, GPIOB clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    //LED_GREEN, LED_RED, CH_PD
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_9;  //GPIO_Pin_9 - CH_PD, GPIO_Pin_3 - LED_GREEN
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; //LED_RED
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void TIMERS_Configuration(void)
{
	//Delay function timer
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	TIM4->CR1 = TIM_CR1_CEN;
	DBGMCU->CR = DBGMCU_CR_DBG_TIM4_STOP;
}

void delay_ms(uint16_t ms)
{
	TIM4->PSC = 35999; // f timer = fclk / 36000 => 1kHz
	TIM4->ARR = ms;
	TIM4->CNT = 0;
	while((TIM4->SR & TIM_SR_UIF)==0){}
	TIM4->SR &= ~TIM_SR_UIF;
}

void delay_us(uint16_t us)
{
	TIM4->PSC = 360; // f timer = fclk / 36000 => 1kHz
	TIM4->ARR = us;
	TIM4->CNT = 0;
	while((TIM4->SR & TIM_SR_UIF)==0){}
	TIM4->SR &= ~TIM_SR_UIF;
}

void led_red_blink(uint16_t time, uint8_t quantiti)
{
	uint8_t i=0;
	while(i<=quantiti)
	{
		i++;
		LED_RED_ON;
		delay_ms(time);
		LED_RED_OFF;
		delay_ms(time);
	}
}

void led_green_blink(uint16_t time, uint8_t quantiti)
{
	uint8_t i=0;
	while(i<=quantiti)
	{
		i++;
		LED_GREEN_ON;
		delay_ms(time);
		LED_GREEN_OFF;
		delay_ms(time);
	}
}

uint8_t jumper = 0;
uint8_t jumper_state(void)
{

		/*Init ports for making possible read conditions of port */
		GPIO_InitTypeDef GPIO_InitStructure;

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

		/*Set GPIOA Port 1 to IN */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/*Set GPIOA Port 2 to OUT */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);


		GPIO_SetBits(GPIOA, GPIO_Pin_2); //Set HI level on port


	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0)
	{
		PWM_Init();
		return 0;
	}
	else if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 1)
	{
		CPPM_Init();
		return 1;
	}

		GPIO_ResetBits(GPIOA, GPIO_Pin_2);
}
