#include "main.h"

void Debug::print(const char* string, ...)
{
	va_list arg;
	va_start(arg, string);
	out(string, arg);
	va_end(arg);
}

void Debug::println(const char* string, ...)
{
	va_list arg;
	va_start(arg, string);
	out(string, arg);
	USART_SendData(USART2, '\n');
	va_end(arg);
}

void Debug::out(const char* string, va_list arg)
{
	const char* traverse;
	int32_t i;
	float f;
	uint8_t* s;
	uint8_t buffer[20];
	
	for(traverse = string; *traverse != '\0'; traverse++)
	{
		while(*traverse != '%') 
		{ 
			if(*traverse == '\0') return;
			
			USART_SendData(USART2, *traverse);
			
			traverse++; 
		}

		traverse++;
		
		switch(*traverse)
		{
			case 'c': i = va_arg(arg, int);		//Fetch char argument
				USART_SendData(USART2, i);
				
				break;
			case 'i':
			case 'd': i = va_arg(arg, int); 		//Fetch Decimal/Integer argument
				if(i < 0)
				{ 
					i = -i;
					
					USART_SendData(USART2, '-');
				}
				
				USART_SendString(USART2, convert(i, 10));
				
				break;
			case 'f': f = (float)va_arg(arg, double);
				if(f < 0.0f)
				{
					f = -f;
					
					USART_SendData(USART2, '-');
				}
				
				ftoa(f, buffer, 4);
				
				USART_SendString(USART2, buffer);
				
				break;
			case 'o': i = va_arg(arg, unsigned int); //Fetch Octal representation
				USART_SendString(USART2, convert(i, 8));
				
				break;
			case 's': s = va_arg(arg, uint8_t *); 		//Fetch string
				USART_SendString(USART2, s);
				
				break;
			case 'x': i = va_arg(arg, unsigned int); //Fetch Hexadecimal representation
				USART_SendString(USART2, convert(i, 16));
				
				break;
		}
	}
}

uint8_t* Debug::convert(uint32_t num, int32_t base)
{ 
	uint8_t Representation[]= "0123456789ABCDEF";
	static uint8_t buffer[50] = {0};
	uint8_t *ptr;
	
	ptr = &buffer[49];
	*ptr = '\0';
	
	do
	{
		*--ptr = Representation[num % base];
		num /= base;
	}while(num != 0);
	
	return ptr;
}

void Debug::reverse(uint8_t* str, uint32_t len)
{
	uint32_t i = 0, j = len - 1, temp;
	
	while(i < j)
	{
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++; j--;
	}
}

uint32_t Debug::intToStr(int32_t x, uint8_t string[], uint32_t d)
{
	uint32_t i = 0;
	
	while(x)
	{
		string[i++] = (x % 10) + '0';
		x = x / 10;
	}
	
	// if number of digits requied more than digits needed,
	// then add '0' at the beginning
	while(i < d) string[i++] = '0';
	
	reverse(string, i);
	string[i] = '\0';
	
	return i;
}

void Debug::ftoa(float n, uint8_t string[], uint8_t afterpoint)
{
	int i = 0;
	
	int ipart = (int)n;
	float fpart = n - (float)ipart;
	
	if(ipart == 0)
		string[i++] = '0';
	else
		i = intToStr(ipart, string, 0);
	
	if(afterpoint != 0)
	{
		string[i] = '.';

		fpart = fpart * pow(10.0, afterpoint);
		intToStr((int)fpart, string + i + 1, afterpoint);
	}
}

void Debug::routeSysclockToPC9()
{
	TM_GPIO_Init(GPIOC, GPIO_PIN_9, TM_GPIO_Mode_AF, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High);
	RCC_MCO2Config(RCC_MCO2Source_SYSCLK, RCC_MCO2Div_1);
}






