/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_music_process_timer.c
Creator: zj.zong					Date: 2009-04-23
Description: process music timer event, as subroutine in music mode
Others:
Version: V0.1
History:
	V0.1	2009-04-23		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_MUSIC_PROCESS_TIMER_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include ".\UI\UI_Music\ui_music_header.h"
#include ".\UI\UI_Music\ui_music_var_ext.h"

extern void Ui_Display_NoFile(void);
extern void Ui_Sel_NextLast_File(void);
extern void Ui_Check_Refresh_MenuFlag(void);

extern void Ap_Check_Restore_LastFile(void);
extern void Ap_Restore_EQ(void);
extern void Ap_SetMP3_Stop(void);
extern void Ap_SetMP3_Play(void);
extern void Ap_SwitchTo_MainMenu(void);
extern void Ap_ProcessLrc(void);
extern void Ap_Disable_DAC(void);
extern void Ap_SetDaci_SampleRate(void);
extern void Ap_Enable_DAC(void);
extern bit	Ap_Init_MP3_Info(void);
extern void Ap_PowerOff_MP3(void);

extern void Api_Check_Battery(void);
void Api_Check_Sleep_PowerOff(void);
void Api_Check_Idle_PowerOff(void);
extern void Api_Check_BklightOff(void);
extern void Api_DynamicLoad_Music_Target(uc8 load_code_idx);

extern void Api_Command_Dis_Fade(void);
extern void Api_Command_Ena_FreqFade(void);
extern void Api_Detect_Headphone_Plug(void);
extern void Ap_MpegState_Stop(void);
extern void Ui_Init_TopIdx(void);
//extern void rtc_read(void);
//extern void rtc_read_all(void);
//extern void rtc_read_time(void);
extern void Set_Key_Sound(ui16 sound_idx);

// debug _jason write
extern void	Api_Display_8x6_ZiKu(uc8 ziku_idx);
extern void Ui_DisplayBrowser_ItemName(void);
// debug _jason write

void Ui_ProcessEvt_Timer(void);
void Ui_Check_Exit_VolumeMenu(void);
void Ui_Check_Manual_Replay(void);
void Ui_Check_Refresh_Id3(void);
void Ui_Check_Refresh_LRC(void);
void Ui_Check_ABReplay_End(void);
void Ui_Check_AB_Filker(void);
void Ui_GetCurrent_PlayTime(void);
void Api_Check_Mode_function(void);
void Api_LED_Display_Function(void);
void Ui_Check_Headphone_ShortProtect(void);
void Ui_SetLcdFlag_0xfd(void);
void Ui_ReturnTo_Music(void);
void Ui_Init_TopIdx(void);
void Ui_Check_RefreshFreq(void);
void Ui_Return_Menu_Setting(void);
void Ui_CheckEvt_FillData(void);
void Ui_ReturnTo_Browser(void);
void Ui_ReturnTo_MenuSD(void);
#ifdef AVOID_POP_NOISE
void Ui_Check_Set_DacPower(void);
#endif
void Api_Check_LockKey(void);
void PWM_LED(void);
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
	ui_Flashled_Count++;
	if(uc_Flashled_flg == 1)
	{
		PWM_LED(); 	//set Flash
		if(ui_Flashled_Count >= 100)
		{
			ui_Flashled_Count = 0;
			uc_sys_led_cunter++;
			if(uc_sys_led_cunter >= 30)
			{
				uc_sys_led_cunter = 0;
				uc_Flashled_flg = 2;
				uc_Setled_flg = 2;	
			}
		}
	}
	else
	{
		if(ui_Flashled_Count >= 100)
		{
			ui_Flashled_Count = 0;
			uc_sys_led_cunter++;
			if(uc_sys_led_cunter >= 10)
			{
				uc_sys_led_cunter = 0;
				uc_Flashled_flg = 1;
				uc_Setled_flg = 1;	
			}
		}				// set all Light
	}
	if(uc_timer_cnt >= 10)							//5*20ms = 100ms
	{
		uc_timer_cnt = 0;

		if((MR_PBDATA & MODE1KEY_MASK) != 0)   // Mode1 Enable
		{
			Api_Check_Battery();
	
	//		Api_Check_LockKey();			// Jason delete

			//Api_Check_Sleep_PowerOff();					//sleep mode
			Api_Check_Mode_function();
		/*	uc_sys_leddis_cnt++;
			if(uc_sys_leddis_cnt >= 2)
			{
				uc_sys_leddis_cnt = 0;
				Api_LED_Display_Function();
			} */
		//	Api_Check_Sleep_PowerOff();					//sleep mode
	
			//if(b_lrc_file_exist == 0)					//when display lrc, and not power off bklight
	
			if((uc_mp3_status == STATE_STOP) || (uc_mp3_status == STATE_PAUSE))
			{
				//if(uc_mp3_replay_ab_flg != 3)			//not A-B replay
				{
					Api_Check_Idle_PowerOff();			//if mp3 is pause/stop state and power off
				}
				if(uc_status_cnt++>10)
				{
				 	b_status_flg ^= 0x01;
					uc_status_cnt = 0;
					uc_lcd_dis_flg |= 0x10;
					ui_sys_event |= EVT_DISPLAY;
				}
			}
			else
			{
				if(b_status_flg == 0)
				{
					b_status_flg = 0x01;
					uc_status_cnt = 0;
					uc_lcd_dis_flg |= 0x98;
				}
				ui_idle_poweroff_cnt = 0;
	
				Ui_GetCurrent_PlayTime();				//get the current played time
	
				if(uc_mp3_status == STATE_PLAY)
				{
					Ui_CheckEvt_FillData();
	
					if(uc_window_no == WINDOW_MUSIC)
					{
						//if(b_lrc_file_exist == 0)
						{
							Ui_Check_Refresh_Id3();
						}
						//else
						{
						//	Ui_Check_Refresh_LRC();
						}
					
						//Ui_Check_ABReplay_End();
						//Ui_Check_AB_Filker();
						//Ui_Check_RefreshFreq();
	
					}
				}
			}
			Ui_Check_Headphone_ShortProtect();
		
			#ifdef PCB_WITH_SPEAKER
			Api_Detect_Headphone_Plug();
			#endif
	
			if((uc_window_no == WINDOW_SYS_RTCADJ))
			{
			 	uc_flicker_cnt++;
				if(uc_flicker_cnt>4)
				{
				 	uc_flicker_cnt = 0;
					b_flicker_flg ^= 0x01;
	
					uc_lcd_dis_flg |= 0xA0;
					ui_sys_event |= EVT_DISPLAY;
	
				}
			}
		}
	}
}

#define PWM_STEP	1
#define PWM_START	1
#define PWM_FREQ	100
#define PWM_GAP		50

void PWM_LED(void)
{
  	if(uc_pwm_mode == 0)
	{
		b_led_pa3 = 1;
		uc_pwmr_width += PWM_STEP;
		if(uc_pwmr_width == PWM_FREQ-PWM_STEP)	//99
		{
		 	uc_pwm_mode ++;	
			uc_pwmg_width = PWM_START;
			b_led_pa4 = 1;			
		}

	//	uc_pwmc_width = PWM_FREQ-10;
		
	}
	else if(uc_pwm_mode == 1)
	{
		uc_pwmr_width -= PWM_STEP;
		if(uc_pwmr_width == PWM_STEP)
		{
		 	//uc_pwmr_width = PWM_STEP;
			//uc_pwm_mode ++;	
			b_led_pa3 = 0;
		}
		if(uc_pwmg_width<PWM_FREQ-2*PWM_STEP)
		uc_pwmg_width += PWM_STEP;
		if(b_led_pa3 == 0)
		{
		 	if(uc_pwm_cnt++ == PWM_GAP)
			{
				uc_pwm_cnt = 0;
				uc_pwm_mode ++;
			
			}
		}
			
	}
	else if(uc_pwm_mode == 2)
	{
		//b_led_pa2 = 1;
		uc_pwmg_width += PWM_STEP;
		if(uc_pwmg_width ==PWM_FREQ-PWM_STEP)	// 99
		{
		 	uc_pwm_mode ++;	
			uc_pwmr_width = PWM_START;
			b_led_pa3 = 1;			
		}
	//	uc_pwmc_width = PWM_FREQ-30;

	}
	else if(uc_pwm_mode == 3)
	{
		uc_pwmg_width -= PWM_STEP;
		if(uc_pwmg_width ==PWM_STEP)
		{
		 	//uc_pwm_mode ++;	
			b_led_pa4 = 0;
		}
		if(uc_pwmr_width<PWM_FREQ-2*PWM_STEP)
		uc_pwmr_width += PWM_STEP;
		if(b_led_pa4 == 0)
		{
		 	if(uc_pwm_cnt++ == PWM_GAP)
			{
				uc_pwm_cnt = 0;
				uc_pwm_mode = 0;
			
			}
		}
	}
/*	else if(uc_pwm_mode == 4) 
	{
		uc_pwmb_width += PWM_STEP;			
		if(uc_pwmb_width ==PWM_FREQ-PWM_STEP)
		{
		 	uc_pwm_mode ++;	
			uc_pwmw_width = PWM_START;
			b_led_pa6 = 1;			
		}

	//	uc_pwmc_width = PWM_FREQ - 50;

	}
	else if(uc_pwm_mode == 5)
	{
		uc_pwmb_width -= PWM_STEP;
		if(uc_pwmb_width ==PWM_STEP)
		{
		 	//uc_pwm_mode ++;	
			b_led_pa5 = 0;
		}
		if(uc_pwmw_width<PWM_FREQ-2*PWM_STEP)
		uc_pwmw_width += PWM_STEP;
		if(b_led_pa5 == 0)
		{
		 	if(uc_pwm_cnt++ == PWM_GAP)
			{
				uc_pwm_cnt = 0;
				uc_pwm_mode ++;
			
			}
		}
	}
	else if(uc_pwm_mode == 6) 
	{
		uc_pwmw_width += PWM_STEP;			
		if(uc_pwmw_width ==PWM_FREQ-PWM_STEP)
		{
		 	uc_pwm_mode ++;	
			uc_pwmr_width = PWM_START;
			b_led_pa3 = 1;			
		}

	//	uc_pwmc_width = PWM_FREQ - 50;

	}
	else 
	{
		uc_pwmw_width -= PWM_STEP;
		if(uc_pwmw_width ==PWM_STEP)
		{
		 	//uc_pwm_mode = 0;	
			b_led_pa6 = 0;
		}
		if(uc_pwmr_width<PWM_FREQ-2*PWM_STEP)
		uc_pwmr_width += PWM_STEP;
		if(b_led_pa6 == 0)
		{
		 	if(uc_pwm_cnt++ == PWM_GAP)
			{
				uc_pwm_cnt = 0;
				uc_pwm_mode = 0;
			
			}
		}	
	}*/	
}

void check_led(void)
{
	if(uc_Setled_flg == 1)
	{
		if(b_led_pa3!=0)
		{
		 	if(uc_200us_timer_cnt>uc_pwmr_width) 
			{
				MR_PADATA |= 0x08;//pa3 = 1
				MR_PADATA |= 0x20;//pa4 = 1				
			}
			else 
			{
				MR_PADATA &= 0xDF;	//pa4 = 0
			 	MR_PADATA &= 0xF7;	//pa3 = 0
				
			}	
		}
		else 
		{
			MR_PADATA |= 0x08;//pa3 = 1
			MR_PADATA |= 0x20;//pa4 = 1	
		}
		if(b_led_pa4!=0)
		{
		 	if(uc_200us_timer_cnt>uc_pwmg_width) 
			{
				MR_PADATA |= 0x40;//pa5 = 1
				MR_PADATA |= 0x10;//pa6 = 1					
			}
			else 
			{
			 	MR_PADATA &= 0xBF;	//pa5 = 0
				MR_PADATA &= 0xEF;	//pa6 = 0
			}	
		}
		else 
		{
			MR_PADATA |= 0x40;//pa5 = 1
			MR_PADATA |= 0x10;//pa6 = 1		
		};
	}
	else
	{
		MR_PADATA |= 0x08;//pa3 = 1
		MR_PADATA |= 0x20;//pa4 = 1
		MR_PADATA |= 0x40;//pa5 = 1
		MR_PADATA |= 0x10;//pa6 = 1			
	}
/*	if(b_led_pa5!=0)
	{
	 	if(uc_200us_timer_cnt>uc_pwmb_width) 
		{
				MR_PADATA |= 0x08;//pa3 = 1
				MR_PADATA |= 0x20;//pa4 = 1				
		}
		else 
		{
			MR_PADATA &= 0xDF;	//pa4 = 0
		 	MR_PADATA &= 0xF7;	//pa3 = 0
			
		}	
	}
//	else 
//	{
//		MR_PADATA |= 0x08;//pa3 = 1
//		MR_PADATA |= 0x20;//pa4 = 1	
//	}

	if(b_led_pa6!=0)
	{
	 	if(uc_200us_timer_cnt>uc_pwmw_width) 
		{
			MR_PADATA |= 0x40;//pa5 = 1
			MR_PADATA |= 0x10;//pa6 = 1					
		}
		else 
		{
			
		 	MR_PADATA &= 0xBF;	//pa5 = 0
			MR_PADATA &= 0xEF;	//pa6 = 0
		}	
	}	 */
//	else 
//	{
//		MR_PADATA |= 0x40;//pa5 = 1
//		MR_PADATA |= 0x10;//pa6 = 1		
//	}
}


void Api_Check_Idle_PowerOff(void)
{
	if((uc_idle_poweroff_level != 0)&&(uc_window_no != WINDOW_MUSIC_BYEBYE))
	{
		if((++ui_idle_poweroff_cnt) >= ui_idle_poweroff_time)
		{
			uc_poweroff_type = POWEROFF_NORMAL;
			//Ap_PowerOff_MP3();
			Set_Key_Sound(KEY_SOUND_POWEROFF);
			uc_next_action = ACTION_POWEROFF;
			uc_logo_idx = 0;
			uc_logo_cnt = 0;
			uc_window_no = WINDOW_MUSIC_BYEBYE;
		}
	}
}

void Api_Check_LockKey(void)
{
	if(uc_lock_level != 0)
	{
		if(ui_lock_time != 0)
		{
			if((--ui_lock_time) == 0)
			{
				b_lock_flg = 1;	
				uc_lcd_dis_flg |= BIT4_MASK;
				ui_sys_event |= EVT_DISPLAY;
			}
		}
	} 
}

extern void Ap_ProcessEvt_FillDecBuf(void);
void Api_Check_Sleep_PowerOff(void)
{
	if((uc_sleep_poweroff_level != 0)&&(uc_window_no != WINDOW_MUSIC_BYEBYE))
	{
		
		if((--ui_sleep_poweroff_time) == 0)
			//if((uc_rtc_hour == uc_rtc_hour_off)&&(uc_rtc_minute == uc_rtc_minute_off))
			{
				uc_poweroff_type = POWEROFF_NORMAL;
				//Ap_PowerOff_MP3();
				Set_Key_Sound(KEY_SOUND_POWEROFF);
				uc_next_action = ACTION_POWEROFF;
				uc_logo_idx = 0;
				uc_logo_cnt = 0;
				uc_window_no = WINDOW_MUSIC_BYEBYE;
			}
		
	}
}

void Api_Check_Mode_function(void)
{
	if((MR_PBDATA & MODE1KEY_MASK) != 0)
	{
	//	MR_PADATA &= (~RLED_PA3PIN_MASK);
	//	MR_PADATA &= (~SPEAKER_CONTROL_MASK);

	}
	else if((MR_PBDATA & MODE2KEY_MASK) != 0)
	{
	//	MR_PADATA |= RLED_PA3PIN_MASK;
	//	MR_PADATA |= SPEAKER_CONTROL_MASK;	
	}
	else
	{
		// Colse
	}
} 
/*******************************************************************************
Function: Ui_Check_Exit_VolumeMenu(void) 
Calls:
Global:Null				
Description: 
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Check_Exit_VolumeMenu(void)
{
	if(uc_window_no == WINDOW_MUSIC_LOGO)
	{
	 	uc_logo_cnt++;
		if(uc_logo_cnt>3) //4
		{
		 	uc_logo_cnt = 0;
			uc_logo_idx++;
			if(uc_logo_idx>14) //14
			{
			 	Ui_ReturnTo_Music(); 
				//uc_window_no = WINDOW_MUSIC_TOP_MENU;
			}
		}
		ui_sys_event |= EVT_DISPLAY;	
	}
	else
	if(uc_window_no == WINDOW_MUSIC_BYEBYE)
	{
	 	uc_logo_cnt++;
		if(uc_logo_cnt>2)
		{
		 	uc_logo_cnt = 0;
			uc_logo_idx++;
			if(uc_logo_idx>13)
			{
			 	Ui_ReturnTo_Music(); 
			}
		}
		ui_sys_event |= EVT_DISPLAY;	
	}
	else if(uc_window_no == WINDOW_MUSIC_SHOW_LOCK)
	{
		if(++uc_menu_exit_cnt >10) Ui_ReturnTo_Music();
	}
	else if(uc_window_no == WINDOW_SYS_RTCADJ)
	{
	 	if(++uc_menu_exit_cnt >200)	Ui_ReturnTo_Music();
	}
	else if((uc_window_no == WINDOW_MUSIC_VOLUME)||(uc_window_no == WINDOW_MUSIC_NUM))
	{
	 	if(++uc_menu_exit_cnt >20)	Ui_ReturnTo_Music();
	}
	else

	if((uc_window_no != WINDOW_MUSIC)&&(uc_mp3_status != STATE_STOP)&&(uc_window_no != WINDOW_MUSIC_BOOK))				//music menu, eq, repeat, replay time, replay gap menu
	{
		//if(uc_window_no <= WINDOW_MUSIC_VOLUME)		//music menu
		{
			Ui_Check_Refresh_MenuFlag();
		
		  	if(((++uc_menu_exit_cnt) >= TIME_EXIT_MENU)&&(b_fb_nofile_flg == 0))
	    	{
			//Ap_Restore_EQ();
        
			Ui_ReturnTo_Music();
		    }
	    }
    }
}


/*******************************************************************************
Function: Ui_Check_Manual_Replay()
Description: for manual A-B replay
Calls:
Global:b_mp3_replay_mode, uc_mp3_replay_gap_cnt
Input: Null
Output:b_mp3_replay_start--set A-B replay start flag
Others:
********************************************************************************/
/*void Ui_Check_Manual_Replay(void)
{
	if(b_mp3_replay_mode == 1)
	{
		if((++uc_mp3_replay_gap_cnt) >= (uc_mp3_replay_gap * 10))	//gap(s)=10*100ms*gap
		{
			uc_mp3_replay_gap_cnt = 0;
			b_mp3_replay_start = 1;
			ui_sys_event |= EVT_MUSIC_AB_REPLAY;					//set the A-B replay
		}
	}
}*/


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
	uc_mp3_time_cnt++;							//for mp3 play timer counter

	if(uc_mp3_status == STATE_PLAY)				//normal playing
	{
		if(uc_mp3_time_cnt >= TIME_1S)
		{
			uc_mp3_time_cnt = 0;
			if(ui_mp3_cur_time < ui_mp3_total_time)
			{
				ui_mp3_cur_time++;
			}
			
			if((uc_mp3_repeat_mode == REPEAT_PREVIEW) && (ui_mp3_cur_time > TIME_PREVIEW))
			{
				Ap_SetMP3_Stop();
				b_mp3_file_end = 1;
				ui_sys_event |= EVT_MUSIC_FILE_END;	//prepare to get next mp3 file
			}
			else
			{
				goto set_distime_flag;
			}
		}
	}
	else if(uc_mp3_time_cnt >= TIME_FAST)			//backward & foreward
	{
		uc_mp3_time_cnt = 0;
		if(uc_mp3_status == STATE_FORWARD)
		{
			if(ui_mp3_cur_time < ui_mp3_total_time)
			{
				ui_mp3_cur_time++;
				goto set_distime_flag;
			}
		}
		else
		{
			if(ui_mp3_cur_time > 0)
			{
				ui_mp3_cur_time--;
set_distime_flag:
				if(uc_window_no == WINDOW_MUSIC)
				{
					uc_lcd_dis_flg |= 0x04;			//display timer
					ui_sys_event |= EVT_DISPLAY;
					Ap_ProcessLrc();				//get the next lrc line
				}
			}
		}
	}
}


/*******************************************************************************
Function: Ui_Check_Refresh_Id3()
Description: refresh id3 infomation
Calls:
Global:uc_mp3_refresh_id3_cnt
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Check_Refresh_Id3(void)
{
	if((++uc_mp3_refresh_id3_cnt) >= TIME_REFRESH_ID3)	//7*100ms
	{
		uc_mp3_refresh_id3_cnt = 0;
		uc_lcd_dis_flg |= 0x80;
		ui_sys_event |= EVT_DISPLAY;
	}	
}



/*******************************************************************************
Function: Ui_Check_Refresh_LRC()
Description: refresh lrc
Calls:
Global:uc_mp3_refresh_lrc_cnt
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Check_Refresh_LRC(void)
{
	if((++uc_mp3_refresh_lrc_cnt) >= TIME_REFRESH_LRC)	//7*100ms
	{
		uc_mp3_refresh_lrc_cnt = 0;
		uc_lcd_dis_flg |= 0x40;
		ui_sys_event |= EVT_DISPLAY;
	}
}


/*******************************************************************************
Function: Ui_Check_ABReplay_End()
Description: check A-B replay end, compare ui_mp3_cur_time with ui_mp3_replay_b_time
Calls:
Global:uc_mp3_replay_ab_flg, ui_mp3_cur_time
Input: Null
Output:Null
Others:
********************************************************************************/
/*void Ui_Check_ABReplay_End(void)
{
	if(uc_mp3_replay_ab_flg == 3)					//A-B replaying
	{
		if(ui_mp3_cur_time >= ui_mp3_replay_b_time)
		{
			ui_sys_event |= EVT_MUSIC_AB_REPLAY;	//set the A-B replay
		}
	}
}*/


/*******************************************************************************
Function: Ui_Check_AB_Filker()
Description: flicker A-B replay pic
Calls:
Global:uc_mp3_replay_ab_flg, uc_mp3_ab_fliker_cnt
Input: Null
Output:Null
Others:
********************************************************************************/
/*void Ui_Check_AB_Filker(void)
{
	if(uc_mp3_replay_ab_flg != 0)						//check if repeat A-B
	{
		if((++uc_mp3_ab_fliker_cnt) >= TIME_FLICKER_AB)	//3*100ms
		{
			uc_mp3_ab_fliker_cnt = 0;
			uc_lcd_dis_flg |= 0x08;						//set bit3, display repeat icon
			ui_sys_event |= EVT_DISPLAY;
		}
	}
}*/

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
			Api_Command_Dis_Fade();
			Ap_SetMP3_Stop();
			ui_mp3_played_time = ui_mp3_cur_time;

			Ap_Disable_DAC();
			Ap_Enable_DAC();
			Ap_SetDaci_SampleRate();

			Api_Command_Ena_FreqFade();
			uc_dynamicload_idx = 38;
			_Api_DynamicLoad_Music_Target(CODE_MUSIC_9_FSEEK_IDX);//appointed to byte offset 0
			Ap_SetMP3_Play();  
		}
	}
	else
	{
		if((MR_DACPCON & 0x80) == 0)
		{
			MR_DACPCON |= 0x80;						//enable headphone short detection
		}
		uc_headphone_short_time = 0;
	}
}
/*******************************************************************************
Function: Ui_ReturnTo_MusicPause()
Description: return to music pause
*******************************************************************************/
void Ui_ReturnTo_MusicPause(void)
{
	b_mp3_save_flg = 0;
	uc_dynamicload_idx = 54;
	mSet_MCUClock_22M;						//set MCU clock is 22M
	Api_DynamicLoad_Music_Target(CODE_MUSIC_7_SELFILE_IDX);		//get current file index in disk
	mSet_MCUClock_12M;
	Ap_Init_MP3_Info();
	Ui_ReturnTo_Music();
}

/*******************************************************************************
Function: Ui_ReturnTo_Music()
Description: return to music play main
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_ReturnTo_Music(void)
{
	if(b_fb_nofile_flg == 0)
	{
		if(uc_mp3_status == STATE_STOP)
		Ap_Check_Restore_LastFile();
		//Ap_Restore_EQ();
		
		uc_mp3_dis_offset = 0;
		uc_window_no = WINDOW_MUSIC;
		Ui_SetLcdFlag_0xfd();
		ui_sys_event |= EVT_DISPLAY;
	}
	/*else		//no file
	{		if(b_sd_exist_flg == 0)			//if SD/MMC card not exist
		{
			Ui_Display_NoFile();
			Ap_SwitchTo_MainMenu();		//return to main menu		
		}
		else
		{
			Ui_Display_NoFile();
			Ui_ReturnTo_MenuSD();		//when have SD/MMC card, and return to sd stop menu
		}	
    }*/
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


/*******************************************************************************
refresh freq
********************************************************************************/
/*void Ui_Check_RefreshFreq(void)
{
	if(++uc_mp3_refresh_freq_cnt > TIME_REFRESH_FREQ)
	{
		uc_mp3_refresh_freq_cnt = 0;
		if(++uc_refresh_pic_num >= Dis_Music_Freq_Num)
		{
			uc_refresh_pic_num = 0;
		}
		uc_lcd_dis_flg |= 0x10;
		ui_sys_event |= EVT_DISPLAY;
	}
}*/

/*******************************************************************************
Function: Ui_ReturnTo_Music_StopMenu()
Description: return to music menu
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
/*void Ui_ReturnTo_Music_StopMenu(void)
{
	uc_window_no = WINDOW_MUSIC_STOP_MENU;
	uc_menu_cur_idx = uc_menu_cur_idx_bk;
	uc_menu_total_num = 4;
	Ui_Return_Menu_Setting();
}
/*******************************************************************************
Function: Ui_ReturnTo_MenuFlash()
Description: return to music stop menu
*******************************************************************************/
/*void Ui_ReturnTo_MenuFlash(void)
{
	uc_window_no = WINDOW_MUSIC_MENU_FLASH;
	uc_menu_cur_idx = uc_menu_cur_idx_bk;
	uc_menu_total_num = 4;
	Ui_Return_Menu_Setting(); //allen
	ui_sys_event |= EVT_DISPLAY;	
}*/
/*******************************************************************************
Function: Ui_ReturnTo_MenuSD()
Description: return to music stop menu have SD/MMC card
*******************************************************************************/
/*void Ui_ReturnTo_MenuSD(void)
{
	uc_window_no = WINDOW_MUSIC_MENU_SD;
	uc_menu_cur_idx = uc_menu_cur_idx_bk;
	uc_menu_total_num = 5;
	Ui_Return_Menu_Setting();  //allen
	ui_sys_event |= EVT_DISPLAY;
}*/

/*******************************************************************************
Function: Ui_Return_Menu_Setting()
Description: return to music menu
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Return_Menu_Setting(void)
{
   	uc_menu_top_idx = 0;
	Ui_Init_TopIdx();	
	Ui_SetLcdFlag_0xfd();
}
/*******************************************************************************
Function: Ui_ReturnTo_Browser()	 allen+
Description: return to music browser
*******************************************************************************/
void Ui_ReturnTo_Browser(void)
{
	if(b_fb_nofile_flg == 0)
	{

		Ui_SetLcdFlag_0xfd();	
	}
/*	else
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
	}	*/
}
/*******************************************************************************
Function: Ui_Check_Menu_FlahsSD(void) allen+
Description: 
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
/*void Ui_Check_Menu_FlahsSD(void)
{
	if(b_sd_exist_flg == 0)
	{
		if(uc_window_no == WINDOW_MUSIC_MENU_FLASH)
		{
			uc_menu_cur_idx_bk = 1;	
		}
		else if(uc_window_no == WINDOW_MUSIC_MENU_DELALL)
		{
			uc_menu_cur_idx_bk = 3;	
		}
		
		Ui_ReturnTo_MenuFlash();
	}
	else
	{
		if(uc_window_no == WINDOW_MUSIC_MENU_FLASH)
		{
			uc_menu_cur_idx_bk = 1;	
		}
		else if(uc_window_no == WINDOW_MUSIC_MENU_SD)
		{
			if(b_sd_flash_flg == 1)
			{
				uc_menu_cur_idx_bk = 1;	
			}
			else
			{
				uc_menu_cur_idx_bk = 2;	
			}	
		}
		else if(uc_window_no == WINDOW_MUSIC_MENU_DELALL)
		{
			uc_menu_cur_idx_bk = 4;	
		}

		Ui_ReturnTo_MenuSD();	
	}
}*/
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
		Ap_SetMP3_Stop();
		ui_mp3_played_time = ui_mp3_cur_time;
		uc_dynamicload_idx = 39;
		Api_DynamicLoad_Music_Target(CODE_MUSIC_9_FSEEK_IDX);
		if(Ap_Init_MP3_Info() == 1)
		{
			Ap_SetMP3_Play();	
		}
	}
}

/*******************************************************************************
Function: Ui_ReturnTo_Music_PlayMenu()
Description: return to music menu
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_ReturnTo_Music_PlayMenu(void)
{
	uc_window_no = WINDOW_MUSIC_PLAY_MENU;
	uc_menu_cur_idx = uc_menu_cur_idx_bk;
	uc_menu_total_num = 6;
	Ui_Return_Menu_Setting();
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
	uc_lcd_dis_flg = 0xfd;
}

#ifdef AVOID_POP_NOISE
void Ui_Check_Set_DacPower(void)
{
	if(b_mp3_startplay_flg == 1)
	{
		uc_mp3_startplay_cnt++;
		if(uc_mp3_startplay_cnt > 30)
		{
			uc_mp3_startplay_cnt = 0;
			if((MR_DACCLK & 0x04) != 0)
			{
				MR_DACCLK &= 0xfb;				//Enable DAC reference voltage
			}
		}
	}
}
#endif



#endif