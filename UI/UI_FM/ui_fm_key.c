/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_fm_key.c
Creator: zj.zong					Date: 2009-06-26
Description: process key event
Others:
Version: V0.1
History:
	V0.1	2009-06-26		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_FM_KEY_PROCESS

#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include ".\UI\UI_FM\ui_fm_header.h"
#include ".\UI\UI_FM\ui_fm_var_ext.h"
#include ".\DRV\DRV_FM\drv_fm.h"

extern void Ui_Display_FM_NO_Channel(void);
extern void Ui_SetLcdFlag_0xfd(void);
extern void	Ui_PowerOn_Backlight(void);
extern void Ui_ReturnTo_FM(void);
extern void Ui_ReturnTo_FM_Menu(void);
extern void Ui_Init_TopIdx(void);

extern void Ap_PowerOff_MP3(void);
extern void Ap_FM_Load_CurFrequency(void);
extern void Ap_FM_Save_CurFrequency(void);
extern void Ap_FM_Manual_IncFreq(void);
extern void Ap_FM_Manual_DecFreq(void);
extern void Ap_FM_Del_CurChannel(void);
extern void Ap_FM_Del_AllChannel(void);
extern void Ap_Change_CurBand(void);
extern void Ap_SwitchTo_MainMenu(void);
extern void Ap_FM_Stop_AutoSearch(void);
extern void Ap_Exit_FM_Mode(void);

extern void Api_Key_Mapping(void);
extern void Api_Set_FM_Volume(uc8 volume_level);
extern void Api_DynamicLoad_CodeMpm1_8kExecute(void);
extern void Api_DynamicLoad_CodeCpm(void);

void Ui_PlayS_InFM(void);
void Ui_NextS_InFM(void);
void Ui_NextL_InFM(void);
void Ui_SelNext_CH_In_MenuDel(void);
void Ui_SelNext_Menu(void);
void Ui_LastS_InFM(void);
void Ui_LastL_InFM(void);
void Ui_SelPrevious_CH_In_MenuDel(void);
void Ui_SelPrevious_Menu(void);
void Ui_EnterVolume(void);
void Ui_ModeS_InFM(void);
void Ui_ModeS_InFM_Menu(void);
void Ui_ModeS_In_DeleteMenu(void);
void Ui_ModeS_In_MenuDeleteAll(void);
void Ui_Set_VolInc(void);
void Ui_Set_VolDec(void);
void Ui_Vol_Common_Fun(void);
void Ui_ModeS_In_Menu_Signal(void);

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
	uc_menu_exit_cnt = 0;						//clr the counter

	Api_Key_Mapping();

/*	if(b_key_hold_val == 0)						//not hold state
	{
		if(	uc_hold_cnt != 0)
		{
			uc_hold_cnt = 0;
		 	Ui_SetLcdFlag_0xfd();
		}  */
		if((ui_bklight_time == 0) && (uc_bklight_time_level != 0))
		{
			Ui_PowerOn_Backlight();
			return;
		}
		Ui_PowerOn_Backlight();

		switch(uc_key_value)
		{
			case KEY_PLAY_S:
				switch(uc_window_no)
				{
					case WINDOW_FM:
						Ui_PlayS_InFM();
						break;
					
					default:
						Ui_ReturnTo_FM();	
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
					case WINDOW_FM:
						Ui_NextS_InFM();
						break;

					case WINDOW_FM_VOLUME:
						Ui_Set_VolInc();
						break;

					case WINDOW_FM_MENU_DEL:
					  	Ui_SelNext_CH_In_MenuDel();
						break;

					default:
						Ui_SelNext_Menu();	 //WINDOW_FM_MENU, WINDOW_FM_MENU_DELALL
						break;
				}
				break;

			case KEY_NEXT_L:
				switch(uc_window_no)
				{
					case WINDOW_FM:
						Ui_NextL_InFM();
						break;

					case WINDOW_FM_VOLUME:
						Ui_Set_VolInc();
						break;

					case WINDOW_FM_MENU_DEL:
					  	Ui_SelNext_CH_In_MenuDel();
						break;

					default:
						Ui_SelNext_Menu();	 //WINDOW_FM_MENU, WINDOW_FM_MENU_DELALL
						break;
				}
				break;

			case KEY_NEXT_LC:
				switch(uc_window_no)
				{
					case WINDOW_FM_VOLUME:
						Ui_Set_VolInc();
						break;

					case WINDOW_FM_MENU_DEL:
					  	Ui_SelNext_CH_In_MenuDel();
						break;

					default:
						Ui_SelNext_Menu();	 //WINDOW_FM_MENU, WINDOW_FM_MENU_DELALL
						break;
				}
				break;

			case KEY_NEXT_LE:
			case KEY_LAST_LE:
				break;

			case KEY_LAST_S:
				 switch(uc_window_no)
				{
					case WINDOW_FM:
						Ui_LastS_InFM();
						break;

					case WINDOW_FM_VOLUME:
						Ui_Set_VolDec();
						break;

					case WINDOW_FM_MENU_DEL:
					  	Ui_SelPrevious_CH_In_MenuDel();
						break;

					default:
						Ui_SelPrevious_Menu();	 //WINDOW_FM_MENU, WINDOW_FM_MENU_DELALL
						break;
				}			
				break;
			
			case KEY_LAST_L:
				switch(uc_window_no)
				{
					case WINDOW_FM:
						Ui_LastL_InFM();
						break;

					case WINDOW_FM_VOLUME:
						Ui_Set_VolDec();
						break;

					case WINDOW_FM_MENU_DEL:
					  	Ui_SelPrevious_CH_In_MenuDel();
						break;

					default:
						Ui_SelPrevious_Menu();	 //WINDOW_FM_MENU, WINDOW_FM_MENU_DELALL
						break;
				}
				break;
			
			case KEY_LAST_LC:
				switch(uc_window_no)
				{
					case WINDOW_FM_VOLUME:
						Ui_Set_VolDec();
						break;

					case WINDOW_FM_MENU_DEL:
					  	Ui_SelPrevious_CH_In_MenuDel();
						break;

					default:
						Ui_SelPrevious_Menu();	 //WINDOW_FM_MENU, WINDOW_FM_MENU_DELALL
						break;
				}
				break;
			
			case KEY_VOL_S:
				switch(uc_window_no)
				{
					case WINDOW_FM:
						Ui_EnterVolume();
						break;

					case WINDOW_FM_VOLUME:
						Ui_ReturnTo_FM();
						break;

					case WINDOW_FM_MENU:
						Ui_ReturnTo_FM();
						break;

					default:
						Ui_ReturnTo_FM_Menu();
						break;
				}
				break;

  			case KEY_REC_L:
				if(((uc_user_option2&0x20) != 0)&&((uc_user_option2&0x08) == 0))
				{
					Ap_Exit_FM_Mode();
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
				 	case WINDOW_FM:
						Ui_ModeS_InFM();
						break;

					case WINDOW_FM_VOLUME:
						Ui_ReturnTo_FM();
						break;

					case WINDOW_FM_MENU:
						Ui_ModeS_InFM_Menu();
						break;

					case WINDOW_FM_MENU_DEL:
						Ui_ModeS_In_DeleteMenu();
						break;

					case WINDOW_FM_MENU_DELALL:
						Ui_ModeS_In_MenuDeleteAll();
						break;

					case WINDOW_FM_SIGNAL:
						Ui_ModeS_In_Menu_Signal();
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
		Ui_PowerOn_Backlight();			//power on backlight
		uc_lcd_dis_flg |= 0x02;				//set bit1, display hold pic
	}	   */

	ui_sys_event |= EVT_DISPLAY;			//set the display event
}

/*******************************************************************************
Function: 1.Ui_PlayS_InFM()
		
Description: function of short press key play
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_PlayS_InFM(void)
{
	if(uc_fm_mode == FM_MODE_PRESET)
	{
		if(uc_fm_ch_total_num != 0)
		{
			uc_fm_cur_ch_idx++;
			Ap_FM_Load_CurFrequency();	//load freq from preset buffer
		}
	}
	else if(uc_fm_mode == FM_MODE_TUNING)
	{
		if(uc_fm_ch_total_num != 0)
		{
			uc_fm_mode = FM_MODE_PRESET;
			uc_fm_cur_ch_idx = 1;
			Ap_FM_Load_CurFrequency();	//load freq from preset buffer
		}
	}
	else										//autosearch or halfsearch
	{
		ui_sys_event &= ~EVT_FM_SEARCH;
		Ap_FM_Stop_AutoSearch();				//stop autosearch or halfsearch
	}
	
	uc_lcd_dis_flg |= 0x40;						//display freq, freq bar, ch index
}

/*******************************************************************************
Function: Ui_NextS_InFM, Ui_LastS_InFM()
Description: function of press key next
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_NextS_InFM(void)
{
	if(b_fm_search_flg == 0)
	{
		Ap_FM_Manual_IncFreq();

		uc_lcd_dis_flg |= 0x40;				//display freq, freq bar, ch index
	}
}


void Ui_LastS_InFM(void)
{
	if(b_fm_search_flg == 0)
	{
		Ap_FM_Manual_DecFreq();

		uc_lcd_dis_flg |= 0x40;				//display freq, freq bar, ch index
	}
}


/********************************************************************************
 next long in fm
********************************************************************************/
void Ui_NextL_InFM(void)
{
	if(b_fm_search_flg == 0)
	{
		uc_fm_mode = FM_MODE_HALFSEARCH_INC; //halfsearch inc
		b_fm_search_flg = 1;
		ui_sys_event |= EVT_FM_SEARCH;
	}
}

/********************************************************************************
key next in fm	delete
********************************************************************************/
void Ui_SelNext_CH_In_MenuDel(void)
{
	if(uc_fm_ch_total_num != 0)
	{
		uc_fm_menu_ch_idx++;
		if(uc_fm_menu_ch_idx > uc_fm_ch_total_num)
		{
			uc_fm_menu_ch_idx = 1;
		}
		ui_fm_menu_dis_freq = ui_fm_preset_buf[uc_fm_menu_ch_idx-1]&0x7fff;	//clr bit15

		uc_lcd_dis_flg |= 0x10;				//display delete ch index and freq
	}
}


/********************************************************************************
 key last long in fm delete
********************************************************************************/
void Ui_SelPrevious_CH_In_MenuDel(void)
{
	if(uc_fm_ch_total_num != 0)
	{
		uc_fm_menu_ch_idx--;
		if(uc_fm_menu_ch_idx == 0)
		{
			uc_fm_menu_ch_idx = uc_fm_ch_total_num;
		}
		
		ui_fm_menu_dis_freq = ui_fm_preset_buf[uc_fm_menu_ch_idx-1]&0x7fff;	//clr bit15

		uc_lcd_dis_flg |= 0x10;				//display delete ch index and freq
	}
}


/*******************************************************************************
Select the next menu (uc_menu_cur_idx+1)
********************************************************************************/
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
	uc_lcd_dis_flg |= 0x58;			//set the bit4
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
	uc_lcd_dis_flg |= 0x58;			//set the bit4
}


/********************************************************************************
 last long in fm
********************************************************************************/
void Ui_LastL_InFM(void)
{
	if(b_fm_search_flg == 0)
	{
		uc_fm_mode = FM_MODE_HALFSEARCH_DEC; //halfsearch dec
		b_fm_search_flg = 1;
		ui_sys_event |= EVT_FM_SEARCH;
	}
}


/*******************************************************************************
Function: 1.Ui_EnterVolume()
		  2.Ui_Set_VolInc()
		  3.Ui_Set_VolDec()
		  4.Ui_Vol_Common_Fun()
Description: function of press key VOL dec and vol inc
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_EnterVolume(void)
{
	if(b_fm_search_flg == 0)
	{
		uc_window_no = WINDOW_FM_VOLUME;
		Ui_SetLcdFlag_0xfd(); 	//clr bit1 hold picture

	}
}


/********************************************************************************
set volume inc in fm
********************************************************************************/
void Ui_Set_VolInc(void)
{
	if(uc_fm_volume_level < VOLUME_LEVEL_MAX)//volume level from 0 to 31
	{
		uc_fm_volume_level++;
	}
	else
	{
		uc_fm_volume_level = VOLUME_LEVEL_MAX;
	}

	Ui_Vol_Common_Fun();
}


/********************************************************************************
set volume dec in fm
********************************************************************************/
void Ui_Set_VolDec(void)
{
	if(uc_fm_volume_level > 0)				//volume level from 0 to 31
	{
		uc_fm_volume_level--;
	}
	else
	{
		uc_fm_volume_level = 0;
	}

	Ui_Vol_Common_Fun();
}


/********************************************************************************
set volume in fm
********************************************************************************/
void Ui_Vol_Common_Fun(void)
{
	Api_Set_FM_Volume(uc_fm_volume_level);		//setting volume value
	uc_lcd_dis_flg |= 0x10;					//set bit5
}


/*******************************************************************************
Function: 1.Ui_ModeS_InFM()
		  2.Ui_ModeS_InFM_Menu()
		  3.Ui_ModeS_In_DeleteMenu()
		  4.Ui_ModeS_In_MenuDeleteAll()
Description: function of press key mode
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_ModeS_InFM(void)
{
	if(b_fm_search_flg == 0)
	{
		uc_menu_cur_idx = 1;
		uc_menu_cur_idx_bk = 1;
		uc_menu_total_num = FM_MENU_NUM_MAX;
	
		uc_menu_top_idx = 0;
		Ui_Init_TopIdx();
		uc_window_no = WINDOW_FM_MENU;
		Ui_SetLcdFlag_0xfd(); 	//clr bit1 hold picture
	}
}

/********************************************************************************
key mode in fm menu window
********************************************************************************/
void Ui_ModeS_InFM_Menu(void)
{
	uc_menu_cur_idx_bk = uc_menu_cur_idx;
	
	switch(uc_menu_cur_idx)
	{
		case 0x01:				//save current channel
			if(uc_fm_mode == FM_MODE_TUNING)
			{
				uc_fm_mode = FM_MODE_PRESET;
				Ap_FM_Save_CurFrequency();
			}
			Ui_ReturnTo_FM();
			break;
			
		case 0x02:				//autosearch
			b_fm_search_flg = 1;
			ui_sys_event |= EVT_FM_SEARCH;
			Ui_ReturnTo_FM();
			break;

		case 0x03:				//signal intensity
			uc_menu_cur_idx = uc_fm_signal_strength_val;
		   	uc_menu_total_num = 7;
			uc_window_no = WINDOW_FM_SIGNAL;
			Ui_SetLcdFlag_0xfd();
			break;
			
		case 0x04:			  	//enter window of delete channel
			if(uc_fm_ch_total_num != 0)
			{
				uc_fm_menu_ch_idx = uc_fm_cur_ch_idx;	//init fm menu channel index
				uc_fm_cur_ch_idx_bk = uc_fm_cur_ch_idx;	//backup fm current channel index
				ui_fm_menu_dis_freq = ui_fm_preset_buf[uc_fm_cur_ch_idx-1]&0x7fff;	//clr bit15
				uc_window_no = WINDOW_FM_MENU_DEL;
				Ui_SetLcdFlag_0xfd();
			}
			else				//have not preset channel
			{
				Ui_Display_FM_NO_Channel();
				Ui_ReturnTo_FM_Menu();
			}
			break;
			
		case 0x05:			   	//enter window of delete all channel
			if(uc_fm_ch_total_num != 0)
			{
				uc_menu_cur_idx = 2;
				uc_menu_total_num = 2;
				uc_window_no = WINDOW_FM_MENU_DELALL;
				Ui_SetLcdFlag_0xfd();
			}
			else
			{
				Ui_Display_FM_NO_Channel();
				Ui_ReturnTo_FM_Menu();
			}		
			break;
			
		case 0x06:			  	//select normal band
			if(uc_fm_band == FM_BAND_JAPAN)
			{
				uc_fm_band_bk = uc_fm_band;
				uc_fm_band = FM_BAND_NORMAL;
				Ap_Change_CurBand();	//change band
			}
			Ui_ReturnTo_FM();
			break;
			
		case 0x07:			  //select Japan band
			if(uc_fm_band == FM_BAND_NORMAL)
			{
				uc_fm_band_bk = uc_fm_band;
				uc_fm_band = FM_BAND_JAPAN;
				Ap_Change_CurBand();	//change band
			}
			Ui_ReturnTo_FM();
			break;
			
		default:		 	 //exit menu
			Ui_ReturnTo_FM();
			break;
	}
	uc_menu_top_idx = 0;
	Ui_Init_TopIdx();
}


/********************************************************************************
key mode in fm delete menu window
********************************************************************************/
void Ui_ModeS_In_DeleteMenu(void)
{
	uc_fm_cur_ch_idx = uc_fm_menu_ch_idx;
	if((ui_fm_preset_buf[uc_fm_cur_ch_idx-1]&0x7fff) == ui_fm_cur_freq)
	{
		uc_fm_mode = FM_MODE_TUNING;	//if deleted channel is current freq, and enter tuning mode
	}
	Ap_FM_Del_CurChannel();				//delete one channel
	
	if(uc_fm_ch_total_num == 0)
	{
		uc_fm_cur_ch_idx = 0;
	}
	else
	{
		uc_fm_cur_ch_idx = uc_fm_cur_ch_idx_bk;
		
		if(uc_fm_cur_ch_idx > uc_fm_menu_ch_idx)		//delete the middle or first channel
		{
			uc_fm_cur_ch_idx--;
		}
		else if(uc_fm_cur_ch_idx > uc_fm_ch_total_num)	//delete the last channel
		{
			uc_fm_cur_ch_idx--;
		}
	}
	
	Ui_ReturnTo_FM();
}

/********************************************************************************
key mode in fm delete menu window
********************************************************************************/
void Ui_ModeS_In_MenuDeleteAll(void)
{
	if(uc_menu_cur_idx==1)
	{
		Ap_FM_Del_AllChannel();	//delete all preset channel
		uc_fm_mode = FM_MODE_TUNING;	//after delete all,in tuning mode
	}
	
	Ui_ReturnTo_FM();
}
/********************************************************************************
key mode in fm signal menu window
********************************************************************************/

void Ui_ModeS_In_Menu_Signal(void)
{
	uc_fm_signal_strength_val = uc_menu_cur_idx;
	uc_fm_accumulative_val = uc_fm_signal_strength_val*16 + 6;

	Ui_ReturnTo_FM();
}

#endif