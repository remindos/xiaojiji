/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_music_process_event.c
Creator: zj.zong					Date: 2009-04-24
Description: process music events (process USB, bat, file error, file end AB replay)
			 as subroutine in music mode
Others:
Version: V0.1
History:
	V0.1	2009-04-24		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_MUSIC_PROCESS_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_Music\ui_music_header.h"
#include "..\..\UI\UI_Music\ui_music_var_ext.h"

extern void Ui_ProcessEvt_BatSub(void);
extern void Ui_PowerOn_Backlight(void);
extern void Ui_Display_FileError(void);
extern void Ui_SetLcdFlag_0xfd(void);
extern void Ui_Sel_NextLast_File(void);
extern void Ui_SetLcdFlag_0xfd(void);
extern void Ui_Display_NoFile(void);
//extern void Ui_ReturnTo_MenuFlash(void);
//extern void Ui_ReturnTo_MenuSD(void);
extern void Ui_ReturnTo_Music(void);


extern bit Ap_Init_MP3_Info(void);
extern void Ap_SetMP3_Play(void);
extern void Ap_SetMP3_Pause(void);
extern void Ap_SetMP3_Stop(void);
extern void Ap_Disable_DAC(void);
extern void Ap_PowerOff_MP3(void);

extern bit	FS_Fread(void);
extern void Api_DynamicLoad_USB(void);
extern void Api_DynamicLoad_PowerOff(void);
extern void Api_Delay_1ms(uc8 time_cnt);
extern void Api_Cmd_Dbuf1_Filled_ValidData(void);
extern void Api_Cmd_Dbuf1_Filled_InvalidData(void);
extern void Api_DynamicLoad_Music_Target(uc8 load_code_idx);
extern void Ap_SwitchTo_MainMenu(void);
//extern void Ui_Display_InsertCard(void);
extern void Ui_Display_DrawSD(void);

void Ap_Exit_MusicMode(void);
extern bit Ap_Check_SDExist(void);
extern void Ui_Select_Book(uc8 book_idx);

#ifdef SP_PLAY_RESERVEDMP3_DEF
extern void Ap_Rsv_FillMp3data(void);
extern void Ap_Rsv_Sel_NextMp3File(void);
#endif


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
	Ap_Exit_MusicMode();					
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
			b_bat_low_flag = 1;
			uc_poweroff_type = POWEROFF_LOW_VOL;
			Ap_PowerOff_MP3();
		}
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
	if(uc_window_no == WINDOW_MUSIC)
	{
		b_lcd_dis_file_err = 1;
		uc_lcd_dis_flg |= 0xfd;
		ui_sys_event |= EVT_DISPLAY;	
	}
	else
	{
	 	b_mp3_file_end = 1;	
		ui_sys_event |= EVT_MUSIC_FILE_END;		//clr file end event and flag
	} 
}


/*******************************************************************************
Function: Ap_ProcessEvt_FileEnd()
Description: process file end
Calls:
Global: uc_load_code_idx, b_mp3_get_next_file, uc_lcd_dis_flg, 
       	b_lcd_clrall_flg, ui_sys_event
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_ProcessEvt_FileEnd(void)
{
	uc_mp3_status = STATE_STOP;				//set current play state
	Ap_SetMP3_Stop();
#ifdef SP_PLAY_RESERVEDMP3_DEF
	//Ap_Rsv_Sel_NextMp3File();
	if(b_rsv_mp3_flg == 1)
	{
		b_rsv_mp3_flg = 0;

		/*if(b_play_resume != 0)
		{
			ui_mp3_cur_time = ui_play_time_bak;
			ul_mp3_byte_offset_bk = ul_mp3_byte_offset_ply;	
		}*/
		if(uc_next_action == ACTION_NEXT_BOOK)
		{
			uc_next_action = 0;
			Ui_Select_Book(uc_select_idx);
			return ;
		}
		else if(uc_next_action==ACTION_POWEROFF)
		{
		 	uc_next_action = 0;
			Ap_PowerOff_MP3();	
		}		
		else if(uc_next_action==ACTION_STOP)
		{
			uc_next_action=0;
			uc_dynamicload_idx = 46;		//current file
			Api_DynamicLoad_Music_Target(CODE_MUSIC_7_SELFILE_IDX);
			if(Ap_Init_MP3_Info() == 1) 	//init info of current mp3(get id3&vbr info, process lrc file, get mp3 filename...)
			{
				Ap_SetMP3_Play();		  //if file is ok, and start play
				//Ap_SetMP3_Stop();

			   	
			}

			Ui_SetLcdFlag_0xfd();
			ui_sys_event |= EVT_DISPLAY;			//set the display event
			return ;
		}
				
		
	}
	else
#endif
	{
		uc_dynamicload_idx = 43;
		Api_DynamicLoad_Music_Target(CODE_MUSIC_7_SELFILE_IDX);
	}

	if(Ap_Init_MP3_Info() == 1)
	{
		//if(b_mp3_get_next_file == 1)			//0:normal repeat and stop
		{
			Ap_SetMP3_Play();					//start play
		}
	}
	
	if(uc_window_no == WINDOW_MUSIC)
	{
		Ui_SetLcdFlag_0xfd();
	}
}


/*******************************************************************************
Function: Ap_ProcessEvt_FillDecBuf()
Description: fill dbuf with new data
Calls:
Global: b_mp3_file_end, ui_sys_event, uc_mp3_fill_dbuf_flg
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_ProcessEvt_FillDecBuf(void)
{
	if((uc_mp3_fill_dbuf_flg & 0x02) == 0)			//uc_mp3_fill_dbuf_flg (bit1: 0--empty; 1--full)
	{
		uc_filldata_cnt = 0;
		mMPEG_Release_Dbuf1;						//mpeg release DBUF1 to MCU
		
		uc_mp3_fill_dbuf_flg |= 2;

		uc_fs_assign_buf_flg = SFC_APP_DBUF1;		//assign dbuf(0x5000) to fs
#ifdef SP_PLAY_RESERVEDMP3_DEF
		if(b_rsv_mp3_flg == 1)
		{
			Ap_Rsv_FillMp3data();
		}
		else
#endif
		if(FS_Fread() == 0)							//get one sector data to fill dbuf
		{
			b_mp3_file_end = 1;
			ui_sys_event |= EVT_MUSIC_FILE_END;
			mMPEG_Occupy_Dbuf1;						//mpeg occupy DBUF
			Api_Cmd_Dbuf1_Filled_InvalidData();	//dbuf be filled with invalid data 
		}
		else
		{
			mMPEG_Occupy_Dbuf1;						//mpeg occupy DBUF
			Api_Cmd_Dbuf1_Filled_ValidData();	//dbuf be filled with valid data
		}
	}
	else
	{
		mMPEG_Occupy_Dbuf1;
		Api_Cmd_Dbuf1_Filled_InvalidData();		//dbuf if full
	}
}

/*******************************************************************************
Function: Ap_Check_Init_FlashSD()
Description: check init flahs or SD/MMC card
Calls:
Global:b_sd_exist_flg, b_sd_flash_flg
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ap_Check_Init_FlashSD(void)
{
//	if(Ap_Check_SDExist() == 1)		//SD card exist, and init ok
//	{	
//Label_SwitchCard:					//switch flash or SD/MMC card
		b_sd_flash_flg = 0;			//select SD/MMC card
		b_sd_exist_flg = 0;			//if no file, and return to main menu	
		uc_dynamicload_idx = 30;
		Api_DynamicLoad_Music_Target(CODE_MUSIC_C_SWITCH_CARD_IDX);
		uc_dynamicload_idx = 41;	//scan disk and get MUSIC total number	
		Api_DynamicLoad_Music_Target(CODE_MUSIC_1_INIT_IDX);
//	}
//	else
//	{
//		b_sd_flash_flg = 0;			//select SD/MMC card
//		b_sd_exist_flg = 0;			//if no file, and return to main menu	
//		goto Label_SwitchCard;
//	}	
}
/*******************************************************************************
Function: Ap_ProcessEvt_AB_Replay()
Description: process A-B replay
			 uc_mp3_replay_ab_flg(1:prepare set "A"; 2:prepare to set "B"; 3:"A"-"B")
Calls:
Global: b_mp3_replay_mode, uc_mp3_replay_time_cnt, uc_mp3_replay_time
        uc_mp3_replay_ab_flg, b_mp3_replay_start, uc_mp3_fseek_flg
        uc_lcd_dis_flg, ui_sys_event, uc_load_code_idx
Input: Null
Output:Null
Others:
********************************************************************************/
/*void Ap_ProcessEvt_AB_Replay(void)
{
	if(b_mp3_replay_mode == 1)						//manual replay
	{
		if(uc_mp3_replay_time_cnt >= uc_mp3_replay_time)
		{
			uc_mp3_replay_ab_flg = 1;				//prepare set A
			return;
		}

		if(b_mp3_replay_start == 1)
		{
			b_mp3_replay_start = 0;
			uc_mp3_replay_time_cnt++;				//manual replay, counter + 1
		}
		else
		{
			Ap_SetMP3_Pause();
			
			uc_lcd_dis_flg |= 0x10;					//"display PLAY State"
			ui_sys_event |= EVT_DISPLAY;
			return;
		}
	}
	
	Ap_SetMP3_Stop();							//stop A-B
	
	uc_dynamicload_idx = 36;
	Api_DynamicLoad_Music_Target(CODE_MUSIC_9_FSEEK_IDX);
	
	Ap_SetMP3_Play();
}*/
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
			if(b_fb_nofile_flg == 1)	
			{
			    Ui_Display_DrawSD();
				Ui_Display_NoFile();
				//Ap_SwitchTo_MainMenu();	
			}
			/*else
			{
				switch(uc_window_no)
				{	
					case WINDOW_MUSIC_MENU_SD:
						Ui_Display_DrawSD();
						uc_menu_cur_idx_bk = 1;
						Ui_ReturnTo_MenuFlash();
						break;

					default:
						break;
				}
			}*/
		}
		else
		{							
			if(uc_mp3_status != STATE_STOP)
			{
				Ap_SetMP3_Stop();
			}
			else
			{
				mMPEG_Release_Dbuf;
			}
			
			b_sd_flash_flg = 0;					//switch to flash
			Ui_Display_DrawSD();				//display SD/MMC card has draw
			b_mp3_restore_file_flg = 1;         //refresh ID3
			uc_dynamicload_idx = 30;			//FS_Finit
			Api_DynamicLoad_Music_Target(CODE_MUSIC_C_SWITCH_CARD_IDX);			
			uc_dynamicload_idx = 41;			//scan all disk				//allen
			Api_DynamicLoad_Music_Target(CODE_MUSIC_1_INIT_IDX);

			uc_window_no = WINDOW_MUSIC_TOP_MENU;
			/*if(b_fb_nofile_flg == 1)
			{
				Ui_Display_NoFile();
//				Ap_SwitchTo_MainMenu();	
			}
			else
			{
				uc_menu_cur_idx_bk = 1;
				Ui_ReturnTo_Music();
			}*/
		}
	}
	else										//refresh menu interface
	{
		b_sd_exist_flg = 0;
		if(uc_mp3_status != STATE_PLAY)
		{
			if(Ap_Check_SDExist())
			{
				/*if(uc_window_no == WINDOW_MUSIC_MENU_FLASH)
				{
					uc_menu_cur_idx_bk = 2;
					Ui_ReturnTo_MenuSD();
				}*/	
			}	
		}						
	}
}



/*******************************************************************************
Function: Ap_Exit_MusicMode()
Description: power off dac and save music parameter into flash
Calls:
Global:b_mp3_save_flg
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Exit_MusicMode(void)
{
	Ap_SetMP3_Stop();
//	MR_PADATA = 0x02;				//pa2 = 1 amp off
	Api_Delay_1ms(250);
	mMPEG_Disable;								//disable mpeg module
	Ap_Disable_DAC();						//power off DAC

	if(b_mp3_save_flg == 0)							//0:not save music parameter
	{
		Api_DynamicLoad_Music_Target(CODE_MUSIC_A_SAVE_PARA_IDX);
	}
}



#endif