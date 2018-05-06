#include <rt_sys.h>
#include "usart.h"

extern "C"
{
	#define DEFAULT_HANDLE 0x100;

	/*
	 * UART prototype functions
	 */
	//char UART_read(void);
	//void UART_write(char);
	
	/*
	 * These names are special strings which will be recognized by 
	 * _sys_open and will cause it to return the standard I/O handles, instead
	 * of opening a real file.
	*/
	const char __stdin_name[] ="STDIN";
	const char __stdout_name[]="STDOUT";
	const char __stderr_name[]="STDERR";
	
	/*
	 * Open a file. May return -1 if the file failed to open. We do not require
	 * this function to do anything. Simply return a dummy handle.
	*/
	FILEHANDLE _sys_open(const char * name, int openmode)
	{
    return DEFAULT_HANDLE;  
	}
	
	/*
	 * Close a file. Should return 0 on success or a negative value on error.
	 * Not required in this implementation. Always return success.
	*/
	int _sys_close(FILEHANDLE fh)
	{
    return 0; //return success
	}

	/*
	 * Write to a file. Returns 0 on success, negative on error, and the number
	 * of characters _not_ written on partial success. This implementation sends
	 * a buffer of size 'len' to the UART.
	*/
	int _sys_write(FILEHANDLE fh, const unsigned char* buf, unsigned len, int mode)
	{
		int i;
    for(i=0;i<len;i++) 
    {
			USART_SendData(USART2, buf[i]);
			
			while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
			//UART_write(buf[i]);
    
      // Fix for HyperTerminal    
      //if(buf[i]=='\n') UART_write('\r');
    }
    
    return 0;   
	}
}