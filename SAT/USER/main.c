#include "sys.h"



int main(void)
{		

	System_init();
    
    while(1)
    {
    	IWDG_Feed();  //喂狗
    #if 1 	
		N720Init();
    
		N720_TCPInit();
        
        GPS_task();

        Led_task();
		
		UartDeal_task();
    #endif
#if 1		
		UartSendData_task();		

		/*处理接收到的CAN数据*/
		CANrecv_task();

		/*CAN发送数据*/
		CANsend_task();
#endif
    }
}


