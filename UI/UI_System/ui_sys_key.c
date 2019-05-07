/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_sys_key.c
Creator: zj.zong			Date: 2009-05-27
Description: process key event
Others:
Version: V0.1
History:
	V0.1	2009-05-27		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_SYS_KEY_DEF

#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include ".\UI\UI_System\ui_sys_header.h"
#include ".\UI\UI_System\ui_sys_var_ext.h"

extern void Ui_SetLcdFlag_0xfd(void);
extern void Ui_ReturnTo_SysMenu(void);
extern void Ui_ReturnTo_SysMenu_PowerOff(void);
extern void Ui_PowerOn_Backlight(void);
extern void Ap_PowerOff_MP3(void);
extern void Ap_Restore_FactoryPara(void);
extern void Ap_SwitchTo_MainMenu(void);
extern void Api_Key_Mapping(void);
extern void Ap_Exit_SysMode(void);
extern void Api_LcdDriver_Set_BklightLevel(uc8 bklight_grade);
extern void Api_DynamicLoad_CodeMpm1_8kExecute(void);
extern void Api_DynamicLoad_CodeCpm(void);
extern void Api_LcdDriver_Clr_AllPage(void);
extern void Api_Display_Picture(ui16 ui_pic_idx);
extern void Api_Delay_1ms(uc8 timer_cnt);
extern bit FS_Finit(void);//allen
extern void FS_GetSpace(void);
extern void FS_WrFlash_Interface(void);

void Ui_Sel_Next_Menu(void);
void Ui_Sel_Previous_Menu(void);
void Ui_ModeS_In_SysMenu(void);
void Ui_ModeS_In_sysMenu_BkLight_Time(void);
void Ui_ModeS_In_SysMenu_Replay(void);
void Ui_ModeS_In_SysMenu_PowerOff(void);
void Ui_ModeS_In_SysMenu_PowerOff_Idle(void);
void Ui_ModeS_In_SysMenu_PowerOff_Sleep(void);
void Ui_ModeS_In_SysMenu_Contrast(void);
void Ui_ModeS_In_SysMenu_Language(void);
void Ui_ModeS_In_SysMenu_Product_Info(void);
void Ui_ModeS_In_SysMenu_Factory(void);
void Ui_Sel_Next_In_Contrast(void);
void Ui_Sel_Last_In_Contrast(void);
void Ui_ModeS_In_RecMenu(void);
void Ui_ModeS_In_OnLine(void);
void Ui_ModeS_In_SysMenu_AMT(void);
void Ui_Init_TopIdx(void);

code ui16 Ui_Sleep_Time_Table[]={0x0000,0x1770,0x2ee0,0x4650,0x5dc0,0x7530,0x8ca0,0xa410,0xbb80,0xd2f0,0xea60,0xffff,0xffff};
code ui16 Ui_Idle_Time_Table[]={0x0000,0x0064,0x00c8,0x012c,0x0190,0x01f4,0x0258};
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
	uc_menu_exit_cnt = 0;						//clr the counter

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
		Ui_PowerOn_Backlight();					//power on backlight

		switch(uc_key_value)
		{
			case KEY_PLAY_S:
				if(uc_window_no != WINDOW_SYS_MENU)
				{
					Ui_ReturnTo_SysMenu();
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
				switch(uc_window_no)
				{
					case WINDOW_SYS_MENU_CONTRAST:
						Ui_Sel_Next_In_Contrast();
						break;
					
					default:
					   	Ui_Sel_Next_Menu();
					   	break;
				}
				break;

			case KEY_LAST_S:
			case KEY_LAST_L:
			case KEY_LAST_LC:
				switch(uc_window_no)
				{
					case WINDOW_SYS_MENU_CONTRAST:
						Ui_Sel_Last_In_Contrast();
						break;		
					
					default:
					   	Ui_Sel_Previous_Menu();
					   	break;
				}
				break;

			case KEY_VOL_S:
		
				switch(uc_window_no)
					{
						case WINDOW_SYS_MENU:
							break;
							
						case WINDOW_SYS_MENU_POWEROFF_IDLE:
							Ui_ReturnTo_SysMenu_PowerOff();
							break;	
							
						case WINDOW_SYS_MENU_POWEROFF_SLEEP:
							Ui_ReturnTo_SysMenu_PowerOff();
							break;	
							
						default:
						   	Ui_ReturnTo_SysMenu();
						   	break;
					}
					break;
		

			

			case KEY_MODE_S:
				switch(uc_window_no)
				{
				 	case WINDOW_SYS_MENU:
						Ui_ModeS_In_SysMenu();
						break;

 /*				 	case WINDOW_SYS_MENU_REC:
						Ui_ModeS_In_RecMenu();
						break;	 */

				 	case WINDOW_SYS_MENU_BKLIGHT_TIME:
						Ui_ModeS_In_sysMenu_BkLight_Time();
						break;

				 	case WINDOW_SYS_MENU_POWEROFF:
						Ui_ModeS_In_SysMenu_PowerOff();
						break;

					case WINDOW_SYS_MENU_POWEROFF_IDLE:
						Ui_ModeS_In_SysMenu_PowerOff_Idle();
						break;

				 	case WINDOW_SYS_MENU_POWEROFF_SLEEP:
						Ui_ModeS_In_SysMenu_PowerOff_Sleep();
						break;

				 	case WINDOW_SYS_MENU_REPLAY:
						Ui_ModeS_In_SysMenu_Replay();
						break;

				 	case WINDOW_SYS_MENU_CONTRAST:
						Ui_ModeS_In_SysMenu_Contrast();
						break;

				 	case WINDOW_SYS_MENU_LANGUAGE:
						Ui_ModeS_In_SysMenu_Language();
						break;

				 	case WINDOW_SYS_MENU_AMT:
						Ui_ModeS_In_SysMenu_AMT();
						break;	

				  	case WINDOW_SYS_MENU_MEMORY:
					case WINDOW_SYS_MENU_VERSION:
						Ui_ModeS_In_SysMenu_Product_Info();
						break;

					case WINDOW_SYS_MENU_FACTORY:
						Ui_ModeS_In_SysMenu_Factory();
						break;

					 case WINDOW_SYS_MENU_ONLINE:
						Ui_ModeS_In_OnLine();
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
	else								//hold state
	{
		Ui_PowerOn_Backlight();			//power on backlight
		uc_lcd_dis_flg |= 0x02;			//set bit1, display hold pic
	}	*/

	ui_sys_event |= EVT_DISPLAY;		//set the display event
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
Select the next menu (uc_menu_cur_idx+1)
********************************************************************************/
void Ui_Sel_Next_Menu(void)
{
	if(uc_menu_cur_idx >= uc_menu_total_num)
	{
		uc_menu_cur_idx = 1;
	}
	else
	{
		if(uc_window_no == WINDOW_SYS_MENU) 	
		{
			if((uc_user_option3&0x04)!=0)	//have amt
			{
				uc_menu_cur_idx++;
			}									 
			else // no amt
		  	{
				if(uc_menu_cur_idx == 5)
				{
					uc_menu_cur_idx = 7;
				}
				else
				{
					uc_menu_cur_idx++;
				}
			}
		}
		else
		{
			uc_menu_cur_idx++;
		}
	}
	Ui_Init_TopIdx();
	uc_lcd_dis_flg |= 0x58;						//set the bit2,bit4 for dis string
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


/********************************************************************************
Select the last menu (uc_menu_cur_idx+1)
********************************************************************************/
void Ui_Sel_Previous_Menu(void)
{
	if(uc_menu_cur_idx <= 1)
	{
		uc_menu_cur_idx = uc_menu_total_num;
	}
	else
	{
		if(uc_window_no == WINDOW_SYS_MENU) 	
		{
			if((uc_user_option3&0x04)!=0)	//have amt
			{
				uc_menu_cur_idx--;
			}									 
			else // no amt
		  	{
				if(uc_menu_cur_idx == 7)
				{
					uc_menu_cur_idx = 5;
				}
				else
				{
					uc_menu_cur_idx--;
				}
			}
		}
		else
		{
			uc_menu_cur_idx--;
		}
	}
 	Ui_Init_TopIdx();
	uc_lcd_dis_flg |= 0x58;						//set the bit2,bit4 for dis string
}
				
/*******************************************************************************
Function: 1.Ui_ModeS_In_SysMenu()
		  2.Ui_ModeS_In_sysMenu_BkLight_Time()
		  4.Ui_ModeS_In_SysMenu_PowerOff()
		  5.Ui_ModeS_In_SysMenu_PowerOff_Idle()
		  6.Ui_ModeS_In_SysMenu_PowerOff_Sleep()
		  7.Ui_ModeS_In_SysMenu_Contrast()
		  8.Ui_ModeS_In_SysMenu_Language()
		  9.Ui_ModeS_In_SysMenu_Product_Info()
		  10.Ui_ModeS_In_SysMenu_Factory()
Description: function of press key mode
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_ModeS_In_SysMenu(void)
{
	uc_menu_cur_idx_bk = uc_menu_cur_idx;

	switch(uc_menu_cur_idx)
	{
/*	   	case 1:
			uc_window_no = WINDOW_SYS_MENU_REC;
			if(uc_rec_quality_level == 0)
			{
				uc_menu_cur_idx = 1;			//normal quality		
			}
			else
			{
				uc_menu_cur_idx = 2;			//high quality	
			}
			uc_menu_total_num = 2;
			uc_lcd_dis_flg |= 0x58;						//set the bit2,bit4 for dis string
			break;	  */
	
		case 1:
			uc_window_no = WINDOW_SYS_MENU_REPLAY;
			uc_menu_total_num = 2;

			if(uc_mp3_replay_mode == 0)
			{
				uc_menu_cur_idx = 1;			//auto 		
			}
			else
			{
				uc_menu_cur_idx = 2;			//manual 	
			}
			uc_lcd_dis_flg |= 0x58;				//set the bit2,bit4 for dis string
			break;
	
		case 2:
			uc_window_no = WINDOW_SYS_MENU_BKLIGHT_TIME;
			uc_menu_cur_idx = uc_bklight_time_level + 1; 
			uc_menu_total_num = 0x07;					 //max back light level is 7
		   	uc_lcd_dis_flg |= 0x58;						//set the bit4 for dis string
			break;
	
		case 3:
			uc_window_no = WINDOW_SYS_MENU_POWEROFF;
			uc_menu_total_num = 3;
			uc_menu_cur_idx = 1;						//power off mp3: idle or sleep power off 
			uc_lcd_dis_flg |= 0x58;						//set the bit2,bit4 for dis string
			break;

		case 4:
		 	uc_window_no = WINDOW_SYS_MENU_CONTRAST;
			uc_menu_cur_idx = uc_bklight_grade;			//max contrast level is 16
			b_lcd_clrall_flg = 1;
			uc_lcd_dis_flg |= 0x10;						//set the bit4 for dis string
			break;

		case 5:
		   	uc_window_no = WINDOW_SYS_MENU_LANGUAGE;
		   	uc_menu_cur_idx = uc_language_idx + 1;		
			uc_menu_total_num = uc_language_total_num;	//total language number is 25
			uc_lcd_dis_flg |= 0x58;						//set the bit2,bit4 for dis string
			break;

		case 6:
		   	uc_window_no = WINDOW_SYS_MENU_AMT;
			if(uc_amt_onoff_flg == 1)
			{
				uc_menu_cur_idx = 1;
			}
			else
			{
				uc_menu_cur_idx = 2;
			}
			uc_menu_total_num = 2;						//total  number is 2
			uc_lcd_dis_flg |= 0x58;						//set the bit2,bit4 for dis string
			break;

		case 7:
			uc_window_no = WINDOW_SYS_MENU_MEMORY;	  	//mp3 memory
									 
			Api_LcdDriver_Clr_AllPage();
			uc_lcd_page_num = 3;	
			uc_lcd_column_num = 0;
			Api_Display_Picture(NORM_STR_LOADING_IDX + ALL_MENU_NUMBER * uc_language_idx);
		
			if(uc_fs_fat_sector_max_num3 < 0x20)	//if flash memory < 1G, and delay 500ms for display "loading..."
			{
				Api_Delay_1ms(250);
				Api_Delay_1ms(250);
			}
			b_sd_flash_flg = 0;	//flash space
			FS_Finit();
			FS_GetSpace();						//get the flash spare capacity
			b_lcd_clrall_flg = 1;						//clr screen
			break;
			  
		case 8:
			uc_window_no = WINDOW_SYS_MENU_VERSION;	   	//mp3 current version			
			uc_lcd_dis_flg |= 0x10;						//set the bit4 for dis string
			break;

		case 9:
		  	uc_window_no = WINDOW_SYS_MENU_FACTORY;
			uc_menu_cur_idx = 2;				
			uc_menu_total_num = 2;						//factory :sue or cancel
			uc_lcd_dis_flg |= 0x58;						//set the bit2,bit4 for dis string
			break;

		case 10:
		  	uc_window_no = WINDOW_SYS_MENU_ONLINE;
			uc_menu_cur_idx = uc_disk_state;			//init menu index and total item(bit0, 1)
			uc_menu_total_num = 3;
			uc_lcd_dis_flg |= 0x58;						//set the bit2,bit4 for dis string
			break;

		default:
		   	Ap_SwitchTo_MainMenu();
			break;
	}
	b_lcd_clrall_flg = 1;
	uc_menu_top_idx = 0;
	Ui_Init_TopIdx();
}

/********************************************************************************
key mode rec quality
********************************************************************************/
/*void Ui_ModeS_In_RecMenu(void)
{
	if(uc_menu_cur_idx == 1)
	{
		uc_rec_quality_level = 0;			//normal quality		
	}
	else
	{
		uc_rec_quality_level = 1;			//high quality	
	}
	Ui_PowerOn_Backlight();						//update bklight time
	Ui_ReturnTo_SysMenu();

}*/

/********************************************************************************
key mode in back light time
********************************************************************************/
void Ui_ModeS_In_sysMenu_BkLight_Time(void)
{
	uc_bklight_time_level = uc_menu_cur_idx - 1;
	Ui_PowerOn_Backlight();						//update bklight time
	Ui_ReturnTo_SysMenu();

}

/********************************************************************************
key mode in system power off
********************************************************************************/
void Ui_ModeS_In_SysMenu_PowerOff(void)
{
	if(uc_menu_cur_idx == 1)							   	//idle power off
	{
		uc_window_no = WINDOW_SYS_MENU_POWEROFF_IDLE;
		uc_menu_cur_idx = uc_idle_poweroff_level + 1;	
		uc_menu_total_num = 0x07;							//idle level is 7
		uc_lcd_dis_flg |= 0x58;						//set the bit2,bit4 for dis string
		uc_menu_top_idx = 0;
		Ui_Init_TopIdx();
	}
	else if(uc_menu_cur_idx == 2)															//sleep power off
	{
		uc_window_no = WINDOW_SYS_MENU_POWEROFF_SLEEP;
		uc_menu_cur_idx = uc_sleep_poweroff_level + 1;		
		uc_menu_total_num = 0x0d;							//sleep level is 13
		uc_lcd_dis_flg |= 0x58;						//set the bit2,bit4 for dis string
		uc_menu_top_idx = 0;
		Ui_Init_TopIdx();
	}
	else
	{
		Ui_ReturnTo_SysMenu();
	}
}

/********************************************************************************
key mode in system power off idle
********************************************************************************/
void Ui_ModeS_In_SysMenu_PowerOff_Idle(void)
{
	uc_idle_poweroff_level = uc_menu_cur_idx - 1;
	ui_idle_poweroff_time = Ui_Idle_Time_Table[uc_idle_poweroff_level];
	uc_menu_cur_idx = 1;							//init menu index and total item

	Ui_ReturnTo_SysMenu_PowerOff();
}

/********************************************************************************
key mode in system power off sleep
********************************************************************************/
void Ui_ModeS_In_SysMenu_PowerOff_Sleep(void)
{
	uc_sleep_poweroff_level = uc_menu_cur_idx - 1;
	ui_sleep_poweroff_time = Ui_Sleep_Time_Table[uc_sleep_poweroff_level];
	uc_menu_cur_idx = 2;							//init menu index and total item

 	Ui_ReturnTo_SysMenu_PowerOff();
}

/********************************************************************************
key mode in system menu replay
********************************************************************************/
void Ui_ModeS_In_SysMenu_Replay(void)
{
	if(uc_menu_cur_idx==1)		 	
	{
  		uc_mp3_replay_mode=0;	   	//auto	  
	}
	else							//manual
	{
		uc_mp3_replay_mode=1; 	
  	}
	Ui_ReturnTo_SysMenu();
}

/********************************************************************************
key mode in system menu amt
********************************************************************************/
void Ui_ModeS_In_SysMenu_AMT(void)
{
	if(uc_menu_cur_idx == 1)
	{
		uc_amt_onoff_flg = 1;
	}
	else
	{
		uc_amt_onoff_flg = 0;
	}
	
	Ui_ReturnTo_SysMenu();
}

/********************************************************************************
key mode in system contrast
********************************************************************************/
void Ui_ModeS_In_SysMenu_Contrast(void)
{
	uc_bklight_grade = uc_menu_cur_idx;
	
	Ui_ReturnTo_SysMenu();
}

/********************************************************************************
key mode in system menu language
********************************************************************************/
void Ui_ModeS_In_SysMenu_Language(void)
{
	uc_language_idx = uc_menu_cur_idx - 1;
	Ui_ReturnTo_SysMenu();
}

/********************************************************************************
key mode in system memory or version
********************************************************************************/
void Ui_ModeS_In_SysMenu_Product_Info(void)
{
	Ui_ReturnTo_SysMenu();
}

/********************************************************************************
key mode in system factory
********************************************************************************/
void Ui_ModeS_In_SysMenu_Factory(void)
{
	if(uc_menu_cur_idx == 1)
	{
		Ap_Restore_FactoryPara();
		ui_idle_poweroff_time = Ui_Idle_Time_Table[uc_idle_poweroff_level];
	}

	Ui_ReturnTo_SysMenu();
}



/*******************************************************************************
Function: Ui_ModeS_In_OnLine()
Description: select connected type of USB(modify USB reserved area) 
*******************************************************************************/
void Ui_ModeS_In_OnLine(void)
{
	uc_disk_state = uc_menu_cur_idx;
	Ui_ReturnTo_SysMenu();
}

#endif