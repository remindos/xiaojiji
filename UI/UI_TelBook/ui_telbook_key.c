/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name:ui_telbook_key.c
Creator: zj.zong				Date: 2009-06-04
Description: process key event
Others:
Version: V0.1
History:
	V0.1	2009-06-04		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_TELBOOK_KEY_DEF

#include "sh86278_sfr_reg.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_api_define.h"
#include ".\UI\UI_TelBook\ui_telbook_header.h"
#include ".\UI\UI_TelBook\ui_telbook_var_ext.h"

extern void Ui_Init_TopIdx(void);
extern void Ui_SetLcdFlag_0xfd(void);
extern void Ui_PowerOn_Backlight(void);	 
extern void Ui_Return_To_TelBookMain(void);
extern void Ui_Display_StringImport_OK(void);
extern void Ui_Display_StringExporting(void);
extern void Ui_Display_StringImporting(void);
extern void Ui_Display_String_NoTelFile(void);
extern void Ui_Display_String_NoSpace(void);
extern void Ui_Display_StringExport_OK(void);

extern bit  Ap_Export_TelFile(void);
extern bit  Ap_Check_TelFile_Exist(void);
extern void Ap_Init_TelFile(void);
extern void Ap_PowerOff_MP3(void);
extern void Ap_SwitchTo_MainMenu(void);
extern void Ap_Exit_TelBookMode(void);

extern void FS_EnterRootDir(void);
extern void FS_GetItemNum_InDir(void);
extern void FS_GetSpace(void);
extern void Api_Key_Mapping(void);
extern void Api_DynamicLoad_Tel_Target(uc8 load_code_idx);
extern void Api_DynamicLoad_CodeMpm1_8kExecute(void);
extern void Api_DynamicLoad_CodeCpm(void);

void Ui_Sel_Next_Menu(void);
void Ui_Sel_Previous_Menu(void);
void Ui_ModeS_In_TelBookIn(void);
void Ui_ModeS_In_TelBookOut(void);
void Ui_ModeS_In_TelBookMain(void);
void Ui_PlayS_In_TelBookName(void);
void Ui_SelNext_In_TelBookName(void);
void Ui_SelPrevious_In_TelBookName(void);
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
		}	 */
		
		if((ui_bklight_time == 0) && (uc_bklight_time_level != 0))
		{
			Ui_PowerOn_Backlight();
			return;
		}

		Ui_PowerOn_Backlight();			//restore backlight
		switch(uc_key_value)
		{
			case KEY_PLAY_S:
				switch(uc_window_no)
				{
					case WINDOW_TELBOOK_NAME:
						Ui_PlayS_In_TelBookName();
						break;
						
					case WINDOW_TELEBOOK_IN:
						uc_menu_cur_idx = 2;
						Ui_Return_To_TelBookMain();
						break;
						
					case WINDOW_TELEBOOK_OUT:
						uc_menu_cur_idx = 3;
						Ui_Return_To_TelBookMain();
						break;
						
					default:
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
			case KEY_NEXT_L:
			case KEY_NEXT_LC:
				if(uc_window_no == WINDOW_TELBOOK_NAME)
				{
					Ui_SelNext_In_TelBookName();
				}
				else
				{
					Ui_Sel_Next_Menu();
				}
				break;

			case KEY_LAST_S:
			case KEY_LAST_L:
			case KEY_LAST_LC:
				if(uc_window_no == WINDOW_TELBOOK_NAME)
				{
					Ui_SelPrevious_In_TelBookName();
				}
				else
				{
					Ui_Sel_Previous_Menu();
				}
				break;


			case KEY_REC_L:
				if(((uc_user_option2&0x20) != 0)&&((uc_user_option2&0x08) == 0))
				{
					Ap_Exit_TelBookMode();

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
					case WINDOW_TELBOOK_MAIN:
						Ui_ModeS_In_TelBookMain();
						break;
				
					case WINDOW_TELBOOK_NAME:
						uc_menu_cur_idx = 1;
						Ui_Return_To_TelBookMain();
						break;

					case WINDOW_TELBOOK_INFO:
						uc_window_no = WINDOW_TELBOOK_NAME;			//return to name list
						Ui_SetLcdFlag_0xfd();
						break;

					case WINDOW_TELEBOOK_IN:
					  	Ui_ModeS_In_TelBookIn();
					  	break;

					case WINDOW_TELEBOOK_OUT:
					   	Ui_ModeS_In_TelBookOut();
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
//	}
//	else								//hold state
//	{
//		Ui_PowerOn_Backlight();		//restore backlight
//		uc_lcd_dis_flg |= 0x02;			//set bit1, display hold pic
//	}

	ui_sys_event |= EVT_DISPLAY;		//set the display event
}

/*******************************************************************************
Function: 1.Ui_PlayS_In_TelBookName()
Description: function of short press key play
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_PlayS_In_TelBookName(void)
{
	uc_window_no = WINDOW_TELBOOK_INFO;
	uc_menu_cur_idx = 1;
	uc_menu_top_idx = 0;
	uc_menu_total_num = DisInfo_Num;
	Ui_Init_TopIdx();
	Ui_SetLcdFlag_0xfd();
}

/*******************************************************************************
Function: 1.Ui_SelNext_In_TelBookName()
		  2.Ui_Sel_Next_Menu()
Description: function of press key next
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_SelNext_In_TelBookName(void)
{
	uc_tel_namelist_sel_idx++;
	if(uc_tel_namelist_sel_idx < uc_tel_namelist_num)
	{
		if(uc_tel_namelist_sel_idx >= (uc_tel_namelist_top_idx + DisMenu_MaxNum))
		{
			uc_tel_namelist_top_idx++;
		}
	}
	else
	{
		uc_tel_namelist_sel_idx = 0;
		uc_tel_namelist_top_idx = 0;
	}
	uc_lcd_dis_flg= 0x28;					

	b_lcd_clrall_flg = 1;		//clr screen
}

/*******************************************************************************
sel next menu
********************************************************************************/
void Ui_Sel_Next_Menu(void)
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
	uc_lcd_dis_flg= 0xfd;					
	b_lcd_clrall_flg = 1;		//clr screen
}

/*******************************************************************************
Function: 1.Ui_SelPrevious_In_TelBookName()
		  2.Ui_Sel_Previous_Menu()
Description: function of press key last
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_SelPrevious_In_TelBookName(void)
{
	if(uc_tel_namelist_sel_idx == 0)
	{
		uc_tel_namelist_sel_idx = uc_tel_namelist_num - 1;
		if(uc_tel_namelist_num >= DisMenu_MaxNum)
		{
			uc_tel_namelist_top_idx = uc_tel_namelist_sel_idx - DisMenu_MaxNum + 1;
		}
		else
		{
			uc_tel_namelist_top_idx = 0;
		}
	}
	else
	{
		uc_tel_namelist_sel_idx--;
	}

	if(uc_tel_namelist_top_idx > uc_tel_namelist_sel_idx)
	{
		uc_tel_namelist_top_idx = uc_tel_namelist_sel_idx;
	}
	uc_lcd_dis_flg= 0x28;					

	b_lcd_clrall_flg = 1;		//clr screen
}

/*******************************************************************************
sel previous menu
********************************************************************************/
void Ui_Sel_Previous_Menu(void)
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
	uc_lcd_dis_flg= 0xfd;					

	b_lcd_clrall_flg = 1;		//clr screen
}

/*******************************************************************************
Function: 1.Ui_ModeS_In_TelBookMain()
		  2.Ui_ModeS_In_TelBookIn()
		  3.Ui_ModeS_In_TelBookOut()
Description: function of press key mode
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_ModeS_In_TelBookMain(void)
{
	switch(uc_menu_cur_idx)
	{
		case 0x01:
			Ap_Init_TelFile();
			if(uc_tel_namelist_num != 0)
			{
				uc_window_no = WINDOW_TELBOOK_NAME;
				uc_menu_top_idx = 0;
				Ui_Init_TopIdx();
				Ui_SetLcdFlag_0xfd();

			}
			else
			{
				Ui_Display_String_NoTelFile();
				Ui_Return_To_TelBookMain();
			}
			break;

		case 0x02:
			uc_window_no = WINDOW_TELEBOOK_IN;
			uc_menu_cur_idx = 2;
			uc_menu_total_num = 2;
			break;

		case 0x03:
			uc_window_no = WINDOW_TELEBOOK_OUT;
			uc_menu_cur_idx = 2;
			uc_menu_total_num = 2;
			break;

		default:
			Ap_SwitchTo_MainMenu();
			break;
	}
}

/*******************************************************************************
key mode in telbook in
********************************************************************************/
void Ui_ModeS_In_TelBookIn(void)
{
	if(uc_menu_cur_idx == 1)
	{
		Api_DynamicLoad_Tel_Target(CODE_TEL_4_CHECK_FILE_IDX);
		if(b_telbook_file_exist == 1)
		{
			Ui_Display_StringImporting();
			Api_DynamicLoad_Tel_Target(CODE_TEL_2_IMPORT_IDX);
			Ui_Display_StringImport_OK();
		}
		else
		{
			Ui_Display_String_NoTelFile();
		}
	}

	uc_menu_cur_idx = 2;
	Ui_Return_To_TelBookMain();
}

/*******************************************************************************
key mode in telbook out
********************************************************************************/
void Ui_ModeS_In_TelBookOut(void)
{
	if(uc_menu_cur_idx == 1)
	{
		Ui_Display_StringExporting();

		FS_EnterRootDir();					//enter root
		FS_GetItemNum_InDir();
		Api_DynamicLoad_Tel_Target(CODE_TEL_1_DEL_OLD_FILE_IDX);
	   	FS_GetSpace();
		Api_DynamicLoad_Tel_Target(CODE_TEL_3_EXPORT_FILE_IDX);

		if(b_export_file_ok == 1)
		{
			Ui_Display_StringExport_OK();
		}
		else
		{
			Ui_Display_String_NoSpace();
		}
	}
	
	uc_menu_cur_idx = 3;
	Ui_Return_To_TelBookMain();
}

#endif