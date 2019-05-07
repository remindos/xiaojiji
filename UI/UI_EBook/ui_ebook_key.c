/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_ebook_key.c
Creator: zj.zong					Date: 2009-05-02
Description: process key event
Others:
Version: V0.1
History:
	V0.1	2009-05-02		  zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_EBOOK_PROCESS_KEY_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include ".\UI\UI_EBook\ui_ebook_header.h"
#include ".\UI\UI_EBook\ui_ebook_var_ext.h"

extern void	Ui_PowerOn_Backlight(void);
extern void Ui_SetLcdFlag_0xfd(void);
extern void Ui_Display_StringDeleting(void);
extern void Ui_DisplayEbook_Loading(void);
extern void Ui_Check_Del_EbookMark(void);
extern void AP_InitPlay_Ebook(void);
extern void Ap_PowerOff_MP3(void);
extern void Ap_SwitchTo_MainMenu(void);
extern void Api_Key_Mapping(void);
extern void Api_DynamicLoad_Ebook_Target(uc8 code_idx);
extern bit FS_Fopen_Appointed(ui16 file_idx);
extern void Api_LcdDriver_Clr_AllPage(void);
extern void Api_LcdDriver_Clr_2345Page(void);
extern void	Api_LcdDriver_ClrOne_Page(uc8 page_idx);
extern void Ap_Exit_EbookMode(void);
extern void Api_DynamicLoad_CodeMpm1_8kExecute(void);
extern void Api_DynamicLoad_CodeCpm(void);
extern void Ui_Display_NoContent(void);
extern void	UI_ReturnTo_Browser(void);
extern void Ui_ReturnTo_MenuSD(void);
extern void Ui_Delete_Return_FlahsSD(void);
extern void	Ui_Display_DrawSD(void);
extern void	Ui_Display_NoFile(void);
extern void Ui_Display_MSWP(void);
extern void Ui_Init_TopIdx(void);
extern void Ui_Ebook_Return_FlahsSD(void);
extern void Ap_Check_SDExist(void);
extern void Ui_ReturnTo_MenuFlash(void);


void Ui_PlayS_InBrowser(void);
void Ui_SelNext_Page_InEbook(void);
void Ui_SelPrevious_Page_InEbook(void);
void Ui_SelNext_InBrowser(void);
void Ui_SelPrevious_InBrowser(void);
void Ui_SelNext_Menu(void);
void Ui_SelPrevious_Menu(void);
void Ui_ExitFolder_InBrowser(void);
void Ui_ModeS_InEbook(void);
void Ui_EnterFolder_InBrowser(void);
void Ui_Enter_DeleteMenu(void);
void Ui_ModeS_In_MenuDeleteAll(void);
void Ui_ModeS_InMenu(void);
void Ui_ModeS_InMenuSD(void);



/*******************************************************************************
Function: process_evt_key()
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
	uc_ebook_play_gap_cnt = 0;					//clr counter
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
		Ui_PowerOn_Backlight();					//restore backlight
		switch(uc_key_value)
		{
			case KEY_PLAY_S:
				 switch(uc_window_no)
				 {	 
					case WINDOW_EBOOK_BROWSER:
						Ui_PlayS_InBrowser();			 //open ebook file
						break;

					case WINDOW_EBOOK:
						if(uc_ebook_play_mode == 1)		 //auto change into manual
						{
							uc_ebook_play_mode =0;
							uc_ebook_play_gap=0;
						}
						else						   	 //manual change into auto
						{
	   						uc_ebook_play_mode =1;
							uc_ebook_play_gap= 2;
						}
						break;

					case WINDOW_EBOOK_DELETE:	
						UI_ReturnTo_Browser();	  		//cancel delete
						break;

					case WINDOW_EBOOK_DELETE_ALL:
						Ui_Delete_Return_FlahsSD();    //allen  delete all menu
						break;
					default:
						break;
						
				  }
				  break;

			case KEY_PLAY_L:
				if(b_play_poweron_flg == 0)
				{										  //power off
					uc_poweroff_type = POWEROFF_NORMAL;
					Ap_PowerOff_MP3();
				
				}
				break;
			
			case KEY_NEXT_S:
			case KEY_NEXT_L:
			case KEY_NEXT_LC:
			   	 switch(uc_window_no)
				 {
				 	case WINDOW_EBOOK:	
				 		if(uc_ebook_play_mode == 0)		  //play next page only in manual mode
						{
					  	 	Ui_SelNext_Page_InEbook();
					   	}
						break;

					case WINDOW_EBOOK_BROWSER:
						Ui_SelNext_InBrowser();			 //select next file
						break;

					default:
						Ui_SelNext_Menu();			   	 //select next menu
						break;

				 }
				 break;
	

			case KEY_LAST_S:
			case KEY_LAST_L:
			case KEY_LAST_LC:
				 switch(uc_window_no)
				 {
				 	case WINDOW_EBOOK:
						if(uc_ebook_play_mode == 0)     //play previous page only in manual mode
					   	{
					  	 	Ui_SelPrevious_Page_InEbook();
					  	}
						break;

					case WINDOW_EBOOK_BROWSER:
						Ui_SelPrevious_InBrowser();		 //select previous file
						break;

					default:
						Ui_SelPrevious_Menu();		     //select previous menu
						break;

				 }
				break;

			case KEY_VOL_S:
				switch(uc_window_no)
				{

					case WINDOW_EBOOK_BROWSER:
						Ui_ExitFolder_InBrowser();
						break;

					case WINDOW_EBOOK_DELETE:
						UI_ReturnTo_Browser();
						break;

					case WINDOW_EBOOK_DELETE_ALL:
						Ui_Delete_Return_FlahsSD();
						break;

					default:
						break;
				}
				break;
			
			case KEY_VOL_L:
				switch(uc_window_no)
				{
					case WINDOW_EBOOK:
						Ui_Ebook_Return_FlahsSD();
						break;
						
					default:
						break; 
				}
				break; 
			
		
			case KEY_REC_L:
				if(((uc_user_option2&0x20) != 0)&&((uc_user_option2&0x08) == 0))
				{
					Ap_Exit_EbookMode();

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
				 	case WINDOW_EBOOK:
					 	Ui_ModeS_InEbook();				  //exit current file
						break;

					case WINDOW_EBOOK_BROWSER:
					 	Ui_EnterFolder_InBrowser();			  //enter folder or delete window
						break;

					case WINDOW_EBOOK_MENU_FLASH:           //allen  
						Ui_ModeS_InMenu();
						break;
						
					case WINDOW_EBOOK_MENU_SD:         //allen
						Ui_ModeS_InMenuSD();
						break;


					case WINDOW_EBOOK_DELETE_ALL:
					Ui_ModeS_In_MenuDeleteAll();      //allen
					break;

					default:
                    Ui_Enter_DeleteMenu();		  //delete file
						break;
													
				 }
				break;

			case KEY_MODE_L:
				Ap_SwitchTo_MainMenu();			  //return to main menu
				break;

			default:
				break;
		}
/*	}
	else 
	{
		Ui_PowerOn_Backlight();
		uc_lcd_dis_flg |= 0x02;							  //set bit1, display hold pic	
	}  */
	ui_sys_event |= EVT_DISPLAY;		                  //set the display event
}


/*******************************************************************************
Function: Ui_PlayS_InBrowser()
Description: if the selected item is ebook file, and play the file
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/

void Ui_PlayS_InBrowser(void)							//play the current file
{
	if((uc_fs_subdir_stk_pt > 0) && (ui_fb_sel_item_idx == 0))
	{
		return;
	}

	if(uc_fb_sel_item_type == 1)					//uc_fs_item_type: 1-file, 2-folder
	{
		ui_ebook_tmp_var = ui_fb_sel_item_idx - ui_fs_subdir_num;
		if(uc_fs_subdir_stk_pt > 0)						//subdir have "dummy folder"
		{
			ui_ebook_tmp_var--;
		}
		
		FS_Fopen_Appointed(ui_ebook_tmp_var);			//open current ebook file according to "ebook index"
		Ui_DisplayEbook_Loading();						//display "Loading..."
		AP_InitPlay_Ebook();
		if(ul_fs_file_length == 0)
		{
			Ui_Display_NoContent();
			UI_ReturnTo_Browser();
		}
		else
		{
			uc_lcd_dis_flg = 0x44; 
			uc_window_no = WINDOW_EBOOK;
		}
	}
}


/*******************************************************************************
Function: 1.Ui_SelNext_Page_InEbook()
		  2.Ui_SelNext_InBrowser()
		  3.Ui_SelNext_Menu()
Description: function of press key next
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/

void Ui_SelNext_Page_InEbook(void)				   		//select next page
{
	if(ui_ebook_page_idx < ui_ebook_page_num)
	{
		ui_ebook_page_idx++;
		uc_lcd_dis_flg |= 0x04;
		ui_sys_event |= EVT_DISPLAY;		                  //set the display event
	}
}

/*******************************************************************************
select the next item in browser
********************************************************************************/
void Ui_SelNext_InBrowser(void)
{
	ui16 tmp_browser_top_item_idx, tmp_fs_item_num_indir;
	
	if(b_ebook_nofile_flg == 1)							//no file return
	{
		return;
	}
	
	tmp_fs_item_num_indir = ui_fs_subdir_num + ui_fs_special_file_num_indir;
	if(uc_fs_subdir_stk_pt > 0)	
	{
		tmp_fs_item_num_indir++;						//has "dummy folder" in SubDir
	}
		
	if(tmp_fs_item_num_indir > 0)
	{
		tmp_browser_top_item_idx = ui_fb_top_item_idx;
		
		ui_fb_sel_item_idx++;
		if(ui_fb_sel_item_idx < tmp_fs_item_num_indir)
		{
			if(ui_fb_sel_item_idx >= (ui_fb_top_item_idx + Dis_BrowserItem_MaxNum))
			{
				ui_fb_top_item_idx++;
			}
		}
		else
		{
			ui_fb_sel_item_idx = 0;
			ui_fb_top_item_idx = 0;
		}
		
		if(tmp_browser_top_item_idx != ui_fb_top_item_idx)
		{
			uc_lcd_dis_flg |= 0x0c;			
		}
		else
		{
			uc_lcd_dis_flg |= 0x08;				//set the lcd display flag
		}
		Api_LcdDriver_Clr_2345Page();			//clr screen
		Api_LcdDriver_ClrOne_Page(6);
		Api_LcdDriver_ClrOne_Page(7);
	}
}

/***********************************************************************
Select the next menu (uc_menu_cur_idx+1)
input:uc_menu_cur_idx,uc_menu_total_num
***********************************************************************/
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
	uc_lcd_dis_flg= 0xfd;					//set the bit4

}
/*******************************************************************************
Function: Ui_ModeS_InMenu()
Description: short press key mode in stop menu (only has flash)
Calls:
Global:uc_menu_cur_idx
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ui_ModeS_InMenu(void)
{
	uc_menu_cur_idx_bk = uc_menu_cur_idx;
	switch(uc_menu_cur_idx)
	{
		case 1:
			uc_window_no = WINDOW_EBOOK_BROWSER;
			if(uc_fs_subdir_stk_pt > 0)
			{
				ui_fb_sel_item_idx = tmp_file_idx_indir + ui_fs_subdir_num;	   //
			}
	
			if(ui_fb_sel_item_idx < Dis_BrowserItem_MaxNum)
			{
				ui_fb_top_item_idx = 0;
			}
			else
			{
				ui_fb_top_item_idx = ui_fb_sel_item_idx - Dis_BrowserItem_MaxNum + 1;
			}
			Ui_SetLcdFlag_0xfd();
			break;

	/*	case 2:
			uc_window_no = WINDOW_EBOOK_SETTING;
			uc_menu_cur_idx = uc_ebook_play_mode + 1;	//init menu index and total item
			uc_menu_total_num = Ebook_MenuSetting_Num;
			b_lcd_clrall_flg = 1;
			uc_lcd_dis_flg = 0xfc;
			uc_lcd_y_saddr_bk = 0xff;
			break;	*/

		case 2:
			uc_window_no = WINDOW_EBOOK_DELETE_ALL;		
			uc_menu_cur_idx = 2;
			uc_menu_total_num = 2;
			Ui_SetLcdFlag_0xfd();

		if(b_ebook_nofile_flg == 0)
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
			Api_DynamicLoad_Ebook_Target(CODE_EBOOK_7_GET_NAME_IDX);
		}
			break;	 

		default:
			Ap_SwitchTo_MainMenu();
			break;
	}
}


/*******************************************************************************
Function: Ui_ModeS_InMenuSD()
Description: short press key mode in stop menu has SD card
*******************************************************************************/
void Ui_ModeS_InMenuSD(void)
{  	
	uc_menu_cur_idx_bk = uc_menu_cur_idx;
	switch(uc_menu_cur_idx)	
	{
		case 1:			
			if(b_sd_flash_flg == 0)
			{
				b_sd_flash_flg = 1;
				uc_dynamicload_idx = 30;		//FS_Finit
				Api_DynamicLoad_Ebook_Target(CODE_EBOOK_8_SWITCHCARD_IDX);
				if(uc_msi_status != 0)
				{
					Ui_Display_DrawSD();
					Ui_ReturnTo_MenuSD();
					return;	
				}
				goto Label_EnterRoot;
			}
			UI_ReturnTo_Browser();			
			break;

		case 2:			
			if(b_sd_flash_flg == 1)
			{
				b_sd_flash_flg = 0;
				uc_dynamicload_idx = 30;		//FS_Finit
				Api_DynamicLoad_Ebook_Target(CODE_EBOOK_8_SWITCHCARD_IDX);
Label_EnterRoot:
				uc_dynamicload_idx = 41;		//Scan All Disk(Flash or SD)
				Api_DynamicLoad_Ebook_Target(CODE_EBOOK_1_INIT_IDX);
				uc_dynamicload_idx = 32;		//enter root dir for display browser
				Api_DynamicLoad_Ebook_Target(CODE_EBOOK_8_SWITCHCARD_IDX);
			}
			UI_ReturnTo_Browser();
			break;
			
	/*	case 3:
			uc_window_no = WINDOW_EBOOK_SETTING;
			uc_menu_title_idx = TITLE_STR_EBOOK_MENU_IDX;
			uc_menu_cur_idx = uc_ebook_play_mode + 1;	//init menu index and total item
			uc_menu_total_num = Ebook_MenuSetting_Num;
			b_lcd_clrall_flg = 1;
			uc_lcd_dis_flg = 0xfc;
			uc_lcd_y_saddr_bk = 0xff;
			break;	*/

		case 3:
			uc_window_no = WINDOW_EBOOK_DELETE_ALL;		
			uc_menu_cur_idx = 2;
			uc_menu_total_num = 2;
			Ui_SetLcdFlag_0xfd();

		if(b_ebook_nofile_flg == 0)
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
			Api_DynamicLoad_Ebook_Target(CODE_EBOOK_7_GET_NAME_IDX);
			}
			else
			{
				Ui_Display_NoFile();
				uc_menu_cur_idx_bk = 3;
				Ui_ReturnTo_MenuSD();	
			}			
			break;	

		default:
			Ap_SwitchTo_MainMenu();
			break;	
	}
}


/*******************************************************************************
Function: 1.Ui_SelPrevious_Page_InEbook()
		  2.Ui_SelPrevious_InBrowser()
		  3.Ui_SelPrevious_Menu()
Description: function of press key last
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/

void Ui_SelPrevious_Page_InEbook(void)
{
	if(ui_ebook_page_idx > 1)
	{
		ui_ebook_page_idx--;
		
		uc_lcd_dis_flg |= 0x04;
		ui_sys_event |= EVT_DISPLAY;
	}
}

/*******************************************************************************
select the previous item in browser
********************************************************************************/
void Ui_SelPrevious_InBrowser(void)
{
	ui16 tmp_browser_top_item_idx, tmp_fs_item_num_indir;
	
	if(b_ebook_nofile_flg == 1)						   //no file return
	{
		return;
	}
	
	tmp_fs_item_num_indir = ui_fs_subdir_num + ui_fs_special_file_num_indir;
	if(uc_fs_subdir_stk_pt > 0)	
	{
		tmp_fs_item_num_indir++;						//has "dummy folder" in SubDir
	}
	
	if(tmp_fs_item_num_indir > 0)
	{
		tmp_browser_top_item_idx = ui_fb_top_item_idx;

		if(ui_fb_sel_item_idx == 0)
		{
			ui_fb_sel_item_idx = tmp_fs_item_num_indir - 1;
			if(tmp_fs_item_num_indir >= Dis_BrowserItem_MaxNum)
			{
				ui_fb_top_item_idx = ui_fb_sel_item_idx - Dis_BrowserItem_MaxNum + 1;
			}
			else
			{
				ui_fb_top_item_idx = 0;
			}
		}
		else
		{
			ui_fb_sel_item_idx--;
		}
	
		if(ui_fb_top_item_idx > ui_fb_sel_item_idx)
		{
			ui_fb_top_item_idx = ui_fb_sel_item_idx;
		}
		
		if(tmp_browser_top_item_idx != ui_fb_top_item_idx)
		{
			uc_lcd_dis_flg |= 0x0c;							//0x0c		//clr screen
		}
		else
		{
			uc_lcd_dis_flg |= 0x08;							//0x18		//set the lcd display flag
		}
		Api_LcdDriver_Clr_2345Page();			//clr screen
		Api_LcdDriver_ClrOne_Page(6);
		Api_LcdDriver_ClrOne_Page(7);
	}
}

/***********************************************************************
Select the last menu (uc_menu_cur_idx-1)
***********************************************************************/
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
	uc_lcd_dis_flg= 0xfd;								//set the bit4
}

/*******************************************************************************
Function: 1.Ui_ExitFolder_InBrowser()
Description: function of press key vol
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_ExitFolder_InBrowser(void)
{
	if(uc_fs_subdir_stk_pt == 0)
	{
		Ap_Check_SDExist();
		if(b_sd_exist_flg == 0)
		{
			uc_menu_cur_idx_bk = 1;
			Ui_ReturnTo_MenuFlash();
		}
		else
		{
			if(b_sd_flash_flg == 1)
			{
				uc_menu_cur_idx_bk = 1;	
			}
			else
			{
				uc_menu_cur_idx_bk = 2;
			}			
			Ui_ReturnTo_MenuSD();	
		}
	}
	
	if(uc_fs_subdir_stk_pt > 0)
	{
		uc_dynamicload_idx = 11;
		Api_DynamicLoad_Ebook_Target(CODE_EBOOK_6_GET_NUM_IDX);	//exit folder and get folder&file number

		uc_lcd_dis_flg |= 0x0c;
		Api_LcdDriver_Clr_2345Page();
		Api_LcdDriver_ClrOne_Page(6);
		Api_LcdDriver_ClrOne_Page(7);
	}
}

/*******************************************************************************
Function: 1.Ui_ModeS_InEbook()
		  2.Ui_EnterFolder_InBrowser()
		  3.Ui_Enter_DeleteMenu()
Description: function of press key mode
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_ModeS_InEbook(void)
{
	if((uc_ebookmark_flag & 0x08)!=0)				//current save EbookMark1
	{	 											   
		ui_ebook_mark1 = ui_ebook_page_idx;	  		//save EbookMark1 page idx
  	}
	else if((uc_ebookmark_flag & 0x10)!=0)			//current save EbookMark2
	{
	  	ui_ebook_mark2 = ui_ebook_page_idx;	  		//save EbookMark2 page idx
  	}
	else if((uc_ebookmark_flag & 0x20)!=0)			//current save EbookMark3
	{
	 	ui_ebook_mark3 = ui_ebook_page_idx;	  		//save EbookMark3 page idx
	}
	UI_ReturnTo_Browser();
}

/*******************************************************************************
Function: enter folder or enter deleting file window
********************************************************************************/
void Ui_EnterFolder_InBrowser(void)
{
	if(b_ebook_nofile_flg == 1)							 	//return to main menu
	{
		Ap_SwitchTo_MainMenu();
	}
	
	if(ui_fb_sel_item_idx == 0)
	{
		if(uc_fs_subdir_stk_pt == 0)						//have not "dummy folder" in ROOT
		{
			goto enter_folder;
		}
		else
		{
			Ui_ExitFolder_InBrowser();							//the first folder is dummy
		}
	}
	else
	{
enter_folder:
		if(uc_fb_sel_item_type == 1)		  		    //uc_fs_item_type: 1-file
		{
			ui_ebook_tmp_var = ui_fb_sel_item_idx - ui_fs_subdir_num;
			if(uc_fs_subdir_stk_pt > 0)			           //subdir have "dummy folder"
			{
				ui_ebook_tmp_var--;
			}

			FS_Fopen_Appointed(ui_ebook_tmp_var);	
			uc_window_no = WINDOW_EBOOK_DELETE;
			uc_menu_cur_idx = 2;
			uc_menu_total_num = 2;	
			uc_lcd_dis_flg |= 0x11;						//set the bit4
			b_lcd_clrall_flg = 1;
		}
		else if(uc_fb_sel_item_type == 2)			  //uc_fs_item_type:  2-folder
		{
			if(uc_fs_subdir_stk_pt < FS_MAX_SUBDIR_DEPTH)
			{
				uc_dynamicload_idx = 10;
				Api_DynamicLoad_Ebook_Target(CODE_EBOOK_6_GET_NUM_IDX);		             //enter folder and get folder&file number		
			}
	  	 	uc_lcd_dis_flg |= 0x0c;
			Api_LcdDriver_Clr_2345Page();
			Api_LcdDriver_ClrOne_Page(6);
			Api_LcdDriver_ClrOne_Page(7);
	   	}
	}
}

/*******************************************************************************
Function: delete file and display
********************************************************************************/

void Ui_Enter_DeleteMenu(void)
{
	if(uc_menu_cur_idx==1)							//delete file
	{
	   	if((b_sd_flash_flg == 1) && ((uc_user_option1 & 0x10) != 0) && ((MR_MSCON4 & 0x40) == 0))
		{
			Ui_Display_MSWP();	
		}
	else	
	    {
		uc_fb_sel_item_type=0;
		Ui_Display_StringDeleting();				//display delete notice "deleting..."
		Ui_Check_Del_EbookMark();
		uc_dynamicload_idx = 20;
		Api_DynamicLoad_Ebook_Target(CODE_EBOOK_5_DEL_IDX);	//del one file
		
		}
	}

	UI_ReturnTo_Browser();					//return to mp3 file browser
}


/*******************************************************************************
Function: Ui_ModeS_In_MenuDeleteAll()
Description: delete all file in browser
Calls:
Global:uc_menu_cur_idx
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ui_ModeS_In_MenuDeleteAll(void)
{
	if(uc_menu_cur_idx == 1)						//delete file
	{
		if((b_sd_flash_flg == 1) && ((uc_user_option1 & 0x10) != 0) && ((MR_MSCON4 & 0x40) == 0))
		{
			Ui_Display_MSWP();
			goto ExitDeleteInterface;	
		}
		else
		{
	     	uc_fb_sel_item_type=0;
			Ui_Display_StringDeleting();						//display delete notice "deleting..."
			uc_dynamicload_idx	= 21;
			Api_DynamicLoad_Ebook_Target(CODE_EBOOK_5_DEL_IDX);//del all file
		
			if(b_ebook_nofile_flg == 1)
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
			else
			{
				uc_dynamicload_idx = 41;
				Api_DynamicLoad_Ebook_Target(CODE_EBOOK_1_INIT_IDX);
				Ui_Delete_Return_FlahsSD();			
			}
		}		
	}
	else
	{
ExitDeleteInterface:	
		Ui_Delete_Return_FlahsSD();	
	}
}  


#endif
