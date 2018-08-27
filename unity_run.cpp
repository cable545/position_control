#include "main.h"

void setUp(void)
{
  // set stuff up here
	/* USART 2 init */
	USART_InitTypeDef usart2InitStruct;
	usart2InitStruct.USART_BaudRate = 115200;
  usart2InitStruct.USART_WordLength = USART_WordLength_8b;
  usart2InitStruct.USART_StopBits = USART_StopBits_1;
  usart2InitStruct.USART_Parity = USART_Parity_No ;
  usart2InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  usart2InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	TM_GPIO_InitAlternate(GPIOA, GPIO_Pin_2 | GPIO_Pin_3, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF_USART2);
	USART_Init(USART2, &usart2InitStruct);
	USART_Cmd(USART2, ENABLE);
	/* USART 2 init end */
}

void tearDown(void) {
    // clean stuff up here
}

void test_function_alway_pass()
{
	TEST_ASSERT_EQUAL_INT(42, 43);
}

int run()
{
	UNITY_BEGIN();
	RUN_TEST(test_function_alway_pass);
  return UNITY_END();
}
