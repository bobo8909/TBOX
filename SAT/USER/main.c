#include "sys.h"



int main(void)
{		

	System_init();

    while(1)
    {
    	IWDG_Feed();  //喂狗
		
		N720Init();
		
		Led_task();
		
		UartSendData_task();		

		UartDeal_task();
		/*处理接收到的CAN数据*/
		CANrecv_task();

		/*CAN发送数据*/
		CANsend_task();
    }
}


