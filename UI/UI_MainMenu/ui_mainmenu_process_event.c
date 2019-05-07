/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_mainmenu_process_event.c
Creator: zj.Zong					Date: 2009-04-20
Description: process mainmenu events
Others:
Version: V0.1
History:
	V0.1	2009-04-20		zj.Zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_MAINMENU_PROCESS_DEF

#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include ".\UI\UI_MainMenu\ui_mainmenu_header.h"
#include ".\UI\UI_MainMenu\ui_mainmenu_var_ext.h"


extern void Ui_PowerOn_Backlight(void);
extern void Ui_Display_HoldPic(void);

extern void Ap_PowerOff_MP3(void);
extern void Ap_MainMenu_DynamicLoad_Music(void);
extern void Ap_MainMenu_DynamicLoad_Record(void);
extern void Ap_MainMenu_DynamicLoad_Voice(void);
extern void Ap_MainMenu_DynamicLoad_FM(void);
extern void Ap_MainMenu_DynamicLoad_System(void);
extern void Ap_MainMenu_DynamicLoad_Ebook(void);
extern void Ap_MainMenu_DynamicLoad_Telbook(void);

extern void Api_Key_Mapping(void);
extern void Api_Check_Battery(void);
extern void Api_Check_BklightOff(void);
extern void Api_Check_Sleep_PowerOff(void);
extern void Api_Check_Idle_PowerOff(void);
extern void Api_Delay_1ms(uc8 time_cut);
extern void Api_Display_Picture(ui16 ui_pic_idx);
extern void Api_DynamicLoad_CodeMpm1_8kExecute(void);
extern void Api_DynamicLoad_CodeCpm(void);
extern void Api_LcdDriver_Clr_AllPage(void);

void Ui_ProcessEvt_Timer(void);
void Ui_ProcessEvt_Display(void);
void Ui_ProcessEvt_Key(void);
void Ui_Display_Loading(void);
void Ui_SelNext_Menu(void);
void Ui_SelPrevious_Menu(void);
void Ui_Enter_SubMode(void);

/*******************************************************************************
Function: Ui_ProcessEvt_Timer()
Description: process timer
Calls:
Global:uc_timer_cnt
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_ProcessEvt_Timer(void)
{
	if(uc_timer_cnt >= 5)						//5*20ms = 100ms
	{
		uc_timer_cnt = 0;
		
		Api_Check_Battery();							
		Api_Check_Sleep_PowerOff();				//sleep mode	

		Api_Check_BklightOff();					//power off backlight
		
		Api_Check_Idle_PowerOff();				//if mp3 is pause/stop state and power off after 1'minute

	}	
}


/*******************************************************************************
Function: Ui_ProcessEvt_Display()
Description: process display
Calls:
Global:uc_lcd_dis_flg, uc_window_no
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_ProcessEvt_Display(void)
{
 /*	if((uc_lcd_dis_flg & BIT1_MASK) != 0)
	{
		uc_hold_cnt = 10;
		Ui_Display_HoldPic();
	}
	
	if(uc_hold_cnt == 0)
  	{  */
 	   	if((uc_lcd_dis_flg & BIT7_MASK) != 0)
		{
			uc_lcd_page_num = 0;	
			uc_lcd_column_num = 0;
			switch((uc_user_option1 & 0x01)|(uc_user_option2 & 0x08)|(uc_user_option3 & 0x80))
			{
				case 0x00:			 //have music rec voice fm sys ebook 
					Api_Display_Picture(PIC_MAIN_MENU_BASE_IDX + uc_menu_cur_idx - 1);
					break;
				case 0x01:			 //have music rec voice sys ebook 
					Api_Display_Picture(PIC_MAIN_MENU_NO_FM_BASE_IDX + uc_menu_cur_idx - 1);
				  	break;
				case 0x08:			 //have music fm sys ebook 
					Api_Display_Picture(PIC_MAIN_MENU_NO_VOICE_REC_BASE_IDX + uc_menu_cur_idx - 1);
				  	break;
				case 0x09:		     //have music sys ebook
					Api_Display_Picture(PIC_MAIN_MENU_NO_VOICE_REC_FM_BASE_IDX + uc_menu_cur_idx - 1);
				  	break;
				case 0x80:			 //have music sys fm rec voice 
					Api_Display_Picture( PIC_MAIN_MENU_NO_EBOOK_TEL_BASE_IDX + uc_menu_cur_idx - 1);
				  	break;
				case 0x81:			 //have music sys rec voice 
					Api_Display_Picture( PIC_MAIN_MENU_NO_FM_EBOOK_TEL_BASE_IDX + uc_menu_cur_idx - 1);
				  	break;
				case 0x88:			 //have music sys fm 
					Api_Display_Picture( PIC_MAIN_MENU_HAVE_MUSIC_FM_SYS_BASE_IDX + uc_menu_cur_idx - 1);
				  	break;
				case 0x89:		     //have music sys 
					Api_Display_Picture(PIC_MAIN_MENU_HAVE_MUSIC_SYS_BASE_IDX + uc_menu_cur_idx - 1);
				  	break;
			}
		
			uc_lcd_page_num = 3;
			uc_lcd_column_num = 50;
			Api_Display_Picture(uc_menu_cur_idx - 1 + MENU_STR_MAIN_MENU_IDX + uc_language_idx * ALL_MENU_NUMBER);	
		}
//	 }													   
	b_lcd_clrall_flg = 0;
	uc_lcd_dis_flg = 0;	
}


void Ui_Display_Loading(void)
{
	Api_LcdDriver_Clr_AllPage();				//display top menu background color
  	uc_lcd_page_num = 3;
	uc_lcd_column_num = 0;
  	Api_Display_Picture(NORM_STR_LOADING_IDX + ALL_MENU_NUMBER * uc_language_idx);		//display loding...
 	Api_Delay_1ms(250);
	Api_Delay_1ms(250);
}


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

	Api_Key_Mapping();

//	if(b_key_hold_val == 0)					//not hold state
//	{
		if((ui_bklight_time == 0) && (uc_bklight_time_level != 0))
		{
			Ui_PowerOn_Backlight();
			return;
		}
		Ui_PowerOn_Backlight();				//power on backlight
									
		switch(uc_key_value)
		{
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
				Ui_SelNext_Menu();
				break;

			case KEY_LAST_S:
			case KEY_LAST_L:
			case KEY_LAST_LC:
				Ui_SelPrevious_Menu();
				break;

			case KEY_REC_L:
				if(((uc_user_option2&0x20) != 0)&&((uc_user_option2&0x08) == 0))
				{
					b_record_key_flag = 1;
	                uc_load_code_idx = CODE_RECORD_0_MAIN_CPM_IDX;
	                Api_DynamicLoad_CodeCpm();					//load code into CPM //allen
	                uc_load_code_idx = CODE_RECORD_0_MAIN_IDX;
	                Api_DynamicLoad_CodeMpm1_8kExecute();		//load code into MPM1
				}
				break;

			case KEY_MODE_S:
				Ui_Enter_SubMode();
				break;
			
			default:
				break;
		}
//	}
//	else
//	{
//		Ui_PowerOn_Backlight();				//power on backlight
//		uc_lcd_dis_flg |= 0x02;				//set bit1, display hold pic
//	}
	ui_sys_event |= EVT_DISPLAY;			//set the display event
}


/*******************************************************************************
Function: Ui_SelNext_Menu()
Description: Select the next menu (uc_menu_cur_idx+1)
Calls:
Global:uc_key_value
Input: uc_menu_cur_idx,uc_menu_total_num
Output:Null
Others:
********************************************************************************/
void Ui_SelNext_Menu(void)
{
   	if(uc_menu_cur_idx >= 7)
 	{
		uc_menu_cur_idx = 1;
	}
	else if(uc_menu_cur_idx >= 5)
	{
	   	if((uc_user_option3 & 0x80)==0x80)		//MainMenu no ebook
		{
			uc_menu_cur_idx = 1;
		}
		else
		{
			uc_menu_cur_idx++;
		}
	}
  	else
	{
	 	if((uc_user_option2 & 0x08)==0x00)		//MainMenu have all mode
		{
			if((uc_user_option1 & 0x01) == 0)	//MainMenu have fm
			{
				uc_menu_cur_idx++;
			}
			else								// MainMenu no fm
			{
				if(uc_menu_cur_idx == 3)
				{
					uc_menu_cur_idx = 5;
				}
				else
				{
					uc_menu_cur_idx++;
				}
			}
		}
		else 									//MainMenu no voice and record
		{
			if((uc_user_option1 & 0x01) == 0)	//MainMenu no voice record 
			{
				if(uc_menu_cur_idx == 1)
				{
					uc_menu_cur_idx = 4;
				}
				else
				{
					uc_menu_cur_idx++;
				}
			}
			else								//MainMenu no voice record fm	
			{
				if(uc_menu_cur_idx == 1)
				{
					uc_menu_cur_idx = 5;
				}
				else
				{
					uc_menu_cur_idx++;
				}
			}
		}
	}
	uc_lcd_dis_flg |= 0x80;						//set the bit7	
}


/*******************************************************************************
Function: Ui_SelPrevious_Menu()
Description: Select the last menu (uc_menu_cur_idx-1)
Calls:
Global:uc_key_value
Input: uc_menu_cur_idx,uc_menu_total_num
Output:Null
Others:
********************************************************************************/
void Ui_SelPrevious_Menu(void)
{
	if(uc_menu_cur_idx <= 1)
	{
	 	if((uc_user_option3 & 0x80)==0x00)		//MainMenu have all mode
		{
			uc_menu_cur_idx = 7;
		}
		else
		{
		 	uc_menu_cur_idx = 5;				//MainMenu no tel ebook
		}
	}
	else
	{
		if((uc_user_option2 & 0x08)==0x00)		//MainMenu have record voice
		{
			if((uc_user_option1 & 0x01) == 0)	//MainMenu have record voice fm		
			{
				uc_menu_cur_idx--;				
			}
			else								 //MainMenu no fm
			{
				if(uc_menu_cur_idx == 5)
				{
					uc_menu_cur_idx = 3;
				}
				else
				{
					uc_menu_cur_idx--;
				}
			}
		}
		else 									//MainMenu no voice record  
		{																	   
			if((uc_user_option1 & 0x01) == 0)	// MainMenu have fm
			{
				if(uc_menu_cur_idx == 4)
				{
					uc_menu_cur_idx = 1;
				}
				else
				{
					uc_menu_cur_idx--;
				}										
			}
			else								//MainMenu no voice record fm
			{
				if(uc_menu_cur_idx == 5)
				{
					uc_menu_cur_idx = 1;
				}
				else
				{
					uc_menu_cur_idx--;
				}
			}
		}	
	}	
	uc_lcd_dis_flg |= 0x80;						//set the bit7
}


/*******************************************************************************
Function: Ui_Enter_SubMode()
Description: enter the corresponding mode
			 1:music; 2:record; 3:voice; 4:FM
			 5:system; 6:ebook; 7:telbook
Calls:
Global:uc_key_value
Input: uc_menu_cur_idx,uc_menu_total_num
Output:Null
Others:
********************************************************************************/
void Ui_Enter_SubMode(void)
{
	Ui_Display_Loading();
	switch(uc_menu_cur_idx)
	{
		case MODE_1_MUSIC:
			Ap_MainMenu_DynamicLoad_Music();
			break;

		case MODE_2_RECORD:
			Ap_MainMenu_DynamicLoad_Record();
			break;

		case MODE_3_VOICE:
			Ap_MainMenu_DynamicLoad_Voice();
			break;

		case MODE_4_FM:
			Ap_MainMenu_DynamicLoad_FM();
			break;

		case MODE_5_SYSTEM:
			Ap_MainMenu_DynamicLoad_System();
			break;

		case MODE_6_EBOOK:
			Ap_MainMenu_DynamicLoad_Ebook();
			break;

		case MODE_7_TELBOOK:
			Ap_MainMenu_DynamicLoad_Telbook();
			break;
	}
}

#endif