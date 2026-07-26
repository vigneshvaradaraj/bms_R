
#include <stdint.h>
#include "events.h"

void timerInit_10ms(void)
{
    /*need to set the timer6 for 0.5ms sec interrupt*/
	//step 1: enable clock for timer6
	RCC->APB1ENR |= (1 << 1);//enable clock for TIMER3
	
	//for apb1 timer clock just dived the 16Mz by 16
	//RCC->CFGR |= (7 << 10);
	
	//cnt clock is 1u sec it needs to be devided by 16 to make the timer clock as 1Mhz 
    /*1u sec is the source of the TIM6*/
	TIM3->PSC = 16 - 1;/*Assume 16 Mhz internal oscillator is used as source*/
	
	//store 500 to auto reload preload register to generate event for every 500u sec
	TIM3->ARR = 500 - 1;
	
//	TIM6->CR1 = 0;
//	TIM6->CR1 |= (1 << 7);
//	TIM6->CR1 |= (1 << 0);
//	TIM6->CR1 |= (1 << 2);
	
	NVIC_EnableIRQ(TIM6_DAC_IRQn);

	TIM3->CR1 |= (1 << 0);
	//enable interrupt 
	TIM3->DIER |= (1 <<0);
}

void timerCallBack_10ms(void)
{
    /*It can handle 20 events so f = (10ms/20)*/
    /*This timer freq = 2Khz = 0.5ms * 20 = 10ms*/
    static int8_t eventHandlerArrayIndex = 0;
   
    if((NULL != eventHandlerArray[eventHandlerArrayIndex]) && (eventHandlerArrayIndex < 20))
    {
        eventHandlerArray[eventHandlerArrayIndex]();
    }
    if(eventHandlerArrayIndex++ >= 20)
    {
        eventHandlerArrayIndex = 0;
    }
}

void TIM3_DAC_IRQHandler(void)
{
    timerCallBack_10ms();
}