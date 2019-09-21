#ifndef _BUZZER_H
#define _BUZZER_H

#define BUZZER_GPIO_PORT						GPIOA
#define BUZZER_GPIO_PIN							GPIO_Pin_0
#define BUZZER_DIFF_GPIO_PIN				GPIO_Pin_1

#define BUZZER_GPIO_PIN_SET					GPIO_BSRR_BS_0
#define BUZZER_GPIO_PIN_RESET				GPIO_BSRR_BR_0

#define BUZZER_DIFF_GPIO_PIN_SET		GPIO_BSRR_BS_1
#define BUZZER_DIFF_GPIO_PIN_RESET	GPIO_BSRR_BR_1

#define BUZZER_NORMAL_MODE					1
#define BUZZER_DIFFERENTIAL_MODE		2

class Buzzer
{
public:
	static void init(const bool activateDifferentialMode);
	static uint32_t beepXTimes(uint32_t times);

private:
	static bool differentialMode;
	static bool initialized;	

	static void beep();
};

#endif
