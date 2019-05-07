/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_poweroff.c
Creator: zj.zong					Date: 2009-04-22
Description:
Others:
Version: V0.1
History:
	V0.1	2009-04-20		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_POWEROFF_DEF

#include "sh86278_api_define.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"

extern void Ui_PowerOn_Backlight(void);
extern void Ui_PowerOff_Backlight(void);
extern void Ap_Delay_1s(void);
extern void Ap_Waiting_KeyPlay_Release(void);
extern void Api_Delay_1ms(uc8 time_cut);
extern void Api_Display_Picture(ui16 pic_idx);
extern void Api_LcdDriver_Clr_AllPage(void);
extern void Api_LcdDriver_PowerDown(void); 

void Ui_Check_PowerOff_Type(void);
void Ui_Display_ByebyePicture(void);
/*******************************************************************************
Function: Ui_PowerOff_Main()
Description: power off mp3, and DC-DC down
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_PowerOff_Main(void)
{	
	uc8 uc_cnt;
	mReset_PC_Stack;							//initial Stack
	MR_IE0 = 0x00;								//disable all irq
	mReset_WatchDog;

   	mSet_MCUClock_12M;
//	MR_MODEN1 = 0x00;						//disable all module except LCD Module
//	MR_PADATA |= 0x02;			//pa2 = 1 amp off
//	Ui_PowerOn_Backlight();
	Api_LcdDriver_Clr_AllPage();				//clr full screen 
//	Ui_Check_PowerOff_Type();				//check power off type, and display accordingly

	if(uc_poweroff_type != POWEROFF_NORMAL)	//if normal power off mp3, and not delay 1s
	{
		Ap_Delay_1s();
	}

	if(b_bat_low_flag == 1)
	{
		MR_PATYPE = 0xFA;
		for(uc_cnt = 0;  uc_cnt < 3; uc_cnt++)
		{
			MR_PADATA |= 0x78;				//pa2 = 1 amp off
			Ap_Delay_1s();	

			MR_PADATA &= 0x87;				//pa2 = 1 amp off
			Ap_Delay_1s();	
		}
	}
	Ui_PowerOff_Backlight();

	Api_LcdDriver_Clr_AllPage();				//clr full screen 
	Api_LcdDriver_PowerDown();

	MR_MODEN2 = 0x00;						//Disable LCMI

	mReset_WatchDog;
	MR_PATYPE = 0xFA;
	MR_PADATA &= 0x87;				//pa2 = 1 amp off
	Ap_Waiting_KeyPlay_Release();

	MR_PMUCON = 0x5a;						//set the PMU stop mode
	while(1)
	{
	}
}






/*******************************************************************************
Function: Ui_Check_PowerOff_Type()
Description: check power off type, and display accordingly
Calls:
Global: uc_poweroff_type
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Check_PowerOff_Type(void)
{
	switch(uc_poweroff_type)
	{
		case POWEROFF_LOW_VOL:
			uc_lcd_page_num = 3;
			uc_lcd_column_num =	0;
	   		Api_Display_Picture(NORM_STR_LOW_VOL_IDX + ALL_MENU_NUMBER * uc_language_idx);
			break;

		case POWEROFF_FLASH_ERR:
	   		uc_lcd_page_num = 3;
		 	uc_lcd_column_num =	0;
			Api_Display_Picture(NORM_STR_FLASH_ERR_IDX + ALL_MENU_NUMBER * uc_language_idx);
			break;

		case POWEROFF_NORMAL:
			Ui_Display_ByebyePicture();
			break;
	}
}

/*******************************************************************************
Function: Ui_Display_ByebyePicture()
Description: display byebye picture
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Display_ByebyePicture(void)
{
	uc8 i;

	for(i = 0; i < uc_poweroff_pic_num; i++)
	{
		mReset_WatchDog;					//clear watchdog
  		uc_lcd_page_num = 0;
		uc_lcd_column_num =	0;
		Api_Display_Picture(PIC_POWEROFF_BASE_IDX + i);
		Api_Delay_1ms(TIME_POWERON_DIS);
	}
}



#endif