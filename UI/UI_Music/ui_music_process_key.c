/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_music_process_key.c
Creator: zj.zong					Date: 2009-04-22
Description: process music key event, as subroutine in music mode
Others:
Version: V0.1
History:
	V0.1	2009-04-22		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_MUSIC_PROCESS_KEY_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include ".\UI\UI_Music\ui_music_header.h"
#include ".\UI\UI_Music\ui_music_var_ext.h"

extern void Ui_Display_BookName(uc8 idx);
extern void Ui_SetLcdFlag_0xfd(void);
extern void Ui_Set_VolInc(void);
extern void Ui_Set_VolDec(void);
extern void Ui_PowerOn_Backlight(void);
extern void Ui_Display_Deleting(void);
extern void Ui_Display_NoFile(void);
extern void Ui_Check_Menu_FlahsSD(void);
extern void Ui_ReturnTo_Music(void);
extern void Ui_ReturnTo_MusicPause(void);
extern void Ui_ReturnTo_Music_StopMenu(void);
extern void Ui_ReturnTo_Music_PlayMenu(void);
extern void Ui_Init_TopIdx(void);
extern void Ui_Return_Menu_Setting(void);	 
extern void Ap_Check_SDExist(void);	 //allen
extern void	Ui_Display_DrawSD();	 //allen
extern void	Ui_ReturnTo_MenuSD();    //allen
extern void Ui_Display_MSWP(void);	 //allen
extern void Api_LcdDriver_Set_BklightLevel(uc8 bklight_grade);

extern bit Ap_Init_MP3_Info(void);
extern void Ap_Check_EQ_Window(void);
extern void Ap_Mpeg_ExitReset(void);
extern void Ap_SetMP3_Play(void);
extern void Ap_SetMP3_Continue(void);
extern void Ap_SetMP3_Pause(void);
extern void Ap_SetMP3_Stop(void);
extern void Ap_SetStopWard(void);
extern void Ap_Exit_MusicMode(void);


extern void Ap_PowerOff_MP3(void);
extern void Ap_SwitchTo_MainMenu(void);
extern void Ap_Play_SelFile_InFileBrowser(void);  
extern void Ap_Save_Music_Parameter(void);
extern void Ap_EnterFolder_InBrowser(void);
extern void Ap_ExitFolder_InBrowser(void);

extern void Api_Key_Mapping(void);
extern void Api_Mpeg_EnterReset(void);
extern void Api_DynamicLoad_CodeMpm1_8kExecute(void);
extern void Api_DynamicLoad_CodeCpm(void);
extern void Api_DynamicLoad_Music_Target(uc8 load_code_idx);
extern void Api_Delay_1ms(uc8 timer_cnt);
extern void Api_Display_Picture(ui16 pic_idx);
extern void Api_Set_Volume(uc8 vol_level);
extern bit FS_Fopen_Appointed(ui16 file_idx);

extern void Ap_Init_ItemIdx(void);

extern void rtc_read(void);
extern void Ap_Switch_Disk_Process(uc8 disk_idx);

void Ui_StopWard(void);
void Ui_PlayS_InMusic(void);
void Ui_PlayS_InBrowser(void);
void Ui_NextS_InMusic(void);
void Ui_NextL_InMusic(void);
void Ui_NextLc_InMusic(void);
void Ui_SelNext_InBrowser(void);
void Ui_SelNext_Menu(void);
void Ui_LastS_InMusic(void);
void Ui_LastL_InMusic(void);
void Ui_LastLc_InMusic(void);
void Ui_SelPrevious_InBrowser(void);
void Ui_SelPrevious_Menu(void);
void Ui_Sel_NextLast_File(void);
void Ui_VolS_InMusic(void);
void Ui_ModeS_InMusic(void);
void Ui_ModeL_InMusic(void);
void Ui_ModeS_In_PlayMenu(void);
void Ui_ModeS_In_StopMenu(void);
void Ui_ModeS_In_EQMenu(void);
void Ui_ModeS_In_MenuFlash(void);
void Ui_ModeS_In_MenuSD(void);
void Ui_ModeS_In_RepeatMenu(void);
void Ui_ModeS_In_RtcTimeMenu(void);
void Ui_ModeS_In_RplGapMenu(void);
void Ui_ModeS_In_MenuDel(void);
void Ui_ModeS_In_Menu_DelAll(void);
void Ui_ModeL_Common(void);
void Ui_Vol_Common_Fun(void);
void Ui_Enter_VolumeSet(void);
void Ui_ReturnTo_Browser(void);
void Ui_ModeS_In_SysMenu_Contrast(void);
void Ui_ModeS_In_SysMenu(void);
void Ui_ModeS_In_sysMenu_BkLight_Time(void);
void Ui_ModeS_InTopMenu(void);
void Ui_ModeS_In_DiskMenu(void);


void parse_ir_value(void);
void Ui_Num_Key(uc8 key);
void Ui_Process_NumKey(ui16 num);
void Set_Key_Sound(ui16 sound_idx);
void Ui_IR_Process(void);

void Ui_Sel_Next_In_Contrast(void);
void Ui_Sel_Last_In_Contrast(void);

void Ui_Poweroff_KeyProc(uc8 cnt);
void Ui_ModeS_In_Powerff(void);

void Ui_Select_Book(uc8 book_idx);
void Ui_ModeS_InLockKey(void);

void Ui_NextFF(void);
void Ui_LastFB(void);
void Ui_Delay_2s(void);

code ui16 ui_lock_time_table[] = {0, 150, 300, 600, 900};
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
	if(key0_ir1==1)
	{	
		key0_ir1 = 0;
		parse_ir_value();
	}
		switch(uc_key_value)
		{
			case KEY_PLAY_S:
				MR_PADATA |= 0x78;
				Ui_PlayS_InMusic();
				break;

			case KEY_PLAY_L:
					if((b_play_poweron_flg == 0)&&(uc_window_no != WINDOW_MUSIC_BYEBYE))
					{
						uc_poweroff_type = POWEROFF_NORMAL;
						uc_logo_idx = 0;
						uc_logo_cnt = 0;
						uc_window_no = WINDOW_MUSIC_BYEBYE;
					}
					break;

			case KEY_LASTKEY_S:
				MR_PADATA |= 0x78;
				Ui_LastS_InMusic();
				break;

			case KEY_NEXTKEY_S:
				MR_PADATA |= 0x78;
				Ui_NextS_InMusic();
				break;

			case KEY_LASTKEY_L:	  // Volume Dec
			case KEY_LASTKEY_LC:
				MR_PADATA |= 0x78;
				Ui_Set_VolDec();
				break;

 			case KEY_NEXTKEY_L:	  // Volume Add
			case KEY_NEXTKEY_LC:
				MR_PADATA |= 0x78;
				Ui_Set_VolInc();
				break;

			case KEY_NEXT_S:
				MR_PADATA |= 0x78;
				Ui_NextS_InMusic();
				break;

		/*	case KEY_NEXT_L:
			case KEY_NEXT_LC:
				Ui_NextFF();
				break;

			case KEY_NEXT_LE:
			case KEY_LAST_LE:
				if(uc_window_no == WINDOW_MUSIC_NUM)
				{
					//Ui_StopWard();
					if(Ap_Init_MP3_Info() == 1) 	//init info of current mp3(get id3&vbr info, process lrc file, get mp3 filename...)
					{
						Ap_SetMP3_Play();		  //if file is ok, and start play
					}
					Ui_ReturnTo_Music();
				}
				break;
		*/
			case KEY_LAST_S:
				MR_PADATA |= 0x78;
				Ui_LastS_InMusic();
				break;

		/*	case KEY_LAST_L:
			case KEY_LAST_LC:
				Ui_LastFB();
	
				break;
		 */
			case KEY_VOL_S:
			case KEY_VOL_L:
			case KEY_VOL_LC:
				MR_PADATA |= 0x78;
				Ui_Set_VolInc();
				break;

			case KEY_VOL_DEC_S:
			case KEY_VOL_DEC_L:
			case KEY_VOL_DEC_LC:
				MR_PADATA |= 0x78;
				Ui_Set_VolDec();
				break;
		}

 	ui_sys_event |= EVT_DISPLAY;			//set the display event
}
void Ui_Delay_2s(void)
{
 	Api_Delay_1ms(250);
	Api_Delay_1ms(250);
	Api_Delay_1ms(250);
	Api_Delay_1ms(250);
	Api_Delay_1ms(250);
	Api_Delay_1ms(250);
	Api_Delay_1ms(250);
	Api_Delay_1ms(250);
}
void Ui_NextFF(void)
{

	if(uc_mp3_status != STATE_STOP) Ap_SetMP3_Stop();
	uc_dynamicload_idx = 41;			//sel next file	
	Api_DynamicLoad_Music_Target(CODE_MUSIC_7_SELFILE_IDX);

}
void Ui_LastFB(void)
{
	if(uc_mp3_status != STATE_STOP) Ap_SetMP3_Stop();
	uc_dynamicload_idx = 42;			//sel last file	
	Api_DynamicLoad_Music_Target(CODE_MUSIC_7_SELFILE_IDX);

}
void Ui_Select_Book(uc8 book_idx)
{
	//uc_save_mode_idx = book_idx; 	//for display
	if(uc_mp3_status != STATE_STOP) Ap_SetMP3_Stop();

	uc_fs_store_cluster1 = 0;		
	uc_fs_store_cluster2 = 0;
	uc_fs_store_length1 = 0;
	uc_fs_store_length2	= 0;

	if(uc_disktype_flg != 0x81)
	{
		Ap_Switch_Disk_Process(1);
	}
/*	if(b_fb_nofile_flg == 0)
	{
			
		if(Ap_Init_MP3_Info() == 1) 	//init info of current mp3(get id3&vbr info, process lrc file, get mp3 filename...)
		{
			Ap_SetMP3_Play();		  //if file is ok, and start play
			Ap_SetMP3_Stop();
		}
	}*/

	uc_book_idx = book_idx;
	uc_dynamicload_idx = 50;
	Api_DynamicLoad_Music_Target(CODE_MUSIC_7_SELFILE_IDX);

	uc_window_no = WINDOW_MUSIC_MENU_BROWSER;
	Ap_Init_ItemIdx();
	Ui_SetLcdFlag_0xfd();
	ui_fb_sel_item_idx = 0;
	ui_fb_cur_item_idx = ui_fb_sel_item_idx;
	ui_fb_top_item_idx = ui_fb_sel_item_idx;
}

void Ui_Num_Key(uc8 key)
{
	if(uc_window_no == WINDOW_MUSIC_INPUT)
	{
		ui_num_key = ui_num_key%1000;
		ui_num_key = ui_num_key*10+key;
		
	}
	else if(uc_window_no == WINDOW_MUSIC)
	{
		uc_window_no = WINDOW_MUSIC_INPUT;
		ui_num_key = key;
		b_lcd_clrall_flg = 1;
		b_num_flg = 1;
	}
 	
	
}
void Ui_Process_NumKey(ui16 num)
{
	if(num == 0) return ;
	//if(uc_mp3_status != STATE_STOP)
		Ap_SetMP3_Stop();
/*	if(b_rsv_mp3_flg !=0)
	{
	 	if(num>ui_rsv_mp3_len)
		{
		 	ui_rsv_mp3_idx = ui_rsv_mp3_base+ui_rsv_mp3_len-1;
		}
		else
		{
			ui_rsv_mp3_idx = ui_rsv_mp3_base+num-1;
		}
		if(Ap_Init_MP3_Info() == 1)		
		{
			Ap_SetMP3_Play();				
		}

	}
	else*/
 	//if((num<(ui_fs_special_file_num_indir+1))&&(num != 0))
	{
		//if(uc_mp3_status != STATE_STOP)
		//	Ap_SetMP3_Stop();
		ui_mp3_idx = num;
		ui_base_idx = 0;
		uc_dynamicload_idx = 51;
		Api_DynamicLoad_Music_Target(CODE_MUSIC_7_SELFILE_IDX);

		if(uc_sysgetlrc_flag == 0)
		{
			ui_mp3_idx = ui_mp3_idx_indir;
		}
		if(Ap_Init_MP3_Info() == 1)		
		{
			Ap_SetMP3_Play();				
		}
	}
	//Ui_SetLcdFlag_0xfd();
	Ui_ReturnTo_Music();	
}

void parse_ir_value(void)
{
	uc8 tmp;
	if(ir_data_buf[1]==0xFF)
	{
		tmp = ir_data_buf[3] ^ 0xFF;
		if(tmp == ir_data_buf[2])
		{
			//uc_key_value=ir_data_buf[2];
			if(ir_data_buf[2] == IR_PLAY_S) uc_key_value = KEY_PLAY_S;
//			else if(ir_data_buf[2] == IR_MODE_S) uc_key_value = KEY_MODE_S;
			else if(ir_data_buf[2] == IR_NEXT_S) uc_key_value = KEY_NEXT_S;
			else if(ir_data_buf[2] == IR_LAST_S) uc_key_value = KEY_LAST_S;
			else if(ir_data_buf[2] == IR_VOL_INC_S) uc_key_value = KEY_VOL_INC_S;
			else if(ir_data_buf[2] == IR_VOL_DEC_S) uc_key_value = KEY_VOL_DEC_S;

//			else if(ir_data_buf[2] == IR_S1_S) uc_key_value = KEY_S1_S;
//			else if(ir_data_buf[2] == IR_S2_S) uc_key_value = KEY_S2_S;
//			else if(ir_data_buf[2] == IR_S3_S) uc_key_value = KEY_S3_S;
//			else if(ir_data_buf[2] == IR_S4_S) uc_key_value = KEY_S4_S;
//			else if(ir_data_buf[2] == IR_MUTE_S) uc_key_value = KEY_MUTE_S;
//			else if(ir_data_buf[2] == IR_NUM0) uc_key_value = KEY_NUM0;
//			else if(ir_data_buf[2] == IR_NUM1) uc_key_value = KEY_NUM1;
//			else if(ir_data_buf[2] == IR_NUM2) uc_key_value = KEY_NUM2;
//			else if(ir_data_buf[2] == IR_NUM3) uc_key_value = KEY_NUM3;
//			else if(ir_data_buf[2] == IR_NUM4) uc_key_value = KEY_NUM4;
//			else if(ir_data_buf[2] == IR_NUM5) uc_key_value = KEY_NUM5;
//			else if(ir_data_buf[2] == IR_NUM6) uc_key_value = KEY_NUM6;
//			else if(ir_data_buf[2] == IR_NUM7) uc_key_value = KEY_NUM7;
//			else if(ir_data_buf[2] == IR_NUM8) uc_key_value = KEY_NUM8;
//			else if(ir_data_buf[2] == IR_NUM9) uc_key_value = KEY_NUM9;

//			else if(ir_data_buf[2] == IR_POWER) uc_key_value = KEY_PLAY_L;

		}
		else uc_key_value = 0;
	}

}

void Set_Key_Sound(ui16 sound_idx)
{
	if(sound_idx>ui_rsv_mp3_total_num) return ;

	uc_mp3_status_bk = uc_mp3_status;
 	//if(uc_mp3_status != STATE_STOP)
		Ap_SetMP3_Stop();

//	ui_rsv_mp3_idx_bak = ui_rsv_mp3_idx;	//save idx for return

	ui_rsv_mp3_idx = sound_idx;
	b_rsv_mp3_flg = 1;	
	if(Ap_Init_MP3_Info()==1)
	{
	 	Ap_SetMP3_Play();
	}
	else if(Ap_Init_MP3_Info()==1)
	{
	 	Ap_SetMP3_Play();
	}

}
/*******************************************************************************
Function: 1.Ui_PlayS_InMusic()
		  2.Ui_PlayS_InBrowser()
Description: function of short press key play
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_PlayS_InMusic(void)
{
	b_status_flg = 1;
	if(uc_mp3_replay_ab_flg == 0)
	{
		switch(uc_mp3_status)
		{
			case STATE_PLAY:
				Ap_SetMP3_Pause();
				break;

			case STATE_PAUSE:
				Ap_SetMP3_Continue();
				break;

			default:
				if(Ap_Init_MP3_Info() == 1) 
				Ap_SetMP3_Play();
				break;
		}
	}
	else
	{
		uc_mp3_replay_ab_flg = 0;	//cancel A-B repeat
	}
	uc_lcd_dis_flg |= 0x98;			//"display PLAY State", clr A-B pic
}


/*******************************************************************************
Function: Ui_PlayS_InBrowser
Description: if the selected item is file, and play the file
Calls:
Global:ui_browser_sel_item_idx, uc_browser_sel_item_type, b_mp3_save_flg
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_PlayS_InBrowser(void)			//play the current file
{
	if(uc_fb_sel_item_type == 1)		//uc_fs_item_type: 1-file, 2-folder
	{
		Ap_Play_SelFile_InFileBrowser();//play this selected mp3 file

		Ui_ReturnTo_Music();			//return to music window
	}
}

/*******************************************************************************
Function: 1.Ui_NextS_InMusic()
Description: function of press key next
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_NextS_InMusic(void)
{
	uc_dynamicload_idx = 41;			//sel next file
	uc_mp3_status_bk = uc_mp3_status;	//save the current state
	Ap_SetMP3_Stop();
	Ui_Sel_NextLast_File();
/*	switch(uc_mp3_replay_ab_flg)
	{
		case 0:
			uc_dynamicload_idx = 41;			//sel next file
			uc_mp3_status_bk = uc_mp3_status;	//save the current state
			Ap_SetMP3_Stop();
			Ui_Sel_NextLast_File();
			break;

		case 1:
			ul_mp3_replay_a_byte = ul_fs_byte_offset;//save the current bytes A 
			ui_mp3_replay_a_time = ui_mp3_cur_time;	//save the current time A
			uc_mp3_replay_ab_flg = 2;				//set replay flag
			uc_lcd_dis_flg |= 0x10;					//display replay A-B
			break;

		case 2:
			ui_mp3_replay_b_time = ui_mp3_cur_time;	//save the current time B
			uc_mp3_replay_ab_flg = 3;				//set replay flag
			ui_sys_event |= EVT_MUSIC_AB_REPLAY;	//set the A-B replay event
			uc_lcd_dis_flg |= 0x10;					//display replay A-B
		
			uc_mp3_replay_time_cnt = 0;
			uc_mp3_replay_gap_cnt = 0;
			b_mp3_replay_start = 1;					//first replay
			break;
	}*/
}

/*******************************************************************************
Function: 1.Ui_LastS_InMusic()
Description: function of press key last
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_LastS_InMusic(void)
{
	if(uc_mp3_replay_ab_flg == 0)				//normal play
	{
		uc_mp3_status_bk = uc_mp3_status;		//save the current state
		Ap_SetMP3_Stop();
		uc_dynamicload_idx = 42;				//last file
		Ui_Sel_NextLast_File();
	}
	else
	{
		uc_mp3_replay_ab_flg = 1;				//wait to set A-
		uc_lcd_dis_flg |= 0x10;
	}
}

/*******************************************************************************
Description: press key next for a long time
********************************************************************************/
/*void Ui_NextL_InMusic(void)
{
	if(uc_mp3_status == STATE_PLAY)
	{
		if(uc_mp3_replay_ab_flg == 0)			//not support speed in A-B mode
		{
			Ap_SetMP3_Stop();

			uc_mp3_status = STATE_FORWARD;
			uc_lcd_dis_flg |= 0x50;				//display play state & lrc
		}
	}
	else
	{
		Ui_NextS_InMusic();
	}
}*/

/*******************************************************************************
Description:press key last for a long time
********************************************************************************/
/*void Ui_LastL_InMusic(void)
{
	if(uc_mp3_status == STATE_PLAY)
	{
		if(uc_mp3_replay_ab_flg == 0)			//not support speed in A-B mode
		{
			Ap_SetMP3_Stop();
			uc_mp3_status = STATE_BCKWARD;
			uc_lcd_dis_flg |= 0x50;				//display play state & lrc
		}
	}
	else
	{
		Ui_LastS_InMusic();
	}	
}*/

/*******************************************************************************
Description: continue press key next for a long time
********************************************************************************/
/*void Ui_NextLc_InMusic(void)
{
	if((uc_mp3_status==STATE_PAUSE) || (uc_mp3_status==STATE_STOP))
	{
		Ui_NextS_InMusic();						//sel the next music file
	}
 	else
	{
		uc_lcd_dis_flg |= 0x40;
	}
}*/

/*******************************************************************************
Description: continue press key last for a long time
********************************************************************************/
/*void Ui_LastLc_InMusic(void)
{
	if((uc_mp3_status == STATE_PAUSE) || (uc_mp3_status == STATE_STOP))
	{
		Ui_LastS_InMusic();
	}
	else
	{
		uc_lcd_dis_flg |= 0x40;
	}
}*/

/*******************************************************************************
Description:Select the next menu (uc_menu_cur_idx+1)
input:uc_menu_cur_idx,uc_menu_total_num
*******************************************************************************/
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
//	Ap_Check_EQ_Window();

	uc_lcd_dis_flg |= 0x58;						//set the bit4	
}

/*******************************************************************************
Select the last menu (uc_menu_cur_idx-1)
input:uc_menu_cur_idx,uc_menu_total_num
*******************************************************************************/
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
//	Ap_Check_EQ_Window();

	uc_lcd_dis_flg |= 0x58;						//set the bit4	
}

/*******************************************************************************
Description: select the next item
********************************************************************************/
void Ui_SelNext_InBrowser(void)
{
	uc_dynamicload_idx = 51;
	Api_DynamicLoad_Music_Target(CODE_MUSIC_B_SUBFUNC_IDX);//get next item
}

/*******************************************************************************
Description: select the last item
********************************************************************************/
void Ui_SelPrevious_InBrowser(void)
{
	uc_dynamicload_idx = 52;
	Api_DynamicLoad_Music_Target(CODE_MUSIC_B_SUBFUNC_IDX);//get previous item
}

/*******************************************************************************
Function: 1.Ui_StopWard()
Description: stop forward or backword
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_StopWard(void)
{
	if((uc_mp3_status==STATE_FORWARD) || (uc_mp3_status==STATE_BCKWARD))
	{
		Ap_SetStopWard();
		uc_lcd_dis_flg |= 0x10;								//display state
	}
}

/*******************************************************************************
Description: select the next or previous file
********************************************************************************/
void Ui_Sel_NextLast_File(void)
{
	Api_DynamicLoad_Music_Target(CODE_MUSIC_7_SELFILE_IDX);//sel next or last file

	if(Ap_Init_MP3_Info() == 1)			//init play viarable
	{
		if(b_mp3_get_next_file == 1)
		{
			if(uc_mp3_status_bk == STATE_PLAY)
			{
			 	Ap_SetMP3_Play();
			}
			else if(uc_mp3_status_bk == STATE_PAUSE)
			{
				Ap_SetMP3_Play();		//start play
				Ap_SetMP3_Pause();		//pause play
			}
		}	
	}
	uc_window_no = WINDOW_MUSIC;
	Ui_SetLcdFlag_0xfd();					
}

/*******************************************************************************
Function: 1.Ui_Enter_VolumeSet()
Description: 
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/ 
void Ui_Enter_VolumeSet(void)
{
	uc_window_no = WINDOW_MUSIC_VOLUME;
	Ui_SetLcdFlag_0xfd();
}

/*******************************************************************************
Function: 1.Ui_ModeS_In_PlayMenu()
		  2.Ui_ModeS_In_playMenu()
		  3.Ui_ModeS_In_EQMenu()
		  4.Ui_ModeS_In_RepeatMenu()
		  5.Ui_ModeS_In_RplTimeMenu()
		  6.Ui_ModeS_In_RplGapMenu()
		  7.Ui_ModeS_In_MenuDel()
Description: function of press key last
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_ModeS_InTopMenu(void)
{
	uc_save_mode_idx = uc_menu_cur_idx;
	switch(uc_menu_cur_idx)
	{
		case 1:
			uc_select_idx = 1;
			Ui_Select_Book(1);
			break;
		case 2:
			uc_select_idx = 2;
			Ui_Select_Book(2);
			break;
		case 3:
			uc_select_idx = 3;
			Ui_Select_Book(3);
			break;
		case 4:

			uc_window_no = WINDOW_MUSIC_PLAY_MENU;
			uc_menu_total_num = MUSIC_PLAY_MENU_NUM;
			uc_menu_cur_idx = 1;
			Ui_Return_Menu_Setting();
			break;
		case 5:
			uc_select_idx = 4;
			Ui_Select_Book(4);
			break;
	}
	
}
void Ui_ModeS_InMusic(void)
{
		Ap_SetMP3_Stop();
		Ap_Save_Music_Parameter();
		uc_window_no = WINDOW_MUSIC_MENU_BROWSER;
//		uc_menu_title_idx = TITLE_STR_FLASH_LIST_IDX;
		Ap_Init_ItemIdx();

		Ui_SetLcdFlag_0xfd(); 

}
void Ui_ModeL_InMusic(void)
{
//	if(uc_disktype_flg == 0x81)
	{
		uc_window_no = WINDOW_MUSIC_TOP_MENU;
		uc_menu_total_num = 5;
		uc_menu_cur_idx = uc_save_mode_idx;	//1;uc_save_mode_idx
		Ui_Return_Menu_Setting();
	}
//	else Ui_ModeS_InTopMenu();
/*	uc_mp3_replay_ab_flg = 0;							//clr A-B replay flag

	if(uc_mp3_status == STATE_PLAY)
	{
		uc_window_no = WINDOW_MUSIC_PLAY_MENU;
		uc_menu_total_num = MUSIC_PLAY_MENU_NUM;
		uc_menu_cur_idx = 1;
	}
	else
	{	
	    Ap_SetMP3_Stop();			
		Ap_Save_Music_Parameter();	//	allen	//save current mp3 file cluster for return to music when press key vol

		Ap_Check_SDExist();
		if(b_sd_exist_flg == 0)
		{
			uc_window_no = WINDOW_MUSIC_MENU_FLASH;
			uc_menu_cur_idx = 1;
			uc_menu_total_num = 4;//Music_MenuFlash_Num;
		}
		else
		{
			uc_window_no = WINDOW_MUSIC_MENU_SD;			
			if(b_sd_flash_flg == 1)
			{
				uc_menu_cur_idx = 1;							//init menu index and total item
			}
			else
			{
				uc_menu_cur_idx = 2;	
			}
			uc_menu_total_num =5;// Music_MenuSD_Num;	
		}

	}	
	Ui_Return_Menu_Setting();*/
}

/*******************************************************************************
Description: confirm menu function selected
********************************************************************************/
void Ui_ModeS_In_PlayMenu(void)
{
 	uc_menu_cur_idx_bk = uc_menu_cur_idx;			//backup current menu idx

	switch(uc_menu_cur_idx)
	{
		case 1:									//select disk
			uc_window_no = WINDOW_MUSIC_MENU_EQ;
			uc_menu_cur_idx = uc_mp3_eq_mode + 1;			//init menu index and total item
			uc_menu_total_num = MUSIC_EQ_MENU_NUM;
			break;

		case 2:									//repeat
			uc_window_no = WINDOW_MUSIC_MENU_REPEAT;
			uc_menu_cur_idx = uc_mp3_repeat_mode + 1;			//init menu index and total item
			uc_menu_total_num = MUSIC_REPEAT_MENU_NUM;
			break;

		case 3:									//timer setup
			uc_window_no = WINDOW_MUSIC_MENU_RTC_TIME;
			uc_menu_cur_idx = 1;			//init menu index and total item
			uc_menu_total_num = 3;//4;
			break;

		case 4:									// backlight and cantrast
			uc_window_no = WINDOW_SYS_MENU;
			uc_menu_cur_idx = 1;			//init menu index and total item
			uc_menu_total_num = 3;
			break;		
			
		default: //exit
			//uc_window_no = WINDOW_MUSIC;
			uc_window_no = WINDOW_MUSIC_TOP_MENU;
			uc_menu_total_num = 5;
			uc_menu_cur_idx = 4;
			break;	
	}	
	Ui_Return_Menu_Setting();
}
 /*******************************************************************************
Description: confirm menu function selected in stop menu
*******************************************************************************/
/*void Ui_ModeS_In_MenuFlash(void)
{
	uc_menu_cur_idx_bk = uc_menu_cur_idx;		//backup current menu idx
	switch(uc_menu_cur_idx)
	{
		case 1:
			uc_window_no = WINDOW_MUSIC_MENU_BROWSER;
//			uc_menu_title_idx = TITLE_STR_FLASH_LIST_IDX;
			Ap_Init_ItemIdx();
			Ui_SetLcdFlag_0xff(); 
			break;
																          
		case 2:
			uc_window_no = WINDOW_MUSIC_MENU_DELALL;
			uc_menu_cur_idx = 2;
			uc_menu_total_num = 2;
			uc_lcd_dis_flg = 0xfc;

			if(uc_fs_subdir_stk_pt == 0)
			{
				b_dir_root_flg = 0;		//root
			}
			else
			{
				b_dir_root_flg = 1;		//subdir
			} 	

			uc_dynamicload_idx = 12;	//get browser name selected currently
			Api_DynamicLoad_Music_Target(CODE_MUSIC_5_GET_ITEM_NUM_IDX);
			uc_dynamicload_idx = 4;
			Api_DynamicLoad_Music_Target(CODE_MUSIC_6_GET_ITEM_NAME_IDX);
			break;

		default:
			Ui_ReturnTo_MusicPause();			
			break;
	}	
}
  */
 /*******************************************************************************
Description: confirm menu function selected in stop menu
*******************************************************************************/
/*void Ui_ModeS_In_MenuSD(void)
{
	uc_menu_cur_idx_bk = uc_menu_cur_idx;		//backup current menu idx	
	switch(uc_menu_cur_idx)
	{
		case 1:	
		    b_mp3_restore_file_flg = 1;		
			uc_dynamicload_idx = 54;
			Api_DynamicLoad_Music_Target(CODE_MUSIC_B_SUBFUNC_IDX);//get next item
			if(b_sd_flash_flg == 0)
			{
				b_sd_flash_flg = 1;
				Ap_Save_Music_Parameter();
				uc_dynamicload_idx = 30;		//FS_Finit
				Api_DynamicLoad_Music_Target(CODE_MUSIC_C_SWITCH_CARD_IDX);
				if(uc_msi_status != 0)
				{
					Ui_Display_DrawSD();
					Ui_ReturnTo_MenuSD();
					return;	
				}
				goto Label_EnterRoot;
			}
			Ui_ReturnTo_Browser();	 //allen		
			break;

		case 2:	
		    b_mp3_restore_file_flg = 1;		
			uc_dynamicload_idx = 54;
			Api_DynamicLoad_Music_Target(CODE_MUSIC_B_SUBFUNC_IDX);//get next item
			if(b_sd_flash_flg == 1)
			{
				b_sd_flash_flg = 0;
				Ap_Save_Music_Parameter();
				uc_dynamicload_idx = 30;		//FS_Finit
				Api_DynamicLoad_Music_Target(CODE_MUSIC_C_SWITCH_CARD_IDX);
Label_EnterRoot:
				uc_dynamicload_idx = 41;		//Scan All Disk(Flash or SD)      //allen
				Api_DynamicLoad_Music_Target(CODE_MUSIC_1_INIT_IDX);
				uc_dynamicload_idx = 32;		//enter root dir for display browser
				Api_DynamicLoad_Music_Target(CODE_MUSIC_C_SWITCH_CARD_IDX);
			}
			Ui_ReturnTo_Browser();		   //allen
			break;
		 
		 
		   case 3:		//delete one music file
			b_mp3_restore_file_flg = 1;
			if(b_fb_nofile_flg == 0)
			{
			uc_window_no = WINDOW_MUSIC_MENU_DEL;
			uc_menu_cur_idx = 2;
			uc_menu_total_num = 2;
			b_lcd_clrall_flg = 1;
			Ui_SetLcdFlag_0xfd();
			}
	     	else
	      	{
			Ui_Display_NoFile();
			uc_menu_cur_idx_bk = 3;
			Ui_ReturnTo_MenuSD();	
		    }
		  
			break;

	       case 4:
			//Ap_SetMP3_Stop();
				
			//mMPEG_Enter_Rest;
			//Ap_Save_Music_Parameter();				//save music para
			//Ap_Mpeg_ExitReset();
		b_mp3_restore_file_flg = 1;	
		if(b_fb_nofile_flg == 0)
		{
	
			if(uc_fs_subdir_stk_pt == 0)
			{
				b_fs_subdir_stk_pt = 1;
			}
			else
			{
				b_fs_subdir_stk_pt = 0;
			}
			uc_dynamicload_idx = 4;
			Api_DynamicLoad_Music_Target(CODE_MUSIC_6_GET_ITEM_NAME_IDX);
			uc_window_no = WINDOW_MUSIC_MENU_DELALL;
			uc_menu_cur_idx = 2;
			uc_menu_total_num = 2;
			b_lcd_clrall_flg = 1;
			Ui_SetLcdFlag_0xfd();
		}
		else
		{
				Ui_Display_NoFile();
				uc_menu_cur_idx_bk = 3;
				Ui_ReturnTo_MenuSD();	
		}
		    break;

		case 5:
			if(b_fb_nofile_flg == 0)
			{
			//	Ui_ReturnTo_MusicPause();
			    Ui_ReturnTo_Music();	
			}
			else
			{
				Ap_SwitchTo_MainMenu();	
			}			
			break;

		default:
			break;
	}
	Ui_SetLcdFlag_0xfd();
  	
   }*/
/*******************************************************************************
Description: select EQ
********************************************************************************/
/*void Ui_ModeS_In_EQMenu(void)
{
	uc_mp3_eq_mode = uc_menu_cur_idx - 1;
	Ui_ReturnTo_Music();	
}*/

void Ui_ModeS_In_DiskMenu(void)
{
	if(uc_menu_cur_idx<4)
	{
		if((b_sd_exist_flg == 0)&&(uc_menu_cur_idx == 3)&&((MR_MSCON4 & 0x80) == 0))
		{
			
			Ui_ReturnTo_Music();	//no tf card
			return ;
		}
		if(uc_mp3_status != STATE_STOP)
	 		Ap_SetMP3_Stop();
		Ap_Switch_Disk_Process(uc_menu_cur_idx);
		if(b_fb_nofile_flg == 0)
		{
			
			if(Ap_Init_MP3_Info() == 1) 	//init info of current mp3(get id3&vbr info, process lrc file, get mp3 filename...)
			{
				Ap_SetMP3_Play();		  //if file is ok, and start play
			}
	     }
		 Ui_ReturnTo_Music(); 	
	}
	else
	{
		uc_menu_cur_idx = 1;
		uc_window_no = WINDOW_MUSIC_PLAY_MENU;
		uc_menu_total_num = MUSIC_PLAY_MENU_NUM;
		Ui_Return_Menu_Setting();	
	}
		
}

/*******************************************************************************
Description: select repeat mode
********************************************************************************/
void Ui_ModeS_In_RepeatMenu(void)
{
	/*if(uc_mp3_repeat_mode == REPEAT_RANDOM)
	{
		if(ui_mp3_idx_indir > ui_fs_special_file_num_indir)
		{
			ui_mp3_idx_indir = 1;
		}
	}*/
	if(uc_menu_cur_idx<3)
	{
		uc_mp3_repeat_mode = uc_menu_cur_idx - 1;
		Ui_ReturnTo_Music();
	}
	else
	{
	 	uc_menu_cur_idx = 2;
		uc_window_no = WINDOW_MUSIC_PLAY_MENU;
		uc_menu_total_num = MUSIC_PLAY_MENU_NUM;
		Ui_Return_Menu_Setting();
	}	
}

/*******************************************************************************
Description: rtc time 
********************************************************************************/
void Ui_ModeS_In_RtcTimeMenu(void)
{
	uc_menu_cur_idx_bk = uc_menu_cur_idx;
	switch(uc_menu_cur_idx)
	{
	 	case 1:	//power off setup
			uc_window_no = WINDOW_SYS_POWEROFF;
			uc_menu_cur_idx = 1;
			uc_menu_total_num = 6;
			uc_lcd_dis_flg = 0xFF;
			break;
		/*case 2:	//rtc time adjust
			Ui_Read_Rtc_Time();
			uc_window_no = WINDOW_SYS_RTCADJ;
			uc_menu_cur_idx = 1;
			uc_lcd_dis_flg = 0xFF;
			break;*/
		case 2:// lock setup
			uc_window_no = WINDOW_SYS_LOCKKEY;
			uc_menu_cur_idx = 1;
			uc_menu_total_num = 6;
			uc_lcd_dis_flg = 0xFF;
			break;

		default:
			//uc_window_no = WINDOW_MUSIC;
			uc_menu_cur_idx = 3;
			uc_window_no = WINDOW_MUSIC_PLAY_MENU;
			uc_menu_total_num = MUSIC_PLAY_MENU_NUM;
			break;
	}
	Ui_Return_Menu_Setting();		
}
/*******************************************************************************
Description: select sys menu mode
********************************************************************************/
void Ui_ModeS_In_SysMenu(void)
{
	uc_menu_cur_idx_bk = uc_menu_cur_idx;		//backup current menu idx	
	switch(uc_menu_cur_idx)
	{
		case 1:
			uc_window_no = WINDOW_SYS_MENU_BKLIGHT_TIME;
			uc_menu_cur_idx = uc_bklight_time_level + 1; 
			uc_menu_total_num = 0x08;
			break;
		case 2:
			uc_window_no = WINDOW_SYS_MENU_CONTRAST;
			uc_menu_cur_idx = uc_bklight_grade;			//max contrast level is 16
			uc_menu_total_num = 16;
			break;
		default:
			//Ui_ReturnTo_Music();
			uc_menu_cur_idx = 4;
			uc_window_no = WINDOW_MUSIC_PLAY_MENU;
			uc_menu_total_num = MUSIC_PLAY_MENU_NUM;
			break;
	}
	Ui_Return_Menu_Setting();		
}


/*******************************************************************************
set volume inc
*******************************************************************************/
void Ui_Set_VolInc(void)
{
	if(uc_mp3_volume_level < VOLUME_LEVEL_MAX)	//volume level from 0 to 31
	{
		uc_mp3_volume_level++;
	}
	else
	{
		uc_mp3_volume_level = VOLUME_LEVEL_MAX;
	}
	Ui_Vol_Common_Fun();
}

/*******************************************************************************
set volume dec
*******************************************************************************/
void Ui_Set_VolDec(void)
{
	if(uc_mp3_volume_level > 0)				//volume level from 0 to 31
	{
		uc_mp3_volume_level--;
	}
	else
	{
		uc_mp3_volume_level = 0;
	}
	Ui_Vol_Common_Fun();
}

/*******************************************************************************
set volume func
*******************************************************************************/
void Ui_Vol_Common_Fun(void)
{
	Api_Set_Volume(uc_mp3_volume_level);			//setting volume value
	uc_lcd_dis_flg |= 0x10;
}

/*******************************************************************************
Function: 1.Ui_Sel_Last_In_Contrast()
		  2.Ui_Sel_Previous_Menu()
Description: function of press key last
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Sel_Last_In_Contrast(void)
{
	if(uc_menu_cur_idx > 1)						// max contrast level is 16
	{
		uc_menu_cur_idx--;
	}
	else
	{
		uc_menu_cur_idx = 1;
	}
	Api_LcdDriver_Set_BklightLevel(uc_menu_cur_idx);
	uc_lcd_dis_flg |= 0x10;						//set the bit4 for dis string
}

/*******************************************************************************
Function: 1.Ui_Sel_Next_In_Contrast()	
		  2.Ui_Sel_Next_In_LightColor()
		  3.Ui_Sel_Next_Menu()
Description: function of press key next
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Sel_Next_In_Contrast(void)
{
	if(uc_menu_cur_idx < 16)
	{
		uc_menu_cur_idx++;
	}
	else
	{
		uc_menu_cur_idx = 16 ;		  		// max contrast level is 16,1~16
	}
	Api_LcdDriver_Set_BklightLevel(uc_menu_cur_idx);
	uc_lcd_dis_flg |= 0x10;						//set the bit4 for dis string
}

/********************************************************************************
key mode in system contrast
********************************************************************************/
void Ui_ModeS_In_SysMenu_Contrast(void)
{
	uc_bklight_grade = uc_menu_cur_idx;
	
	Ui_ReturnTo_Music();
}

/********************************************************************************
key mode in back light time
********************************************************************************/
void Ui_ModeS_In_sysMenu_BkLight_Time(void)
{
	if(uc_menu_cur_idx<8)
	{
		uc_bklight_time_level = uc_menu_cur_idx - 1;
		Ui_PowerOn_Backlight();						//update bklight time
		Ui_ReturnTo_Music();
	}
	else
	{
		uc_menu_cur_idx = 1;			//init menu index and total item
	 	uc_window_no = WINDOW_SYS_MENU;		
		uc_menu_total_num = 3;
		Ui_Return_Menu_Setting();
	}
}
void Ui_ModeS_In_Powerff(void)
{
 /*	uc_menu_cur_idx++;
	if(uc_menu_cur_idx == 2)
	{
		if(uc_sleep_poweroff_level == 0)
		{
			Ui_ReturnTo_Music();
		}
		else
		{
	 		rtc_read();
			uc_rtc_hour_off = uc_rtc_hour;
			uc_rtc_minute_off = uc_rtc_minute;
		}
	}

	if(uc_menu_cur_idx==4)
	{//setup finish
		Ui_ReturnTo_Music();
	}*/
	uc_sleep_poweroff_level = 1;
	switch(uc_menu_cur_idx)
	{
	 	case 1:
			ui_sleep_poweroff_time = 9000; //15 Minute 10*60*15
			break;
		case 2:
			ui_sleep_poweroff_time = 18000; //30 Minute 10*60*30
			break;
		case 3:
			ui_sleep_poweroff_time = 27000; //45 Minute 10*60*45
			break;
		case 4:
			ui_sleep_poweroff_time = 36000; //60 Minute 10*60*60
			break;
		case 5:
			ui_sleep_poweroff_time = 54000; //90 Minute 10*60*90
			break;
		default:	// return
			uc_sleep_poweroff_level = 0;
			uc_menu_cur_idx = 1;
			uc_window_no = WINDOW_MUSIC_MENU_RTC_TIME;
			uc_menu_total_num = 3;
			Ui_Return_Menu_Setting();
			return ;
			break;
	}
	Ui_ReturnTo_Music();
}

void Ui_ModeS_InLockKey(void)
{
	

	switch(uc_menu_cur_idx)
	{
		case 1:
		case 2:
		case 3:
		case 4:
			uc_lock_level = uc_menu_cur_idx;
			ui_lock_time = ui_lock_time_table[uc_lock_level];
			Ui_ReturnTo_Music();
			break;
		case 5:
			uc_lock_level = 0;
			ui_lock_time = 0;
			Ui_ReturnTo_Music();
			break;
		default:
			//uc_lock_level = 0;
			uc_menu_cur_idx = 2;
			uc_window_no = WINDOW_MUSIC_MENU_RTC_TIME;
			uc_menu_total_num = 3;
			Ui_Return_Menu_Setting();
			break;
	}
		
}
/*void Ui_Set_Poweroff(void)
{
	if(uc_sleep_poweroff_level == 0) uc_sleep_poweroff_level = 1;
	else uc_sleep_poweroff_level = 0;

	uc_lcd_dis_flg |= 0x02;
}*/

// Adjust Hour
/*void Poweroff_AdjHour(uc8 cnt)
{
	uc8 tmph;
	tmph=BcdToHex(uc_rtc_hour_off);
	tmph+=cnt;
	if(tmph>23)
	{
		if(cnt==0x01)
		tmph=0x00;
		else
		tmph=23;
	}
	uc_rtc_hour_off=HexToBcd(tmph);
	
}*/

// Adjust Minute
/*void Poweroff_AdjMinute(uc8 cnt)
{
	uc8 tmpm;
	tmpm=BcdToHex(uc_rtc_minute_off);
	tmpm+=cnt;
	if(tmpm>59)
	{
		if(cnt==0x01)
			tmpm=0x00;
		else
			tmpm=59;
	}
	uc_rtc_minute_off=HexToBcd(tmpm);
	
}*/


/*void Ui_Poweroff_KeyProc(uc8 cnt)
{
	switch(uc_menu_cur_idx)
 	{
		case 1:
			Ui_Set_Poweroff();
			uc_lcd_dis_flg |= 0x02;
			break;
		case 2:
			Poweroff_AdjHour(cnt);
			uc_lcd_dis_flg |= 0x80;
			break;
		case 3:
			Poweroff_AdjMinute(cnt);
			uc_lcd_dis_flg |= 0x80;
			break;
	}

}*/



#endif