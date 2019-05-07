/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_rec_process_event.c
Creator: zj.zong					Date: 2009-04-24
Description: process record events (process USB, bat, ...)
			 as subroutine in record mode
Others:
Version: V0.1
History:
	V0.1	2009-06-02		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_REC_PROCESS_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_Record\ui_rec_header.h"
#include "..\..\UI\UI_Record\ui_rec_var_ext.h"

extern void Ui_ProcessEvt_BatSub(void);
extern void Ui_Display_Rec_NoSpace(void);
extern void Ui_PowerOn_Backlight(void);
extern void Ui_SetLcdFlag_0xfd(void);


extern void Ap_SetStopRec(void); 
extern void Ap_PowerOff_MP3(void);
extern void Ap_Exit_RecMode(void);
extern void Ap_Encode_Block(void);
extern void Ap_Get_NewRecFile(void);
extern void Api_DynamicLoad_USB(void);
extern void Api_DynamicLoad_Record_Target(uc8 load_code_idx);
extern bit FS_Fwrite(void);
extern bit Ap_Check_SDExist(void);

void Ap_ProcessEvt_Usb(void);
void Ap_ProcessEvt_Bat(void);
void Ap_ProcessEvt_Encode(void);
extern unsigned char data	uc_msi_status;
extern	 void  Ap_SwitchTo_MainMenu();
//extern void Ui_Display_InsertCard(void);
extern void Ui_Display_DrawSD(void);

extern void Ui_ReturnTo_Rec(void);
extern void Ap_Init_VoiceDir(void);
void Ui_ReturnTo_MenuSD(void);
/*******************************************************************************
Function: Ap_ProcessEvt_Usb()
Description: process usb event, enter usb mode
Calls:
Global:uc_load_code_idx
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_ProcessEvt_Usb(void)
{
	Ap_Exit_RecMode();			//exit record mode		
	Ui_PowerOn_Backlight();						

	Api_DynamicLoad_USB();	   //danamic load usb code
}


/*******************************************************************************
Function: Ap_ProcessEvt_Bat()
Description: process battery
Calls:
Global:uc_bat_low_cnt, uc_poweroff_type
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_ProcessEvt_Bat(void)
{
	Ui_ProcessEvt_BatSub();			//get battery level

	uc_lcd_dis_flg |= 0x01;			//display battery
	ui_sys_event |= EVT_DISPLAY;	

	if((MR_USBCON & 0x1) == 0)		//if USB connected, and not power off
	{
		if(uc_bat_low_cnt >= BATTERY_LOW_CNT)
		{
			uc_poweroff_type = POWEROFF_LOW_VOL;
			Ap_PowerOff_MP3();	    // power off mp3
		}
	}
}


/*******************************************************************************
Function: Ap_ProcessEvt_Encode()
Description: when fill 1010 sample(1010 word) to dbuf0&1 in interupt routinue,
			 interupt routinue will change playbuf, set adpcm encode event flag,
			 main loop program will do ADPCM encode here.
Calls:
Global: 	
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_ProcessEvt_Encode(void)
{
	Ap_Encode_Block();					//compress adc data to cbuf

	uc_fs_assign_buf_flg = SFC_APP_CBUF;//write cbuf
	if(FS_Fwrite() == 0)
	{
		b_rec_card_full_flg = 1;
		Ui_Display_Rec_NoSpace();		//display flash no space
		Ap_SetStopRec();

		Api_DynamicLoad_Record_Target(CODE_RECORD_7_GET_NEW_FILE_IDX);	//get new record file index and calculate the remaining time
		
		Ui_SetLcdFlag_0xfd();
	}
	b_rec_adcbuf_full_flg = 1;	//write one sector is over
}

/*******************************************************************************
Function: Ap_ProcessEvt_Msi()
Description: check SD/MMC card plug in or out, and process event corresponding  
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ap_ProcessEvt_Msi(void)
{
	if((ui_bklight_time == 0) && (uc_bklight_time_level != 0))
	{
		Ui_PowerOn_Backlight();
	}
	
	if(b_msi_plug_flg == 0)
	{
		b_sd_exist_flg = 0;				//have not sd card
		uc_msi_status = 0xff;

		if(b_sd_flash_flg == 1)
		{
			Ui_Display_DrawSD();

			if(uc_rec_status == STATE_RECORD)
			{
				uc_rec_status = STATE_STOP;
				b_rec_mode_flg = 0;	
				mSet_MCUClock_12M;	
				Api_DynamicLoad_Record_Target(CODE_RECORD_5_STOP_IDX);//write record file header......
			}

			b_sd_flash_flg=0;
			uc_dynamicload_idx = 30;				//FS_Finit
			Api_DynamicLoad_Record_Target(CODE_RECORD_A_SWITCH_CARD_IDX);

			Ap_Init_VoiceDir();			
		}
		Ui_ReturnTo_Rec();					
	}
	else										//refresh menu interface
	{
		if(uc_rec_status != STATE_RECORD)
		{			
			if(Ap_Check_SDExist())
			{
				if(uc_window_no == WINDOW_REC_MENU_FLASH)				
				{
					uc_menu_cur_idx = 2;
					Ui_ReturnTo_MenuSD();
				}		 	
			}	
		}		 					
	}	
}


#endif