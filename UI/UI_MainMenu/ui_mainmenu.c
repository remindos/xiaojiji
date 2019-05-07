/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_mainmenu.c
Creator: zj.zong					Date: 2009-04-22
Description: select mode(7'mode:music, .... telbook)
Others:
Version: V0.1
History:
	V0.1	2009-04-22		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_MAINMENU_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include ".\UI\UI_MainMenu\ui_mainmenu_header.h"
#include ".\UI\UI_MainMenu\ui_mainmenu_var_ext.h"


extern void Ui_Init_MainMenu(void);
extern void Ui_ProcessEvt_Key(void);
extern void Ui_ProcessEvt_Timer(void);
extern void Ui_ProcessEvt_Display(void);
extern void Ap_ProcessEvt_Usb(void);
extern void Ap_ProcessEvt_Bat(void);
extern void Ap_Enable_IrqBit(void);

/*******************************************************************************
Function: Ui_MainMenu_Main()
Description:
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_MainMenu_Main(void)
{
	mReset_PC_Stack;							//initial PC Stack
	mReset_WatchDog;
	
	Ap_Enable_IrqBit();   						//enable interrupt(timer irq, ADC irq,USB Detect irq)

	Ui_Init_MainMenu();

	while(1)
	{
		mReset_WatchDog;						//Clear watchdog

		if((ui_sys_event & EVT_USB) != 0)
		{
			ui_sys_event &= ~EVT_USB;
			Ap_ProcessEvt_Usb();				//plug usb, and enter usb mode
		}

		else if((ui_sys_event & EVT_KEY) != 0)
		{
			ui_sys_event &= ~EVT_KEY;
			Ui_ProcessEvt_Key();				//process key event
		}

		else if((ui_sys_event & EVT_TIMER) != 0)
		{
			ui_sys_event &= ~EVT_TIMER;
			Ui_ProcessEvt_Timer();				//20ms timer
		}

		else if((ui_sys_event & EVT_BATTERY) != 0)
		{
			ui_sys_event &= ~EVT_BATTERY;
			Ap_ProcessEvt_Bat();				//process adc data (battery in)
		}

		else if((ui_sys_event & EVT_DISPLAY) != 0)
		{
			ui_sys_event &= ~EVT_DISPLAY;
			Ui_ProcessEvt_Display();			//dislay function
		}
	}
}
#endif