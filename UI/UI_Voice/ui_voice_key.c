/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_voice_key.c
Creator: zj.zong				Date: 2009-05-22
Description: process key event
Others:
Version: V0.1
History:
	V0.1	2009-05-22		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_VOICE_PROCESS_KEY_DEF

#include "sh86278_api_define.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include ".\UI\UI_Voice\ui_voice_header.h"
#include ".\UI\UI_Voice\ui_voice_var_ext.h"

extern void Ui_SetLcdFlag_0xfd(void);
extern void Ui_Display_StringDeleting(void);
extern bit  Ui_Init_PlayWav(void);
extern void Ui_PowerOn_Backlight(void);
extern void Ui_ReturnTo_Voice(void);
extern void Ui_ReturnTo_Voice_PlayMenu(void);
extern void Ui_Init_TopIdx(void);

extern void Ap_PowerOff_MP3(void);
extern void Ap_SetWav_Stop(void);
extern void Ap_SetWav_Play(void);
extern void Ap_SetWav_Continue(void);
extern void Ap_SetWav_Pause(void);
extern void Ap_Mpeg_ExitReset(void);
extern void Ap_SwitchTo_MainMenu(void);
extern void Ap_Forward_Backward_Over(void);
extern void Ap_Exit_VoiceMode(void);
extern void Ui_Display_DrawSD(void);
extern void Ui_Display_NoFile(void);
extern void Ui_Display_MSWP(void);
extern  bit    Ap_Check_SDExist(void);
extern  bit    Ui_ReturnTo_MenuSD(void);

extern void Api_Key_Mapping(void);
extern void Api_Set_Volume(uc8 volume_level);
extern void Api_DynamicLoad_Voice_Target(uc8 load_code_idx);
extern void Api_DynamicLoad_CodeMpm1_8kExecute(void);
extern void Api_DynamicLoad_CodeCpm(void);
extern void Api_Delay_1ms(uc8 timer_cnt);

extern void Ap_Disable_DAC();
extern void Ap_Enable_DAC();
void Ui_PlayS_InVoice(void);
void Ui_NextS_InVoice(void);
void Ui_LastS_InVoice(void);
void Ui_SelNext_Menu(void);
void Ui_SelPrevious_Menu(void);
void Ui_NextL_InVoice(void);
void Ui_LastL_InVoice(void);
void Ui_NextLc_InVoice(void);
void Ui_LastLc_InVoice(void);
void Ui_VolS_InVoice(void);
void Ui_Set_VolInc(void);
void Ui_Set_VolDec(void);
void Ui_ModeS_InVoice(void);
void Ui_ModeS_In_PlayMenu(void);
//void Ui_ModeS_In_StopMenu(void);
void Ui_ModeS_InVoice_StopMenu_Flash(void);
void Ui_ModeS_In_RepeatMenu(void);
void Ui_ModeS_In_DeleteMenu(void);
void Ui_ModeS_In_MenuDeleteAll(void);
void Ui_Prepare_StartPlay_File(void);
void Ui_ModeS_InVoice_StopMenu_SD(void);
void Ui_Vol_CommonFun(void);

/*******************************************************************************
Function: Ui_ProcessEvt_Key()
Description: process key
Calls:
Global:uc_key_value
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_ProcessEvt_Key(void)
{
	ui_idle_poweroff_cnt = 0;					//reset the idle_cnt when press any key
	uc_menu_exit_cnt = 0;
	
	Api_Key_Mapping();

/*	if(b_key_hold_val == 0)						//not hold state
	{
		if(	uc_hold_cnt != 0)
		{
			uc_hold_cnt = 0;
		 	Ui_SetLcdFlag_0xfd();
		}	*/
		if((ui_bklight_time == 0) && (uc_bklight_time_level != 0))
		{
			Ui_PowerOn_Backlight();
			return;
		}
		Ui_PowerOn_Backlight();					//restore backlight
		switch(uc_key_value)
		{
		
			case KEY_PLAY_S:
			   	switch(uc_window_no)
				{
					case WINDOW_VOICE:
				    	if(b_voice_file_err == 1)
						{
							return;
						}
						Ui_PlayS_InVoice();
						break;

					case WINDOW_VOICE_PLAY_MENU:
					case WINDOW_VOICE_STOP_MENU_SD:
					case WINDOW_VOICE_STOP_MENU_FLASH:
						Ui_ReturnTo_Voice();
						break;

				 	case WINDOW_VOICE_MENU_DELALL:
					case WINDOW_VOICE_MENU_DEL:
						Ui_ReturnTo_Voice();
						break;

					default:
					 	Ui_ReturnTo_Voice_PlayMenu();
						break;
				}
				break;

			case KEY_PLAY_L:
				if(b_play_poweron_flg == 0)
				{
					uc_poweroff_type = POWEROFF_NORMAL;
					Ap_PowerOff_MP3();
				}
				break;
			
			case KEY_NEXT_S:
				switch(uc_window_no)
				{
					case WINDOW_VOICE:
						Ui_NextS_InVoice();
						break;

					case WINDOW_VOICE_VOLUME:
						 Ui_Set_VolInc();
						break;

					default:
						Ui_SelNext_Menu();
						break;	
				}
				break;

			case KEY_NEXT_L: 
				switch(uc_window_no)
				{
					case WINDOW_VOICE:
						Ui_NextL_InVoice();
						break;

					case WINDOW_VOICE_VOLUME:
						Ui_Set_VolInc();
						break;

					default:
						Ui_SelNext_Menu();
						break;	
				}
				break;
	
			case KEY_NEXT_LC:
  				switch(uc_window_no)
				{
					case WINDOW_VOICE:
						Ui_NextLc_InVoice();
						break;

					case WINDOW_VOICE_VOLUME:
 						Ui_Set_VolInc();
						break;

					default:
						Ui_SelNext_Menu();
						break;	
				}
				break;

			case KEY_NEXT_LE:
			case KEY_LAST_LE:
				switch(uc_window_no)
				{
					case WINDOW_VOICE:
						Ap_Forward_Backward_Over();  
						break;	
				}
				break; 		

			case KEY_LAST_S:

				switch(uc_window_no)
				{
					case WINDOW_VOICE:
						Ui_LastS_InVoice();
						break;

					case WINDOW_VOICE_VOLUME:
						Ui_Set_VolDec();
						break;

					default:
						Ui_SelPrevious_Menu();
						break;		
				}
				break;
			
			case KEY_LAST_L:

				switch(uc_window_no)
				{
					case WINDOW_VOICE:
						Ui_LastL_InVoice();
						break;

					case WINDOW_VOICE_VOLUME:
						Ui_Set_VolDec();
						break;

					default:
						Ui_SelPrevious_Menu();
						break;	
				}
				break;

			case KEY_LAST_LC:

				switch(uc_window_no)
				{
					case WINDOW_VOICE:
						Ui_LastLc_InVoice();
						break;

					case WINDOW_VOICE_VOLUME:
						Ui_Set_VolDec();
						break;

					default:
						Ui_SelPrevious_Menu();
						break;	
				}
				break;	

			case KEY_VOL_S:
				switch(uc_window_no)
				{
					case WINDOW_VOICE:
						Ui_VolS_InVoice();
						break;

					case WINDOW_VOICE_VOLUME:
						Ui_ReturnTo_Voice();
						break;

					case WINDOW_VOICE_PLAY_MENU:
					case WINDOW_VOICE_STOP_MENU_SD:
					case WINDOW_VOICE_STOP_MENU_FLASH:
					 	Ui_ReturnTo_Voice();
						break;

					case WINDOW_VOICE_MENU_REPEAT:
						Ui_ReturnTo_Voice_PlayMenu();
						break;

				    default:
						Ui_ReturnTo_Voice();
						break;
				}
				break;	

			case KEY_REC_L:
				if(((uc_user_option2&0x20) != 0)&&((uc_user_option2&0x08) == 0))
				{
					Ap_Exit_VoiceMode();
					b_record_key_flag = 1;
	                uc_load_code_idx = CODE_RECORD_0_MAIN_CPM_IDX;
	                Api_DynamicLoad_CodeCpm();					//load code into CPM //allen
	                uc_load_code_idx = CODE_RECORD_0_MAIN_IDX;
	                Api_DynamicLoad_CodeMpm1_8kExecute();		//load code into MPM1
				}
				break;



			case KEY_MODE_S:
	  			switch(uc_window_no)
				{
					case WINDOW_VOICE:
						Ui_ModeS_InVoice();
						break;

					case WINDOW_VOICE_VOLUME:
						Ui_ReturnTo_Voice();
						break;

					case WINDOW_VOICE_PLAY_MENU:
						Ui_ModeS_In_PlayMenu();
						break;

				/*	case WINDOW_VOICE_STOP_MENU:
						Ui_ModeS_In_StopMenu();
						break; */

					case WINDOW_VOICE_MENU_REPEAT:
						Ui_ModeS_In_RepeatMenu();
						break;

					case WINDOW_VOICE_STOP_MENU_FLASH:
						Ui_ModeS_InVoice_StopMenu_Flash();
						break;

					case WINDOW_VOICE_STOP_MENU_SD:
						Ui_ModeS_InVoice_StopMenu_SD();
						break;

					case WINDOW_VOICE_MENU_DEL:
						Ui_ModeS_In_DeleteMenu();
						break;

					case WINDOW_VOICE_MENU_DELALL:
						Ui_ModeS_In_MenuDeleteAll();
						break;
						
					default:
						break;		
				}
				break;

			case KEY_MODE_L:
				Ap_SwitchTo_MainMenu();
				break;

			default:
				break;
		}
/*	}
	else									//hold state
	{
		Ui_PowerOn_Backlight();				//power on backlight
		uc_lcd_dis_flg |= 0x02;				//set bit1, display hold pic
	}	 */

	ui_sys_event |= EVT_DISPLAY;			//set the display event
}


/*******************************************************************************
Function: 1.Ui_PlayS_InVoice()		
Description: function of short press key play
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_PlayS_InVoice(void)
{
	if(b_fb_nofile_flg == 0)
	{
		if(uc_voice_replay_ab_flg == 0)
		{
			switch(uc_voice_status)
		  	{
		  		case STATE_PLAY:
			 		Ap_SetWav_Pause();
					break;

				case STATE_PAUSE:
					Ap_SetWav_Continue();
					break;
		
				default:
				    Ap_SetWav_Play();
					break;
			  }	
		}
		else
		{
			uc_voice_replay_ab_flg = 0;
		}
		
		uc_lcd_dis_flg |= 0x10;			//"display PLAY State", clr A-B pic
	}
}

/*******************************************************************************
Function: 1.Ui_NextS_InVoice()
Description: function of press key next
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_NextS_InVoice(void)
{
	switch(uc_voice_replay_ab_flg)
	{
		case 0:		 									//normal play
			if(b_fb_nofile_flg == 0)
			{
				uc_voice_status_bk = uc_voice_status;	//save the current state
				Ap_SetWav_Stop();
				uc_dynamicload_idx = 35;
				Api_DynamicLoad_Voice_Target(CODE_VOICE_4_SEL_FILE_IDX);	//sel next file
				Ui_Prepare_StartPlay_File();
				uc_lcd_dis_flg |= 0x5c;
			}
			break;
		case 1:		   									//A-B repeat
			ul_voice_replay_a_byte = ul_fs_byte_offset;	//save the current bytes A 
			ui_voice_replay_a_time = ui_voice_cur_time;	//save the current time A
			uc_voice_replay_ab_flg = 2;					//set replay flag
			uc_lcd_dis_flg |= 0x10;						//display replay A-B
			break;

		case 2:
			ui_voice_replay_b_time = ui_voice_cur_time;	//save the current time B
			uc_voice_replay_ab_flg = 3;					//set replay flag
			ui_sys_event |= EVT_VOICE_AB_REPLAY;		//set the A-B replay event
			uc_lcd_dis_flg |= 0x10;						//display replay A-B
			break;	
	}		
}

/*******************************************************************************
Function: 1.Ui_LastS_InVoice()
Description: function of press key last
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/ 
void Ui_LastS_InVoice(void)
{
	if(uc_voice_replay_ab_flg == 0)					//normal play
	{
		if(b_fb_nofile_flg == 0)
		{
			uc_voice_status_bk = uc_voice_status;	//save the current state
			Ap_SetWav_Stop();
			uc_dynamicload_idx = 36;		  	 //sel last file
			Api_DynamicLoad_Voice_Target(CODE_VOICE_4_SEL_FILE_IDX); 	
								
			Ui_Prepare_StartPlay_File();
			uc_lcd_dis_flg |= 0x5c;
		}
	}
	else
	{
		uc_voice_replay_ab_flg = 1;				//wait to set A-
		uc_lcd_dis_flg |= 0x10;
	}
}

/********************************************************************************
 next long in voice
********************************************************************************/
void Ui_NextL_InVoice(void)
{
	if(uc_voice_status == STATE_PLAY)
	{
		if(uc_voice_replay_ab_flg == 0)			//not support speed in A-B mode
		{
			Ap_SetWav_Stop();
			uc_voice_status =  STATE_FORWARD;
			uc_lcd_dis_flg |= 0x10	;		//display play state
		}
	}
	else
	{
		Ui_NextS_InVoice();
	}
}

/********************************************************************************
 last long in voice
********************************************************************************/
void Ui_LastL_InVoice(void)
{
	if(uc_voice_status == STATE_PLAY)
	{
		if(uc_voice_replay_ab_flg == 0)		//not support speed in A-B mode
		{
			Ap_SetWav_Stop();
			uc_voice_status = STATE_BCKWARD;
			uc_lcd_dis_flg |= 0x10;			//display play state
		}
	}
	else
	{
		Ui_LastS_InVoice();
	}
}
/********************************************************************************
 next long continue in voice
********************************************************************************/
void Ui_NextLc_InVoice(void)
{
	if((uc_voice_status == STATE_PAUSE) || (uc_voice_status == STATE_STOP))
	{
		Ui_NextS_InVoice();				//sel the next voice file
	}
}

/********************************************************************************
 last long continue in voice
********************************************************************************/
void Ui_LastLc_InVoice(void)
{
	if((uc_voice_status == STATE_PAUSE) || (uc_voice_status == STATE_STOP))
	{
		Ui_LastS_InVoice();
	}
}

/********************************************************************************
Select the next menu (uc_menu_cur_idx+1)
*********************************************************************************/
void Ui_SelNext_Menu(void)
{
	if(uc_menu_cur_idx >= uc_menu_total_num)
	{
		uc_menu_cur_idx = 1;
	}
	else
	{
		uc_menu_cur_idx++;
	}
	Ui_Init_TopIdx();
	uc_lcd_dis_flg = 0x58;	
}

/********************************************************************************
Select the last menu (uc_menu_cur_idx-1)
********************************************************************************/
void Ui_SelPrevious_Menu(void)
{
	if(uc_menu_cur_idx <= 1)
	{
		uc_menu_cur_idx = uc_menu_total_num;
	}
	else
	{
		uc_menu_cur_idx--;
	}
	Ui_Init_TopIdx();	
 	uc_lcd_dis_flg = 0x58;
}

/*******************************************************************************
Function: 1.Ui_VolS_InVoice()
		  2.Ui_Set_VolInc()
		  3.Ui_Set_VolDec()
		  4.Ui_Vol_CommonFun()
Description: function of press key vol
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/ 
void Ui_VolS_InVoice(void)
{
	if(b_fb_nofile_flg == 0)
	{
		uc_window_no = WINDOW_VOICE_VOLUME;
		Ui_SetLcdFlag_0xfd();
	}
}

/********************************************************************************
volume inc in voice
********************************************************************************/
void Ui_Set_VolInc(void)
{
	if(uc_voice_volume_level < VOLUME_LEVEL_MAX)	//volume level from 0 to 31
	{
		uc_voice_volume_level++;
	}
	else
	{
		uc_voice_volume_level = VOLUME_LEVEL_MAX;
	}

	Ui_Vol_CommonFun();
	uc_lcd_dis_flg = 0x10;
}

/********************************************************************************
volume dec in voice
********************************************************************************/
void Ui_Set_VolDec(void)
{
	if(uc_voice_volume_level > 0)			//volume level from 0 to 31
	{
		uc_voice_volume_level--;
	}
	else
	{
		uc_voice_volume_level = 0;
	}

	Ui_Vol_CommonFun();
	uc_lcd_dis_flg = 0x10;
}

/********************************************************************************
set volume in voice
********************************************************************************/
void Ui_Vol_CommonFun(void)
{
	Api_Set_Volume(uc_voice_volume_level);			//setting volume value
}

/*******************************************************************************
Function: 1.Ui_ModeS_InVoice()
		  2.Ui_ModeS_In_PlayMenu()
		  3.Ui_ModeS_In_StopMenu()
		  4.Ui_ModeS_In_RepeatMenu()
		  5.Ui_ModeS_In_DeleteMenu()
		  6.Ui_ModeS_In_MenuDeleteAll()
Description: function of press key mode
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/ 
/*void Ui_ModeS_InVoice(void)
{
	uc_voice_replay_ab_flg = 0;					//clr A-B replay flag

	if(ui_fb_total_num == 0)
	{
		Ap_SwitchTo_MainMenu();
	}
	else
	{
		if(uc_voice_status == STATE_PLAY)
		{
			uc_window_no = WINDOW_VOICE_PLAY_MENU;
		}
		else
		{
			uc_window_no = WINDOW_VOICE_STOP_MENU;
		}

		uc_menu_cur_idx = 1;					//init menu index and total item
		uc_menu_total_num = 3;
	   	uc_menu_top_idx = 0;
		Ui_Init_TopIdx();	
		Ui_SetLcdFlag_0xfd();
	}
} */
 /*******************************************************************************/
void Ui_ModeS_InVoice(void)
{
	switch(uc_voice_status)
	{
		case STATE_PLAY:
			if(uc_voice_replay_ab_flg != 0)
			{
				uc_voice_replay_ab_flg = 0;
				uc_lcd_dis_flg |= 0x10;					//set the bit4
			}
			else
			{
				uc_window_no = WINDOW_VOICE_PLAY_MENU;
				uc_menu_cur_idx = 1;
				uc_menu_total_num =3;  // Voice_PlayMenu_Num;
			}
			break;

		default:
			if(Ap_Check_SDExist() == 1)			//1:have SDand init ok, 0:no SD or init fail
			{
				uc_window_no = WINDOW_VOICE_STOP_MENU_SD;
				if(b_sd_flash_flg == 1)
				{
					uc_menu_cur_idx = 1;							//init menu index and total item
				}
				else
				{
					uc_menu_cur_idx = 2;	
				}
				uc_menu_total_num =5;// Voice_MenuSD_Num;	
			}
			else
			{
				uc_window_no = WINDOW_VOICE_STOP_MENU_FLASH;
				uc_menu_cur_idx = 1;
				uc_menu_total_num =3;// Voice_MenuFlash_Num;	
			}
			break;
	}

	uc_menu_top_idx = 0;
	Ui_Init_TopIdx();	
	Ui_SetLcdFlag_0xfd();
}

/********************************************************************************
key mode in voice play
********************************************************************************/
void Ui_ModeS_In_PlayMenu(void)
{
	uc_menu_cur_idx_bk = uc_menu_cur_idx;		//backup current menu idx

	if(uc_menu_cur_idx == 1)
	{
		uc_window_no = WINDOW_VOICE_MENU_REPEAT;
		uc_menu_cur_idx = uc_voice_repeat_mode + 1;	//init menu index and total item
		uc_menu_total_num = 5;
		Ui_SetLcdFlag_0xfd();
		uc_menu_top_idx = 0;
		Ui_Init_TopIdx();
	}
	else if(uc_menu_cur_idx == 2)
	{
		if(uc_voice_repeat_mode != REPEAT_PREVIEW)
		{
			uc_voice_replay_ab_flg = 1;			//wait to set "A-"
		}
		Ui_ReturnTo_Voice();
	}
	else
	{
		Ui_ReturnTo_Voice();
	}
}

/********************************************************************************
key mode in voice pause or stop
********************************************************************************/
void Ui_ModeS_InVoice_StopMenu_Flash(void)
{
	uc_menu_cur_idx_bk = uc_menu_cur_idx;		//backup current menu idx

	if(uc_menu_cur_idx == 1)
	{
		uc_window_no = WINDOW_VOICE_MENU_DEL;
		uc_menu_cur_idx = 2;					//init menu index and total item
		uc_menu_total_num = 2;
		b_lcd_clrall_flg = 1;
	}
	else if(uc_menu_cur_idx == 2)
	{
		uc_window_no = WINDOW_VOICE_MENU_DELALL;
		uc_menu_cur_idx = 2;					//init menu index and total item
		uc_menu_total_num = 2;
		b_lcd_clrall_flg = 1;
	}
	else
	{
		Ui_ReturnTo_Voice();
	}
}
/*******************************************************************************
Function: Ui_ModeS_InVoice_StopMenu_SD()
Description: function of short press key mode in menu have SD card
*******************************************************************************/
void Ui_ModeS_InVoice_StopMenu_SD(void)
{
	uc_menu_cur_idx_bk = uc_menu_cur_idx;	
	switch(uc_menu_cur_idx)
	{
		case 1:
			if(b_sd_flash_flg == 0)
			{
				b_sd_flash_flg = 1;
				uc_dynamicload_idx = 30;		//FS_Finit
				Api_DynamicLoad_Voice_Target(CODE_VOICE_9_SWITCH_CARD_IDX);
				if(uc_msi_status != 0)
				{
					Ui_Display_DrawSD();
					Ui_ReturnTo_MenuSD();
					return;	
				}
				goto Label_InitVoice;
			}
			Ui_ReturnTo_Voice();			
			break;

		case 2:
			if(b_sd_flash_flg == 1)
			{
				b_sd_flash_flg = 0;
				uc_dynamicload_idx = 30;		//FS_Finit
				Api_DynamicLoad_Voice_Target(CODE_VOICE_9_SWITCH_CARD_IDX);
Label_InitVoice:
				Ap_SetWav_Stop();
				//uc_dynamicload_idx = 41;		//Scan All Disk(Flash or SD)
				uc_dynamicload_idx = 41;			//init variable, restore parameter and init dac
	            Api_DynamicLoad_Voice_Target(CODE_VOICE_1_INIT1_IDX);
				Api_DynamicLoad_Voice_Target(CODE_VOICE_2_INIT2_IDX);
				
				if(b_fb_nofile_flg == 0)
				{
				Api_DynamicLoad_Voice_Target(CODE_VOICE_3_INIT_PLAY_IDX);
					if(Ui_Init_PlayWav() == 1)		//init info of current wav file(get FS, BR, filename...)
					{	
						Ap_SetWav_Play();			//if file is ok, and start play
					}	
				}
			}
			Ui_ReturnTo_Voice();
			break;

		case 3:
			if(b_fb_nofile_flg == 0)
			{
				uc_window_no = WINDOW_VOICE_MENU_DEL;
				uc_menu_cur_idx = 2;					//init menu index and total item
				uc_menu_total_num =2;// Voice_MenuDel_Num;
				uc_lcd_dis_flg = 0x14;
				b_lcd_clrall_flg = 1;	
			}
			else
			{
				Ui_Display_NoFile();
				uc_menu_cur_idx_bk = 3;
				Ui_ReturnTo_MenuSD();
			}			
			break;

		case 4:
			if(b_fb_nofile_flg == 0)
			{
				uc_window_no = WINDOW_VOICE_MENU_DELALL;
				uc_menu_cur_idx = 2;					//init menu index and total item
				uc_menu_total_num =2;// Voice_MenuDelAll_Num;
				b_lcd_clrall_flg = 1;	
			}
			else
			{
				Ui_Display_NoFile();
				uc_menu_cur_idx_bk = 4;
				Ui_ReturnTo_MenuSD();	
			}
			break;

		case 5:
			if(b_fb_nofile_flg == 0)
			{
				Ui_ReturnTo_Voice();	
			}
			else
			{
				Ap_SwitchTo_MainMenu();	
			}				
			break;

		default:
			break;
	}
}


/********************************************************************************
key mode in voice repeat menu
********************************************************************************/
void Ui_ModeS_In_RepeatMenu(void)
{
	uc_voice_repeat_mode = uc_menu_cur_idx - 1;
	Ui_ReturnTo_Voice();
}

/********************************************************************************
key mode in voice delete menu
********************************************************************************/
void Ui_ModeS_In_DeleteMenu(void)
{
	if(uc_menu_cur_idx == 1)
	{
	   	if((b_sd_flash_flg == 1) && ((uc_user_option1 & 0x10) != 0) && ((MR_MSCON4 & 0x40) == 0))
		{
	    	Ui_Display_MSWP();
			Ui_ReturnTo_Voice();	
		}
	  else
	    {
		Ap_SetWav_Stop();
		 mMPEG_Enter_Rest;		//mpeg enter stop mode, release CBUF
		
		 Ui_Display_StringDeleting();
	    uc_dynamicload_idx = 20;		
		Api_DynamicLoad_Voice_Target(CODE_VOICE_6_DEL_FILE_IDX);  //delete one file	
		Ap_Mpeg_ExitReset();
	    	if(b_fb_nofile_flg == 0)
		  {
			Api_DynamicLoad_Voice_Target(CODE_VOICE_3_INIT_PLAY_IDX);	 //danamic load voice init code
		  }
			else
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
	    }
	}
	Ui_ReturnTo_Voice();
}

/********************************************************************************
key mode in voice delete all menu
********************************************************************************/
void Ui_ModeS_In_MenuDeleteAll(void)
{
	if(uc_menu_cur_idx == 1)
  {
	/*	Ap_SetWav_Stop();
		mMPEG_Enter_Rest;		//mpeg enter stop mode, release CBUF

		Ui_Display_StringDeleting();

		uc_dynamicload_idx = 21;	 //delete all file
		Api_DynamicLoad_Voice_Target(CODE_VOICE_6_DEL_FILE_IDX); 			
	
		Ap_Mpeg_ExitReset();
		ui_voice_cur_time = 0;
		ui_voice_total_time = 0;
	}*/	
	if((b_sd_flash_flg == 1) && ((uc_user_option1 & 0x10) != 0) && ((MR_MSCON4 & 0x40) == 0))
	{
		Ui_Display_MSWP();
		Ui_ReturnTo_Voice();	
	}
	else
	{
		Ap_SetWav_Stop();
		mMPEG_Enter_Rest;		//mpeg enter stop mode, release CBUF

		Ui_Display_StringDeleting();

		uc_dynamicload_idx = 21;	 //delete all file
		Api_DynamicLoad_Voice_Target(CODE_VOICE_6_DEL_FILE_IDX); 			
	
		Ap_Mpeg_ExitReset();		
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
  }			
	Ui_ReturnTo_Voice();
}

/********************************************************************************
prepare start play file
********************************************************************************/
void Ui_Prepare_StartPlay_File(void)
{
   	Api_DynamicLoad_Voice_Target(CODE_VOICE_3_INIT_PLAY_IDX);	 //danamic load voice init code
   	if(Ui_Init_PlayWav() == 1)
	{
		if(b_voice_get_next_file == 1)
		{
			switch(uc_voice_status_bk)
			{
				case STATE_PLAY:
				 	Ap_SetWav_Play();			//start play
					break;
				
				case STATE_PAUSE:
					Ap_SetWav_Play();			//start play
					Ap_SetWav_Pause();
					break;		
			}
		}
	}
	
	uc_lcd_dis_flg |= 0x5c;
}

/********************************************************************************
init play ok or error flag
********************************************************************************/
bit Ui_Init_PlayWav(void)
{
	if((b_voice_file_err == 0) && (b_voice_file_end == 0))
	{
		return 1;		//init ok
	}
	else
	{
		return 0;		//init fail
	}

}

#endif