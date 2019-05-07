/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_rec_key.c
Creator: zj.zong				Date: 2009-06-01
Description: process key event
Others:
Version: V0.1
History:
	V0.1	2009-06-01		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_RECORD_KEY_PROCESS_DEF

#include "sh86278_api_define.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include ".\UI\UI_Record\ui_rec_header.h"
#include ".\UI\UI_Record\ui_rec_var_ext.h"

extern void Ui_SetLcdFlag_0xfd(void);
extern void Ui_PowerOn_Backlight(void);
extern void Ui_ReturnTo_Rec(void);

extern void Ap_CalRec_RemainTime(void);

extern void Ap_SetStopRec(void);
extern void Ap_SetStartRec(void);
extern void Ap_SetPauseRec(void);
extern void Ap_PowerOff_MP3(void);
extern void Ap_SetContinueRec(void);
extern void Ap_SwitchTo_MainMenu(void);
extern void Api_Key_Mapping(void);
extern void Api_DynamicLoad_Record_Target(uc8 load_code_idx);
extern bit Ap_Check_SDExist(void);
extern void Ap_Init_VoiceDir(void);
extern void Ui_Display_MSWP(void);
extern void Ui_Init_TopIdx(void);

void Ui_PlayS_InRecord(void);
void Ui_PlayL_In_Record(void);
void Ui_ModeS_InRec(void);
void Ui_ModeS_In_RecMenu(void);	   //allen
void Ui_ModeS_In_RecMenuSD(void);	   //allen
void Ui_ModeS_In_QualityMenu(void);	   //allen
void Ui_SelPrevious_Menu(void);
void Ui_SelNext_Menu(void);

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
	ui_idle_poweroff_cnt = 0;				//reset the idle_cnt when press any key
	uc_menu_exit_cnt = 0;					//clr the counter, exit menu window

	Api_Key_Mapping();

/*	if(b_key_hold_val == 0)					//not hold state
	{
		if(	uc_hold_cnt != 0)
		{
			uc_hold_cnt = 0;
		 	Ui_SetLcdFlag_0xfd();
		}	  */
		if((ui_bklight_time == 0) && (uc_bklight_time_level != 0))
		{
			Ui_PowerOn_Backlight();
			return;
		}
		Ui_PowerOn_Backlight();				// power on back light grade
		switch(uc_key_value)
		{
			case KEY_PLAY_S:
				if(uc_window_no == WINDOW_REC)
				{
					Ui_PlayS_InRecord();	//start or pause record
				}
				else
				{
					Ui_ReturnTo_Rec();	//return to record window
				}
				break;

			case KEY_REC_S:
				if((uc_user_option2&0x20) != 0)
				{
			   	  	if(uc_window_no == WINDOW_REC)
					{
						Ui_PlayS_InRecord();	//start or pause record
					}	
				}
				break;

			case KEY_PLAY_L:
				Ui_PlayL_In_Record();		//save record or power off mp3
				break;
			
			case KEY_NEXT_S:
			case KEY_NEXT_L:
			case KEY_NEXT_LC:
				if(uc_window_no != WINDOW_REC)	//select next menu
				{
					Ui_SelNext_Menu();
				}  
				break;

			case KEY_LAST_S:
			case KEY_LAST_L:
			case KEY_LAST_LC:
				if(uc_window_no != WINDOW_REC)	//select last menu
				{
					Ui_SelPrevious_Menu();
				}
				break;
	
			case KEY_MODE_S:
				switch(uc_window_no)
				{
					case WINDOW_REC:
						Ui_ModeS_InRec();
						break;
						
					case WINDOW_REC_MENU_FLASH:
						Ui_ModeS_In_RecMenu();
						break;

					case WINDOW_REC_MENU_SD:
						Ui_ModeS_In_RecMenuSD();
						break;
						
					case WINDOW_REC_MENU_QUALITY:
						Ui_ModeS_In_QualityMenu();
						break;	
				}
				break;

			case KEY_REC_L:
				if(((uc_user_option2&0x20) != 0)&&((uc_user_option2&0x08) == 0))
				{
					if(uc_rec_status != STATE_STOP)
					{
					 	Ap_SetStopRec();
						Api_DynamicLoad_Record_Target(CODE_RECORD_7_GET_NEW_FILE_IDX);	//get new record file index and calculate the remaining time
						uc_lcd_dis_flg |= 0x5c;			//dis time, index, name, state
					}
				}
				break;
	
			case KEY_MODE_L:					   	//return to main menu
				Ap_SwitchTo_MainMenu();
				break;
		}
/*	}
	else									//hold state
	{
		Ui_PowerOn_Backlight();			// restore back light grade
		uc_lcd_dis_flg |= 0x02;				//set bit1, display hold pic
	}  */

	ui_sys_event |= EVT_DISPLAY;			//set the display event
}



/*******************************************************************************
Function: 1.Ui_PlayS_InRec()
		  2.Ui_PlayL_InRec()
Description: function of press key play
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ui_PlayS_InRecord(void)
{
	switch(uc_rec_status)
	{
		case STATE_STOP:
			if((b_sd_flash_flg == 1) && ((uc_user_option1 & 0x10) != 0) && ((MR_MSCON4 & 0x40) == 0))
			{
				Ui_Display_MSWP();
				Ui_ReturnTo_Rec();	
			}
			else
			{
				Ap_SetStartRec();	
			}
			break;
			
		case STATE_RECORD:
			Ap_SetPauseRec();
			break; 
			
		default:
			Ap_SetContinueRec();
			break; 
	}
	
	uc_lcd_dis_flg |= 0x10;						//set the bit4
}

/*******************************************************************************
exit record mode or save current record
********************************************************************************/
void Ui_PlayL_In_Record(void)
{
	if(b_play_poweron_flg == 0)
	{
		if(uc_rec_status == STATE_STOP)
		{
			uc_poweroff_type = POWEROFF_NORMAL;
			Ap_PowerOff_MP3();
		}
		else								//stop record and prepare to record again
		{
			Ap_SetStopRec();
			Api_DynamicLoad_Record_Target(CODE_RECORD_7_GET_NEW_FILE_IDX);	//get new record file index and calculate the remaining time
			uc_lcd_dis_flg |= 0x5c;			//dis time, index, name, state
		}
	}
}

/*******************************************************************************
Function: 1.Ui_SelNext_Menu()
Description: function of press key next
Calls:
Global:Null
Input: Null
Output:Null
Others:
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
	uc_lcd_dis_flg = 0x58;
} 

/*******************************************************************************
Function: 1.Ui_SelPrevious_Menu()
Description: function of press key last
Calls:
Global:Null
Input: Null
Output:Null
Others:
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
Function: 1.Ui_ModeS_In_Record()
		  2.Ui_ModeS_In_Record_Menu()
Description: function of press key mode
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************
void Ui_ModeS_In_Record(void)
{
	if(uc_rec_status == STATE_STOP)
	{
		uc_window_no = WINDOW_REC_MENU;
		if(b_rec_samplerate_flg == 0)
		{
			uc_menu_cur_idx = 1;			//normal quality		
		}
		else
		{
			uc_menu_cur_idx = 2;			//high quality	
		}
		uc_menu_total_num = 2;				//init menu index and total item
		Ui_SetLcdFlag_0xfd();				//set display flag
	}
} */
 /*******************************************************************************
Function: 1.Ui_ModeS_InRec()
		  2.Ui_ModeS_In_RecMenu()
		  3.Ui_ModeS_In_RecMenuSD()
		  4.Ui_ModeS_In_QualityMenu()
Description: function of short press key mode
*******************************************************************************/
void Ui_ModeS_InRec(void)
{
	switch(uc_rec_status)
	{
		case STATE_STOP:
			if(Ap_Check_SDExist() == 0)
			{
				uc_window_no = WINDOW_REC_MENU_FLASH;
				uc_menu_cur_idx = 1;
				uc_menu_total_num = 2;	
			}
			else
			{
				uc_window_no = WINDOW_REC_MENU_SD;
				if(b_sd_flash_flg == 1)
				{
					uc_menu_cur_idx = 1;							//init menu index and total item
				}
				else
				{
					uc_menu_cur_idx = 2;	
				}
				uc_menu_total_num = 4; //Rec_MenuSD_Num;	
			}
			uc_menu_top_idx = 0;
			Ui_Init_TopIdx();
			Ui_SetLcdFlag_0xfd();				
			break; 

		default:
			break;
	}			
}

  /*******************************************************************************/
void Ui_ModeS_In_RecMenu(void)
{
	if(uc_menu_cur_idx == 1)	
	{
		uc_window_no = WINDOW_REC_MENU_QUALITY;
		if(b_rec_samplerate_flg == 0)
		{
			uc_menu_cur_idx = 1;				//normal quality		
		}
		else
		{
			uc_menu_cur_idx = 2;				//high quality	
		}
		uc_menu_total_num =2;  // Rec_MenuFlash_Num;					//init menu index and total item 


		Ui_SetLcdFlag_0xfd();		
	}
	else
	{
		Ui_ReturnTo_Rec();	
	}
}

/*******************************************************************************/
void Ui_ModeS_In_RecMenuSD(void)
{
	switch(uc_menu_cur_idx)	
	{
		case 1:
			if(b_sd_flash_flg == 0)
			{
				b_sd_flash_flg = 1;
				uc_dynamicload_idx = 30;		//FS_Finit
				Api_DynamicLoad_Record_Target(CODE_RECORD_A_SWITCH_CARD_IDX);
				goto Label_InitRec;
			}
			Ui_ReturnTo_Rec();
			break;

		case 2:
			if(b_sd_flash_flg == 1)
			{
				b_sd_flash_flg = 0;
				uc_dynamicload_idx = 30;		//FS_Finit
				Api_DynamicLoad_Record_Target(CODE_RECORD_A_SWITCH_CARD_IDX);
Label_InitRec:
				Ap_Init_VoiceDir();	
			}
			Ui_ReturnTo_Rec();
			break;

		case 3:
			uc_window_no = WINDOW_REC_MENU_QUALITY;
			if(b_rec_samplerate_flg == 0)
			{
				uc_menu_cur_idx = 1;				//normal quality		
			}
			else
			{
				uc_menu_cur_idx = 2;				//high quality	
			}
			uc_menu_total_num = 2; //Rec_MenuFlash_Num;					//init menu index and total item 
			uc_menu_top_idx = 0;
			Ui_Init_TopIdx();
			Ui_SetLcdFlag_0xfd();
			break;

		case 4:
			Ui_ReturnTo_Rec();
			break;

		default:
			break;
	}
}

/*******************************************************************************
select normal quality or high quality
********************************************************************************/
void Ui_ModeS_In_QualityMenu(void)
{
	if(uc_menu_cur_idx == 1)
	{
		b_rec_samplerate_flg = 0;	//normal quality
	}
	else
	{
		b_rec_samplerate_flg = 1;	//high quality	
	}	
	Ap_CalRec_RemainTime();
	
	if(b_rec_samplerate_flg == 1)	//16KHz sample rate
	{
		MR_SYSCLK |= 0x40;			//bit6: 1--256KHz, used for 16K sample rate
	}
	else
	{
		MR_SYSCLK &= 0xbf;			//bit6: 0--128KHz, used for 8K sample rate
	}	
	Ui_ReturnTo_Rec();
}


#endif