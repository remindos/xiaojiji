/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_voice_process_event.c
Creator: zj.zong					Date: 2009-04-24
Description: process voice events (process USB, bat, file error, file end AB replay)
			 as subroutine in voice mode
Others:
Version: V0.1
History:
	V0.1	2009-04-24		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_VOICE_PROCESS_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_Voice\ui_voice_header.h"
#include "..\..\UI\UI_Voice\ui_voice_var_ext.h"


extern void Ui_ProcessEvt_BatSub(void);
extern void Ui_PowerOn_Backlight(void);
extern void Ui_SetLcdFlag_0xfd(void);
extern void Ap_Exit_VoiceMode(void);
extern void Ap_PowerOff_MP3(void);
extern void Ap_SetWav_Stop(void); 
extern void Ap_SetWav_Play(void);
extern void Ui_Display_DrawSD(void);
extern void Ui_Display_NoFile(void);
extern void  Ui_ReturnTo_Voice(void);
extern void  Ui_Init_PlayWav(void);
extern void  Ui_ReturnTo_MenuSD(void);
extern bit	Ap_Check_SDExist(void);

extern bit	FS_Fread(void);
extern void Api_DynamicLoad_USB(void);
extern void Api_Cmd_Dbuf1_Filled_ValidData(void);
extern void Api_Cmd_Dbuf1_Filled_InvalidData(void);
extern void Api_DynamicLoad_Voice_Target(uc8 load_code_idx);
extern void Api_Command_ContinueDec(void);
extern void Ap_Play_Next_file(void);
extern unsigned char data	uc_msi_status;
extern	 void  Ap_NoFileTo_MainMenu();
//extern void Ui_Display_InsertCard(void);
void Ap_Check_Init_FlashSD(void);
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
	Ap_Exit_VoiceMode();					
	Ui_PowerOn_Backlight();						

	Api_DynamicLoad_USB();
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
	Ui_ProcessEvt_BatSub();							//get battery level

	uc_lcd_dis_flg |= 0x01;							//display battery
	ui_sys_event |= EVT_DISPLAY;	

	if((MR_USBCON & 0x1) == 0)						//if USB connected, and not power off
	{
		if(uc_bat_low_cnt >= BATTERY_LOW_CNT)
		{
			uc_poweroff_type = POWEROFF_LOW_VOL;
			Ap_PowerOff_MP3();
		}
	}


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
	
	if(b_msi_plug_flg == 0)				//only process unplug SD/MMC card event	
	{
		b_sd_exist_flg = 0;				//have not sd card
		uc_msi_status = 0xff;

		if(b_sd_flash_flg == 0)			//when in flash	
		{
			if(b_fb_nofile_flg == 1)		//allen
			{
                 Ap_NoFileTo_MainMenu();
			}
			else
			{
				switch(uc_window_no)
				{	
					case WINDOW_VOICE_STOP_MENU_SD:
						Ui_Display_DrawSD();
						Ui_ReturnTo_Voice();
						break;

					default:
						break;
				}
			}
		}
		else
		{				
			if(uc_voice_status != STATE_STOP)
			{
				Ap_SetWav_Stop();
			}
			else
			{
				mMPEG_Release_Dbuf;
			}

			b_sd_flash_flg = 0;			//switch to flash
			Ui_Display_DrawSD();

			uc_dynamicload_idx = 30;					//scan flash voice total num
			Api_DynamicLoad_Voice_Target(CODE_VOICE_9_SWITCH_CARD_IDX);
		//	uc_dynamicload_idx = 41;					//scan flash voice total num
			uc_dynamicload_idx = 41;			//init variable, restore parameter and init dac
        	Api_DynamicLoad_Voice_Target(CODE_VOICE_1_INIT1_IDX);
			Api_DynamicLoad_Voice_Target(CODE_VOICE_2_INIT2_IDX);	 //allen

			if(b_fb_nofile_flg == 1)
			{
            	Ap_NoFileTo_MainMenu();
			}
			else
			{
				Api_DynamicLoad_Voice_Target(CODE_VOICE_3_INIT_PLAY_IDX);
				Ui_Init_PlayWav();
				Ui_ReturnTo_Voice();
			}
		}
	}
	else										//refresh menu interface
	{
		if(uc_voice_status != STATE_PLAY)
		{			
			if(Ap_Check_SDExist())
			{
				if(uc_window_no == WINDOW_VOICE_STOP_MENU_FLASH)
				{
					uc_menu_cur_idx_bk = 2;
					Ui_ReturnTo_MenuSD();
				}	
			}	
		}						
	}
}

/*******************************************************************************
Function: Ap_Check_Init_FlashSD()
Description: init flash or sd card for seek total num etc.
*******************************************************************************/
void Ap_Check_Init_FlashSD(void)
{
    uc_dynamicload_idx = 40;			//init variable, restore parameter and init dac
	Api_DynamicLoad_Voice_Target(CODE_VOICE_1_INIT1_IDX); //allen


	if(Ap_Check_SDExist() == 1)			//SD card exist, and init ok
	{	
Label_SwitchCard:
		uc_dynamicload_idx = 30;		//switch flash or SD/MMC card
		Api_DynamicLoad_Voice_Target(CODE_VOICE_9_SWITCH_CARD_IDX);
		uc_dynamicload_idx = 41;			//init variable, restore parameter and init dac
	    Api_DynamicLoad_Voice_Target(CODE_VOICE_1_INIT1_IDX);
		Api_DynamicLoad_Voice_Target(CODE_VOICE_2_INIT2_IDX);	 //get num of dir "VOICE", and open corresponding wav file
	}
	else
	{
		b_sd_flash_flg = 0;				//select SD/MMC card
		b_sd_exist_flg = 0;				//if no file, and return to main menu	
		goto Label_SwitchCard;
	}	
}
/*******************************************************************************
Function: Ap_ProcessEvt_FileError()
Description: process file error
Calls:
Global: 1.if have played 5s, and play the next file
		2.otherwise, stop and display "file error"
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_ProcessEvt_FileError(void)
{
	if(uc_window_no == WINDOW_VOICE)
	{
		b_voice_file_err = 1;
		uc_lcd_dis_flg |= 0xfd;
		ui_sys_event |= EVT_DISPLAY;	
	}
	else
	{
	 	b_voice_file_end = 1;	
		ui_sys_event |= EVT_MOVIE_FILE_END;		//clr file end event and flag

	} 
}


/*******************************************************************************
Function: Ap_ProcessEvt_FileEnd()
Description: process file end
Calls:
Global: uc_load_code_idx, b_voice_get_next_file, uc_lcd_dis_flg, 
       	b_lcd_clrall_flg, ui_sys_event
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_ProcessEvt_FileEnd(void)
{
	uc_voice_status_bk = uc_voice_status;	//save the current state
	uc_voice_status = STATE_STOP;			//set current play state
	Ap_SetWav_Stop();

	Ap_Play_Next_file();

	if(uc_window_no == WINDOW_VOICE)
	{
		uc_lcd_dis_flg |= 0x5c;
		ui_sys_event |= EVT_DISPLAY;
	}
}


/*******************************************************************************
Function: Ap_ProcessEvt_FillDecBuf()
Description: fill dbuf with new data
Calls:
Global: ui_sys_event, uc_wav_fill_dbuf_flg
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_ProcessEvt_FillDecBuf(void)
{
	if((uc_voice_fill_dbuf_flg & 0x02) == 0)	//uc_voice_fill_dbuf_flg (bit1: 0--empty; 1--full)
	{
		uc_filldata_cnt = 0;
		mMPEG_Release_Dbuf1;						//mpeg release DBUF1 to MCU
		uc_voice_fill_dbuf_flg |= 2;

		uc_fs_assign_buf_flg = SFC_APP_DBUF1;		//assign dbuf(0x5000) to fs
		if(FS_Fread() == 0)							//get one sector data to fill dbuf
		{
			b_voice_file_end = 1;	 				 //set  file error
			ui_sys_event |= EVT_VOICE_FILE_END;
			mMPEG_Occupy_Dbuf1;						//mpeg occupy DBUF
			Api_Cmd_Dbuf1_Filled_InvalidData();	//dbuf be filled with invalid data 
		}
		else
		{
			mMPEG_Occupy_Dbuf1;						//mpeg occupy DBUF
			Api_Cmd_Dbuf1_Filled_ValidData();	//dbuf be filled with valid data
		}
	}
	else											//dbuf(0x5000) is full
	{
		mMPEG_Occupy_Dbuf1;							//mpeg occupy DBUF
		Api_Cmd_Dbuf1_Filled_InvalidData();		//dbuf if full
	}
}


/*******************************************************************************
Function: Ap_ProcessEvt_ABReplay()
Description: process A-B replay
			 uc_voice_replay_ab_flg(1:prepare set "A"; 2:prepare to set "B"; 3:"A"-"B")
Calls:
Global:  ui_voice_replay_a_time, 
         uc_lcd_dis_flg, ui_sys_event, uc_load_code_idx
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_ProcessEvt_ABReplay(void)
{
	Ap_SetWav_Stop();
	uc_dynamicload_idx = 31;
	Api_DynamicLoad_Voice_Target(CODE_VOICE_5_FSEEK_IDX); 	//appiont to A-

	Ap_SetWav_Play();
}


#endif