/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_sys_main.c
Creator: zj.zong				Date: 2009-05-27
Description: system main loop(wait for interrupt setting ui_sys_event, and process event)
Others:
Version: V0.1
History:
	V0.1	2009-05-27		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_SYS_MAIN_DEF

#include "sh86278_api_define.h"
#include "sh86278_sfr_reg.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include ".\UI\UI_System\ui_sys_header.h"
#include ".\UI\UI_System\ui_sys_var_ext.h"

extern void	Ap_Enable_IrqBit(void); 
extern void Ap_ProcessEvt_Usb(void);
extern void Ui_ProcessEvt_Key(void);
extern void Ui_ProcessEvt_Timer(void);
extern void Ap_ProcessEvt_Bat(void);
extern void Ui_ProcessEvt_Display(void);
extern void Ap_ProcessEvt_Msi(void); 
extern void Ap_SysInit(void);

/*******************************************************************************
Function: Ui_SysMain()
Description: system main function
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_SysMain(void)
{
	mReset_PC_Stack;					//reset PC STACK					
	mReset_WatchDog;					//clr watchdog
										 
	Ap_Enable_IrqBit();   					//enable interrupt(timer irq, ADC irq,USB Detect irq)
	
	Ap_SysInit();					 	//init system parameter
	
	Ui_ProcessEvt_Display();			//display sys main window(first display)
	ui_sys_event &= ~EVT_DISPLAY;
	ui_sys_event &= ~EVT_KEY;			//clr key event, it is invalid before first display

	while(1)
	{
		mReset_WatchDog;				//clr watchdog

		if((ui_sys_event & EVT_USB) != 0)
		{
			ui_sys_event &= ~EVT_USB;
			Ap_ProcessEvt_Usb();			//plug usb , and enter usb mode
		}

		else if((ui_sys_event & EVT_MSI) != 0)
		{
			ui_sys_event &= ~EVT_MSI;
			Ap_ProcessEvt_Msi();				//insert sd or draw sd
		}

		else if((ui_sys_event & EVT_KEY) != 0)
		{
			ui_sys_event &= ~EVT_KEY;
			Ui_ProcessEvt_Key();			//process key event
		}

		else if((ui_sys_event & EVT_TIMER) != 0)
		{
			ui_sys_event &= ~EVT_TIMER;
			Ui_ProcessEvt_Timer();			//20ms timer
		}

		else if((ui_sys_event & EVT_BATTERY) != 0)
		{
			ui_sys_event &= ~EVT_BATTERY;
			Ap_ProcessEvt_Bat();			//process adc data (battery in)
		}
	  
	  	else if((ui_sys_event & EVT_DISPLAY) != 0)
		{
			ui_sys_event &= ~EVT_DISPLAY;
			Ui_ProcessEvt_Display();		//dislay function
		}
	}
}


#endif