#ifndef __DEBUG_H
#define __DEBUG_H

enum
{
	STDOUT_HANDLE,
	EXT_DEBUG_HANDLE
};

class Debug
{
private:
	static uint8_t* convert(uint32_t num, int32_t base);
	static void reverse(uint8_t* str, uint32_t len);
	static uint32_t intToStr(int32_t x, uint8_t string[], uint32_t d);
	static void out(const char* string, va_list arg);
	
public:	
	static void print(const char* string, ...);
	static void println(const char* string, ...);
	static void ftoa(float n, uint8_t string[], uint8_t afterpoint);
	static void routeSysclockToPC9();
};

#endif
