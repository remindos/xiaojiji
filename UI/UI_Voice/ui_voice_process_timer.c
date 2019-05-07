/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_voice_process_timer.c
Creator: zj.zong					Date: 2009-05-22
Description: process voice timer event, as subroutine in voice mode
Others:
Version: V0.1
History:
	V0.1	2009-05-22		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_VOICE_PROCESS_TIMER_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include ".\UI\UI_Voice\ui_voice_header.h"
#include ".\UI\UI_Voice\ui_voice_var_ext.h"

extern void Ui_Prepare_StartPlay_File(void);
extern void Ui_Check_Refresh_MenuFlag(void);

extern void Ap_Enable_DAC(void);
extern void Ap_SwitchTo_MainMenu(void);
extern void Ap_SetWav_Stop(void);
extern void Ap_Disable_DAC(void);
extern void Ap_SetDaci_SampleRate(void);
extern void Ap_SetWav_Play(void);
extern bit  Ui_Init_PlayWav(void);
extern bit  Ui_Display_NoFile(void);
extern bit  Ap_NoFileTo_MainMenu(void);

extern void Api_Check_Battery(void);
extern void Api_Check_Idle_PowerOff(void);
extern void Api_Check_Sleep_PowerOff(void);
extern void Api_Check_BklightOff(void);
extern void Api_Detect_Headphone_Plug(void);
extern void Api_DynamicLoad_Voice_Target(uc8 load_code_idx);

void Ui_ProcessEvt_Timer(void);
void Ui_Check_Exit_VolumeMenu(void);
void Ui_Check_Refresh_ABReplay_End(void);
void Ui_Check_AB_Filker(void);
void Ui_GetCurrent_PlayTime(void);
void Ui_Check_Headphone_ShortProtect(void);
void Ui_ReturnTo_Voice_PlayMenu(void);
void Ui_ReturnTo_MenuSD(void);

void Ui_SetLcdFlag_0xfd(void);
void Ui_ReturnTo_Voice(void);
void Ui_Check_RefreshFreq(void);
void Ui_Init_TopIdx(void);
void Ui_CheckEvt_FillData(void);
void Ap_MpegState_Stop(void);
void Ui_Init_TopIdx(void);
#ifdef AVOID_POP_NOISE
void Ui_Check_Set_DacPower(void);
#endif

/*******************************************************************************
Function: Ui_ProcessEvt_Timer()
Description: process timer 100ms/once
Calls:
Global:uc_timer_cnt
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_ProcessEvt_Timer(void)
{
#ifdef AVOID_POP_NOISE
	Ui_Check_Set_DacPower();
#endif
	if(uc_timer_cnt >= 5)							//5*20ms = 100ms
	{
		uc_timer_cnt = 0;

		Api_Check_Battery();

		Api_Check_Sleep_PowerOff();					//sleep mode

		Api_Check_BklightOff();					//power off backlight
		
		Ui_Check_Exit_VolumeMenu();					//5s exit setting volume and menu window

		if((uc_voice_status == STATE_STOP) || (uc_voice_status == STATE_PAUSE))
		{
			Api_Check_Idle_PowerOff();			//if voice is pause/stop statee
		}
		else
		{
			ui_idle_poweroff_cnt = 0;

			Ui_GetCurrent_PlayTime();				//get the current played time

			if(uc_voice_status == STATE_PLAY)
			{
				Ui_CheckEvt_FillData();

				if(uc_window_no == WINDOW_VOICE )
				{
					Ui_Check_Refresh_ABReplay_End();
					Ui_Check_AB_Filker();
					Ui_Check_RefreshFreq();
				}
			}
		}

		Ui_Check_Headphone_ShortProtect();

		#ifdef PCB_WITH_SPEAKER
		Api_Detect_Headphone_Plug();
		#endif
	}
}


/*******************************************************************************
Function: Ui_Check_Exit_VolumeMenu()
Description: exit delete menu or volume setting mode
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Check_Exit_VolumeMenu(void)
{
	if(uc_window_no != WINDOW_VOICE)			//voice menu
	{
		if(uc_window_no <= WINDOW_VOICE_VOLUME)		//voice menu
		{
			Ui_Check_Refresh_MenuFlag();
		}
		if(((++uc_menu_exit_cnt) >= TIME_EXIT_MENU)&&(uc_window_no != WINDOW_VOICE_STOP_MENU_SD))
		{
			Ui_ReturnTo_Voice();
		}
	}
}


/*******************************************************************************
Function: Ui_GetCurrent_PlayTime()
Description: calculate current played time
Calls:
Global:uc_mp3_time_cnt, ui_mp3_cur_time
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_GetCurrent_PlayTime(void) 
{
	uc_voice_time_cnt++;							//for voice play timer counter

	if(uc_voice_status == STATE_PLAY)			//normal playing
	{
		if(uc_voice_time_cnt >= TIME_1S)
		{
			uc_voice_time_cnt = 0;
			if(ui_voice_cur_time < ui_voice_total_time)
			{
				ui_voice_cur_time++;
			}

			if((uc_voice_repeat_mode == REPEAT_PREVIEW) && (ui_voice_cur_time > TIME_PREVIEW))
			{
				Ap_SetWav_Stop();
				b_voice_file_end = 1;
				ui_sys_event |= EVT_VOICE_FILE_END;	//prepare to get next wav file
			}
			else
			{
				goto set_distime_flag;
			}
		}
	}
	else if(uc_voice_time_cnt >= TIME_FAST)			//backward & foreward
	{
		uc_voice_time_cnt = 0;
		if(uc_voice_status == STATE_FORWARD)
		{
			if(ui_voice_cur_time < ui_voice_total_time)
			{
				ui_voice_cur_time++;
				goto set_distime_flag;
			}
		}
		else
		{
			if(ui_voice_cur_time > 0)
			{
				ui_voice_cur_time--;
set_distime_flag:
				if(uc_window_no == WINDOW_VOICE)
				{
					uc_lcd_dis_flg |= 0x04;			//display timer
					ui_sys_event |= EVT_DISPLAY;
				}
			}
		}
	}
}

/*******************************************************************************
Function: check_ab_replay_end()
Description: check A-B replay end, compare ui_voice_cur_time with ui_voice_replay_b_time
Calls:
Global:uc_voice_replay_ab_flg, ui_voice_cur_time
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Check_Refresh_ABReplay_End(void)
{
	if(uc_voice_replay_ab_flg == 3)					//A-B replaying
	{
		if(ui_voice_cur_time >= ui_voice_replay_b_time)
		{
			ui_sys_event |= EVT_VOICE_AB_REPLAY;	//set the A-B replay
		}
	}
}



/*******************************************************************************
Function: Ui_Check_AB_Filker()
Description: flicker A-B replay pic
Calls:
Global:uc_voice_replay_ab_flg, uc_voice_ab_fliker_cnt
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Check_AB_Filker(void)
{
	if(uc_voice_replay_ab_flg != 0)				//check if repeat A-B
	{
		if((++uc_voice_ab_fliker_cnt) >= TIME_FLICKER_AB)
		{
			uc_voice_ab_fliker_cnt = 0;
			uc_lcd_dis_flg |= 0x10;					//set bit4, display A-B replay icon
			ui_sys_event |= EVT_DISPLAY;
		}
	}
}


/*******************************************************************************
Function: Ui_Check_RefreshFreq()
Description: 
Calls:
Global:
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Check_RefreshFreq(void)
{
	if(++uc_refresh_voice_cnt > TIME_REFRESH_VOICE_FREQ)
	{
		uc_refresh_voice_cnt = 0;
		if(++uc_refresh_pic_num >= DIS_VOICE_FREQ_NUM)
		{
			uc_refresh_pic_num = 0;
		}
		uc_lcd_dis_flg |= 0x80;
		ui_sys_event |= EVT_DISPLAY;
	}
}

/*******************************************************************************
Function: Ui_CheckEvt_FillData()
Description: 
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_CheckEvt_FillData(void)
{
	uc_filldata_cnt++;
	if(uc_filldata_cnt >= 10)		//100ms*10
	{
		uc_filldata_cnt = 0;
		Ap_MpegState_Stop();
		Ap_SetWav_Stop();
		uc_dynamicload_idx = 32;		  			// bit2
		Api_DynamicLoad_Voice_Target(CODE_VOICE_5_FSEEK_IDX);	//fseek 0
	  	Api_DynamicLoad_Voice_Target(CODE_VOICE_3_INIT_PLAY_IDX);	 //danamic load voice init code
		if(Ui_Init_PlayWav() == 1)		  //get wav file FS, BR, file name...
		{
			Ap_SetWav_Play();		  //if file is ok, and start play
		}
	}
}


/*******************************************************************************
Function: Ui_Check_Headphone_ShortProtect()
Description: check headphone short protect
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Check_Headphone_ShortProtect(void)
{
	if((MR_DACSTA & 0x07) != 0)
	{
		uc_headphone_short_time++;
		if(uc_headphone_short_time >= 5)
		{
			Ap_SetWav_Stop();

			Ap_Disable_DAC();
			Ap_Enable_DAC();
			Ap_SetDaci_SampleRate();
			uc_dynamicload_idx = 30;		  			// bit2
			Api_DynamicLoad_Voice_Target(CODE_VOICE_5_FSEEK_IDX);	//fseek according to byte offset

			if(b_voice_file_end == 0)		// 0 is ok
			{
				Ap_SetWav_Play();
			}
			else								//file end
			{
				ui_sys_event |= EVT_VOICE_FILE_END;
			}
		}
	}
	else
	{
		if((MR_DACPCON & 0x80) == 0)
		{
			MR_DACPCON |= 0x80;					//enable headphone short detection
		}
		uc_headphone_short_time = 0;
	}
}


/*******************************************************************************
Function: Ui_SetLcdFlag_0xfd()
Description: set clr lcd flag, and dis flag 0xff
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_SetLcdFlag_0xfd(void)	
{
	b_lcd_clrall_flg = 1;
	uc_lcd_dis_flg = 0xfd;		 //clr bit1 hold flag
}

/*******************************************************************************
Function: Ap_ReturnTo_Voice_StopMenu()
Description: return to voice menu
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************
void Ui_ReturnTo_Voice_StopMenu(void)
{
	uc_window_no = WINDOW_VOICE_STOP_MENU;
	uc_menu_cur_idx = uc_menu_cur_idx_bk;
	uc_menu_total_num = 3;

   	uc_menu_top_idx = 0;
	Ui_Init_TopIdx();	
	Ui_SetLcdFlag_0xfd();
}

 */
/*******************************************************************************
Function: Ui_ReturnTo_MenuSD()
Description: return to voice stop menu have SD/MMC card
*******************************************************************************/
void Ui_ReturnTo_MenuSD(void)
{
	uc_window_no = WINDOW_VOICE_STOP_MENU_SD;
	uc_menu_cur_idx = uc_menu_cur_idx_bk;
	uc_menu_total_num =5;// Voice_MenuSD_Num;
	Ui_SetLcdFlag_0xfd();
	Ui_Init_TopIdx();
	ui_sys_event |= EVT_DISPLAY;	
}

/*******************************************************************************
Function: Ap_ReturnTo_Voice_PlayMenu()
Description: return to voice menu
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_ReturnTo_Voice_PlayMenu(void)
{
	uc_window_no = WINDOW_VOICE_PLAY_MENU;
	uc_menu_cur_idx = 1;
	uc_menu_total_num = 3;

	uc_menu_top_idx = 0;
	Ui_Init_TopIdx();	
	Ui_SetLcdFlag_0xfd();
}


/*******************************************************************************
Function: Ap_ReturnTo_Voice()
Description: return to voice play main
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_ReturnTo_Voice(void)
{
	if(b_fb_nofile_flg == 0)
	{
		uc_window_no = WINDOW_VOICE;
		Ui_SetLcdFlag_0xfd();
		ui_sys_event = EVT_DISPLAY;	
	}
	else
	{
		if(b_sd_exist_flg == 0)			//if SD/MMC card not exist
		{
			Ap_NoFileTo_MainMenu();		
		}
		else
		{
			Ui_Display_NoFile();
			Ui_ReturnTo_MenuSD();		//when have SD/MMC card, and return to sd stop menu
		}	
	}	
}



/*******************************************************************************
Function: Ui_Init_TopIdx()
Description: set clr lcd flag, and dis flag 0xff
*******************************************************************************/
void Ui_Init_TopIdx(void)
{
	if(uc_menu_cur_idx > (DisMenu_MaxNum + uc_menu_top_idx))	//"1 <= uc_menu_cur_idx <= 10"
	{
		uc_menu_top_idx = uc_menu_cur_idx - DisMenu_MaxNum;
	}
	else if(uc_menu_cur_idx <= uc_menu_top_idx)
	{
		uc_menu_top_idx = uc_menu_cur_idx - 1;
	}		
}

#ifdef AVOID_POP_NOISE
void Ui_Check_Set_DacPower(void)
{
	if(b_voice_startplay_flg == 1)
	{
		uc_voice_startplay_cnt++;
		if(uc_voice_startplay_cnt > 30)
		{
			uc_voice_startplay_cnt = 0;
			if((MR_DACCLK & 0x04) != 0)
			{
				MR_DACCLK &= 0xfb;				//Enable DAC reference voltage
			}
		}
	}
}
#endif

#endif