/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_fm_main.c
Creator: zj.zong			Date: 2009-05-23
Description: fm main loop(wait for interrupt setting ui_sys_event, and process event)
Others:
Version: V0.1
History:
	V0.1	2009-05-23		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_FM_MAIN_DEF

#include "sh86278_api_define.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include ".\UI\UI_FM\ui_fm_header.h"
#include ".\UI\UI_FM\ui_fm_var_ext.h"

extern void Ap_ProcessEvt_Bat(void);
extern void Ap_ProcessEvt_Usb(void);
extern void Ui_ProcessEvt_Key(void);
extern void Ap_processEvt_FM_Search(void);
extern void Ui_ProcessEvt_Timer(void);
extern void Ui_ProcessEvt_Display(void);
extern void Ap_Init_FM_Driver(void);

extern void Ap_Enable_IrqBit(void);
extern void Ap_FM_Init(void);

/*******************************************************************************
Function: Ui_FM_Main()
Description: fm main function
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_FM_Main(void)
{
	mReset_PC_Stack;			//reset PC STACK
	mReset_WatchDog;			//reset watchdog
	mSet_MCUClock_12M;

	Ap_Init_FM_Driver();	
	 
	Ap_Enable_IrqBit();   			//enable interrupt(mpeg cmd & special1 & flag irq, timer irq, ADC irq,USB Detect irq)

	Ap_FM_Init();				//init fm

	while(1)
	{
		mReset_WatchDog;		//reset watchdog

		if((ui_sys_event & EVT_USB) != 0)
		{
			ui_sys_event &= ~EVT_USB;
			Ap_ProcessEvt_Usb();		//plug usb , and enter usb mode
		}

		else if((ui_sys_event & EVT_KEY) != 0)
		{
			ui_sys_event &= ~EVT_KEY;
			Ui_ProcessEvt_Key();		//process key event
		}

		else if((ui_sys_event & EVT_TIMER) != 0)
		{
			ui_sys_event &= ~EVT_TIMER;
			Ui_ProcessEvt_Timer();		//20ms timer
		}

		else if((ui_sys_event & EVT_FM_SEARCH) != 0)
		{
			ui_sys_event &= ~EVT_FM_SEARCH;
			Ap_processEvt_FM_Search();	//fm auto search
		}

		else if((ui_sys_event & EVT_BATTERY) != 0)
		{
			ui_sys_event &= ~EVT_BATTERY;
			Ap_ProcessEvt_Bat();		//process adc data (battery in)
		}

		else if((ui_sys_event & EVT_DISPLAY) != 0)
		{
			ui_sys_event &= ~EVT_DISPLAY;
			Ui_ProcessEvt_Display();	//dislay function
		}
	}
}


#endif