#include "main.h"

bool Buzzer::differentialMode = false;
bool Buzzer::initialized = false;

void Buzzer::beep()
{
	if(differentialMode)
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

void Buzzer::init(const bool activateDifferentialMode)
{
	differentialMode = activateDifferentialMode;
	
	if(differentialMode)
	{
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
	
	initialized = true;
}

uint32_t Buzzer::beepXTimes(uint32_t times)
{
	if(!initialized) return BUZZER_NOT_INITIALIZED;
	
	for(uint32_t i = times; i > 0; i--)
	{
		beep();
		delay(500);
	}
	
	return EXIT_SUCCESS;
}
