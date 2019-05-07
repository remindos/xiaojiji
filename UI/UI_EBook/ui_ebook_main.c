/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_ebook_main.c
Creator: zj.zong		Date: 2009-05-04
Description: ebook main loop(wait for interrupt setting ui_sys_event, and process event)
Others:
Version: V0.1
History:
	V0.1	2009-05-04		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_EBOOK_MAIN_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include ".\UI\UI_EBook\ui_ebook_header.h"
#include ".\UI\UI_EBook\ui_ebook_var_ext.h"

extern void Ui_ProcessEvt_Key(void);
extern void Ui_ProcessEvt_Display(void);
extern void Ui_ProcessEvt_Timer(void);
extern void Ui_Display_NoFile(void);
extern void Ap_SwitchTo_MainMenu(void);
extern void Ui_ReturnTo_MenuSD(void);

extern void Api_DynamicLoad_Ebook_Target(uc8 code_idx);
extern void Ap_Enable_IrqBit(void);
extern void Ap_ProcessEvt_Bat(void);
extern void Ap_ProcessEvt_Usb(void);
extern void Ap_ProcessEvt_Msi(void);
extern void Ap_Check_Init_FlashSD(void);


/*******************************************************************************
Function: ebook_main()
Description: ebook main function
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void ebook_main(void)
{
	mReset_PC_Stack;			//reset PC STACK
	mReset_WatchDog;			//reset watchdog

	Ap_Enable_IrqBit();   		//enable interrupt(timer irq, ADC irq,USB Detect irq)
/*	Api_DynamicLoad_Ebook_Target(CODE_EBOOK_1_INIT_IDX); //init ebook parameter
	
	Ui_ProcessEvt_Display();				//display ebook filebrowser(first display)
	ui_sys_event &= ~EVT_DISPLAY;	*/
	Ap_Check_Init_FlashSD();

	if(b_ebook_nofile_flg == 1)
	{
		if(b_sd_exist_flg == 0)			//if SD/MMC card not exist
		{
			Ui_Display_NoFile();
			Ap_SwitchTo_MainMenu();		//return to main menu		
		}
		else
		{
			Ui_Display_NoFile();
			Ui_ReturnTo_MenuSD();		//when have SD/MMC card, and return to sd stop menu
		}
	}
//	Ap_ProcessEvt_Msi();
	Ui_ProcessEvt_Display();					//display ebook main menu(first display)
	ui_sys_event &= ~(EVT_DISPLAY | EVT_KEY);
				                            	//clr key event, it is invalid before first display

	while(1)
	{
		mReset_WatchDog;					//clr watchdog

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

		else if((ui_sys_event & EVT_DISPLAY) != 0)
		{
			ui_sys_event &= ~EVT_DISPLAY;
			Ui_ProcessEvt_Display();		//dislay one page ebook
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
	}
}


#endif