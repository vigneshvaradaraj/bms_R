
#include <stdint.h>
#include "events.h"
void timerInit_10ms(void)
{
    /*need to set the timer6 for 0.5ms sec interrupt*/
	//step 1: enable clock for timer6
	RCC->APB2ENR |= (1 << 17);//enable clock for TIMER16
	
	//for apb1 timer clock just dived the 16Mz by 16
	//RCC->CFGR |= (7 << 10);
	
	//cnt clock is 1u sec it needs to be devided by 16 to make the timer clock as 1Mhz 
    /*1m sec is the source of the TIM6*/
	TIM16->PSC = 16000 - 1;/*Assume 16 Mhz internal oscillator is used as source*/
	
	//store 500 to auto reload preload register to generate event for every 25m sec
	TIM16->ARR = 25 - 1;
	
//	TIM6->CR1 = 0;
//	TIM6->CR1 |= (1 << 7);
//	TIM6->CR1 |= (1 << 0);
//	TIM6->CR1 |= (1 << 2);
	
	NVIC_EnableIRQ(TIM6_DAC_IRQn);

	TIM16->CR1 |= (1 << 0);
	//enable interrupt 
	TIM16->DIER |= (1 <<0);
}

void timerCallBack_10ms(void)
{
    /*It can handle 20 events so f = (500ms/20)*/
    /*This timer freq = 40hz = 25ms * 20 = 500ms*/
    static uint32_t caseCount = 0;
    switch(caseCount)
    {
        case 0:
        {
            break;
        }
        case 1:
        {
            break;
        }
        case 2:
        {
            break;
        }
        case 3:
        {
            break;
        }
        case 4:
        {
            break;
        }
        case 5:
        {
            break;
        }
        case 6:
        {
            break;
        }
        case 7:
        {
            break;
        }
        case 8:
        {
            break;
        }
        case 9:
        {
            break;
        }
        case 10:
        {
            break;
        }
        case 11:
        {
            break;
        }
        case 12:
        {
            break;
        }
        case 13:
        {
            break;
        }
        case 14:
        {
            break;
        }
        case 15:
        {
            break;
        }
        case 16:
        {
            break;
        }
        case 17:
        {
            break;
        }
        case 18:
        {
            break;
        }
        case 19:
        {
            caseCount = 0;
            break;
        }
        default:
        {

            break;
        }
    }

    caseCount++;
}