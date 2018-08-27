#include "main.h"

bool Buzzer::isDifferentialDrive = false;
bool Buzzer::isInitialized = false;

void Buzzer::beepBuzzer()
{
	if(isDifferentialDrive)
	{
		for(int i = 4000; i > 0; i--)
		{
			BUZZER_GPIO_PORT->BSRR = BUZZER_GPIO_PIN_SET;
			BUZZER_GPIO_PORT->BSRR = BUZZER_DIFF_GPIO_PIN_RESET;
			delayMicroseconds(125);
			BUZZER_GPIO_PORT->BSRR = BUZZER_GPIO_PIN_RESET;
			BUZZER_GPIO_PORT->BSRR = BUZZER_DIFF_GPIO_PIN_SET;
			delayMicroseconds(125);
		}
	}
	else
	{
		for(int i = 4000; i > 0; i--)
		{
			BUZZER_GPIO_PORT->BSRR = BUZZER_GPIO_PIN_SET;
			delayMicroseconds(125);
			BUZZER_GPIO_PORT->BSRR = BUZZER_GPIO_PIN_RESET;
			delayMicroseconds(125);
		}
	}
}

void Buzzer::initBuzzer(bool differentialDrive)
{
	isDifferentialDrive = differentialDrive;
	
	if(isDifferentialDrive)
	{
		/*
		 * initialzed the gpio port 5 and 6 from gpio modul a as an output
		 * and enables the clock from gpioa
		 */
		TM_GPIO_Init(
			BUZZER_GPIO_PORT,
			BUZZER_GPIO_PIN | BUZZER_DIFF_GPIO_PIN,
			TM_GPIO_Mode_OUT,
			TM_GPIO_OType_PP,
			TM_GPIO_PuPd_NOPULL,
			TM_GPIO_Speed_High
		);
	}
	else
	{
		TM_GPIO_Init(
			BUZZER_GPIO_PORT,
			BUZZER_GPIO_PIN,
			TM_GPIO_Mode_OUT,
			TM_GPIO_OType_PP,
			TM_GPIO_PuPd_UP,
			TM_GPIO_Speed_High
		);
	}
	
	isInitialized = true;
}

uint32_t Buzzer::buzzerBeepXTimes(uint32_t count)
{
	if(!isInitialized) return BUZZER_NOT_INITIALIZED;
	
	for(int i = count; i > 0; i--)
	{
		beepBuzzer();
		delay(500);
	}
	
	return EXIT_SUCCESS;
}
