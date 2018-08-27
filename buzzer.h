#ifndef _BUZZER_H
#define _BUZZER_H

#define BUZZER_GPIO_PORT						GPIOA
#define BUZZER_GPIO_PIN							GPIO_Pin_0
#define BUZZER_DIFF_GPIO_PIN				GPIO_Pin_1

#define BUZZER_GPIO_PIN_SET					GPIO_BSRR_BS_0
#define BUZZER_GPIO_PIN_RESET				GPIO_BSRR_BR_0

#define BUZZER_DIFF_GPIO_PIN_SET		GPIO_BSRR_BS_1
#define BUZZER_DIFF_GPIO_PIN_RESET	GPIO_BSRR_BR_1

class Buzzer
{
public:
	static void initBuzzer(bool differentialDrive);
	static uint32_t buzzerBeepXTimes(uint32_t count);

private:
	static bool isDifferentialDrive;
	static bool isInitialized;	

	static void beepBuzzer();
};

#endif
