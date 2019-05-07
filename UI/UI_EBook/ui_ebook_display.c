/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_ebook_display.c
Creator: zj.zong					Date: 2009-05-04
Description: display function
Others:
Version: V0.1
History:
	V0.1	2009-05-04		   zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_EBOOK_PROCESS_DISPLAY_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include ".\UI\UI_EBook\ui_ebook_header.h"
#include ".\UI\UI_EBook\ui_ebook_var_ext.h"


extern void Ui_Display_HoldPic(void);

extern void Ap_FillDecbuf_WithUnicode(void);
extern void Ap_Get_Browser_ItemName();
extern void Ui_Display_Battery(void);
extern void Api_LcdDriver_Clr_AllPage(void);

extern void Api_Delay_1ms(uc8 timer_cnt);
extern void Api_Dis_FileBrowser_Reverse(void);
extern void Api_Display_Picture(ui16 ui_pic_idx);
extern void Api_Display_Unicode(ui16 unicode_idx);
extern void Api_DynamicLoad_Ebook_Target(uc8 code_idx);
extern void Api_Display_8x6_ZiKu(uc8 ziku_idx);
extern void Api_LcdDriver_Clr_2345Page(void);
extern void	Api_LcdDriver_ClrOne_Page(uc8 page_idx);


void Ui_Display_Ebook(void);
void Ui_DisplayEbook_OnePage(void);
void Ui_Display_EbookBrowser(void);
void Ui_Display_EbookDel(void);
void Ui_Display_BrowserItem(void);
void Ui_Display_DummyFolder(void);
void Ui_DisplayBrowser_ItemFlag(void);
void Ui_DisplayBrowser_ItemName(void);
void Ui_DisplayEbook_Loading(void);
//void Ui_Display_InsertCard(void);
void Ui_Display_DrawSD(void);
void Ui_Display_Menu_Item_Idx(uc8 numr_idx,uc8 total_num);
//void Ui_Display_InsertCard(void);	
void Ui_Display_MenuCommon(void);
void Ui_Display_MenuString(void);
void Ui_Display_SelectMenuFlag(void);
void Ui_Display_MSWP(void);
void Ui_Display_DelAllMenu(void);


code ui16 MenuStr_Table_Ebook[]=
{
	0x0000,							//NULL EBOOK
	MENU_STR_EBOOK_MENU_SD_IDX,	    //menu sd	
	MENU_STR_EBOOK_MENU_FLASH_IDX,  //menu flash
	0x0000,							//WINDOW_EBOOK_MENU_RPL_TIME
	0x0000,							//WINDOW_EBOOK_MENU_RPL_GAP
	0x0000,							//WINDOW_EBOOK_VOLUME
	0x0000,							//WINDOW_EBOOK_MENU_DEL
	0x0000,							//WINDOW_EBOOK_MENU_DELALL
	0x0000,							//WINDOW_EBOOK_MENU_BROWSER
	
};

/*******************************************************************************
Function: Ui_ProcessEvt_Display()
Description: display
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
		if(b_lcd_clrall_flg == 1)				//clr LCD all page
		{
			Api_LcdDriver_Clr_AllPage();
		}
		switch(uc_window_no)
		{
		 	case WINDOW_EBOOK:
				Ui_Display_Ebook();					//display one page ebook, not display battery and hold
				break;
	
			case WINDOW_EBOOK_BROWSER:
			 	Ui_Display_EbookBrowser();
					break;
			
			case WINDOW_EBOOK_DELETE:	
		    	Ui_Display_EbookDel();			//display delete window
		           break;

			case WINDOW_EBOOK_DELETE_ALL:	
			     Ui_Display_DelAllMenu(); 			//display delete window
		           break;		

			default:
				 Ui_Display_MenuCommon();
				 break; 
		}
//	}
	b_lcd_clrall_flg = 0;
	uc_lcd_dis_flg = 0;
}

/*******************************************************************************
Function: 1.Ui_Display_Ebook()
		  2.Ui_DisplayEbook_OnePage
Description: fill and display one page ebook
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Display_Ebook(void)
{
	if((uc_lcd_dis_flg & BIT2_MASK) != 0)
	{
		if(ui_ebook_page_num != 0)
		{
			Ap_FillDecbuf_WithUnicode();  //fill decbuf with unicode, be used to display one page ebook at least
		}

		Ui_DisplayEbook_OnePage();		  //display one page of unicode
	}
	if((uc_lcd_dis_flg & BIT6_MASK) != 0)
	{
		uc_lcd_page_num = 0;
		uc_lcd_column_num = 2;
		Ui_DisplayBrowser_ItemName();					//display file name
	}
}

/********************************************************************************
 display one page ebook
********************************************************************************/
void Ui_DisplayEbook_OnePage(void)
{
	ui16 tmp_disbuf_pt = 0;
	uc8 tmp_line_num = 0;
	b_lcd_reverse_flg = 0;
	uc_ebook_play_gap_cnt = 0;						//for auto play ebook
	Api_LcdDriver_Clr_2345Page();
	Api_LcdDriver_ClrOne_Page(6);
	Api_LcdDriver_ClrOne_Page(7);
	if(ui_ebook_page_num == 0)
	{
		return;
	}	
	uc_lcd_page_num = 2;
	uc_lcd_column_num = 0;
	while(1)
	{		
		if((ui_dbuf0[tmp_disbuf_pt]==0x000d) && (ui_dbuf0[tmp_disbuf_pt+1])==0x000a)	//"Enter", switch to the next line
		{
			tmp_disbuf_pt += 2;
			tmp_line_num++;
			uc_lcd_column_num = 0;
			uc_lcd_page_num += 2;
		}
		else
		{
			if(uc_lcd_column_num > DIS_EBOOK_X_END_ADDR)
			{
				tmp_line_num++;
				uc_lcd_column_num = 0;
				uc_lcd_page_num += 2;
			}
			
			if(tmp_line_num >= DIS_EBOOK_LINE_MAX_NUM)
			{
				return;
			}
			
			Api_Display_Unicode(ui_dbuf0[tmp_disbuf_pt]);
			tmp_disbuf_pt++;
		}
	}
}

/*******************************************************************************
Function: 1.Ui_Display_EbookBrowser()
		  2.Ui_Display_BrowserItem()
		  3.Ui_DisplayBrowser_ItemFlag()
		  4.Ui_DisplayBrowser_ItemName()
		  5.Ui_Display_DummyFolder()
Description: bit3--display folder and file
                1. firstly, display one dummy folder(be used for return to previous layer dir)
                2. secondly, display all folders
                3. finally, display all files
                4. displayed folder and file max number is Dis_BrowserItem_MaxNum
             bit4--display selected lines and progress bar
Calls:
Global:ui_fb_top_item_idx, ui_fb_sel_item_idx
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Display_EbookBrowser(void)
{
	if((uc_lcd_dis_flg & BIT0_MASK) != 0)	//display battery is common func
	{
		Ui_Display_Battery();
	}

	if((uc_lcd_dis_flg & BIT2_MASK) != 0)
	{
		/*if havn't file and folder in root, and display "no file"*/
		if((uc_fs_subdir_stk_pt == 0) && ((ui_fs_subdir_num + ui_fs_special_file_num_indir) == 0))	
		{
			Api_LcdDriver_Clr_AllPage();	

			b_ebook_nofile_flg = 1;
 			uc_lcd_page_num = 3;
			uc_lcd_column_num = 0;
        	if(b_sd_flash_flg == 1)		//SD/MMC care exsit, and select flash
         	{
           	Api_Display_Picture(NORM_STR_SD_NOFILE_IDX + ALL_MENU_NUMBER * uc_language_idx);
	        }
        	else
	        { 	
	        Api_Display_Picture(NORM_STR_FLASH_NOFILE_IDX + ALL_MENU_NUMBER * uc_language_idx);
	         }	
			 	return;
		}
	}

	if((uc_lcd_dis_flg & BIT3_MASK) != 0)
	{
		Ui_Display_BrowserItem();
	}
}


/*******************************************************************************
if mp3 file&folder number is great than DisMenu_MaxNum, and display browser bar,
otherwise, not display browser bar
********************************************************************************/
void Ui_Display_BrowserItem(void)
{
	b_browser_fir_getname_flg = 0;
	uc_fb_sel_item_type = 0;
	
	ui_ebook_tmp_var = ui_fs_subdir_num + ui_fs_special_file_num_indir;	//folder + txt file 
	if(uc_fs_subdir_stk_pt > 0)
	{
		ui_ebook_tmp_var++;		//file + folder + dummy folder
	}

	if(ui_ebook_tmp_var >= Dis_BrowserItem_MaxNum)	//init item number that is displayed on page
	{
		uc_fb_tmp_dir_max_item = Dis_BrowserItem_MaxNum;
	}
	else
	{
		uc_fb_tmp_dir_max_item = (uc8)ui_ebook_tmp_var;
	}
	uc_lcd_page_num = 2;
	for(uc_fb_dis_cnt = 0; uc_fb_dis_cnt < uc_fb_tmp_dir_max_item; uc_fb_dis_cnt++)
	{
		if((ui_fb_top_item_idx + uc_fb_dis_cnt) == 0)	//display dummy folder
		{
			if(uc_fs_subdir_stk_pt > 0)					//have not "dummy folder" in ROOT
			{
				if(ui_fb_sel_item_idx == 0)
				{
					b_lcd_reverse_flg = 1;
				}
				else
				{
					b_lcd_reverse_flg = 0;
				}
				
				Ui_Display_DummyFolder();
			}
			else
			{
				goto dis_other_item;
			}
		}
		else												//display file or folder
		{
dis_other_item:
			ui_fb_cur_item_idx = ui_fb_top_item_idx + uc_fb_dis_cnt;
			
			if(ui_fb_sel_item_idx == ui_fb_cur_item_idx)
			{
				b_lcd_reverse_flg = 1;						//check current item that is selected
			}
			else
			{
				b_lcd_reverse_flg = 0;
			}
			if(uc_fs_subdir_stk_pt > 0)						//if it's subdir, and ui_browser_cur_item_idx-1(dummy folder)
			{
				ui_fb_cur_item_idx--;
			}

			if(ui_fb_cur_item_idx < ui_fs_subdir_num)	//display folder
			{
				uc_dynamicload_idx = 2;			//get browser item name
				Api_DynamicLoad_Ebook_Target(CODE_EBOOK_7_GET_NAME_IDX);
			}
			else											//display file
			{
				uc_dynamicload_idx = 1;			//get browser item name
				Api_DynamicLoad_Ebook_Target(CODE_EBOOK_7_GET_NAME_IDX);
			}
								
			if(b_lcd_reverse_flg == 1)
			{
				uc_fb_sel_item_type = uc_fs_item_type;	//backup selected item type
			}
		  
		  	uc_lcd_column_num = 16;
	        uc_lcd_page_num_bk = uc_lcd_page_num;
			Ui_DisplayBrowser_ItemName();					//display file or folder name
			if(b_lcd_reverse_flg == 1)
			{
				Api_Dis_FileBrowser_Reverse();
			  	 b_lcd_reverse_flg = 0;
			}
		   
		    uc_lcd_page_num = uc_lcd_page_num_bk;
			uc_lcd_column_num = 0;
			Ui_DisplayBrowser_ItemFlag();					//display file or folder flag pic
		}
		uc_lcd_page_num = uc_lcd_page_num_bk + 2;
	}
	Ui_Display_Menu_Item_Idx(ui_fb_sel_item_idx + 1,ui_ebook_tmp_var);
}

/*******************************************************************************
Function: Ui_Display_MenuCommon()
Description: bit2--display menu title	
		      1. displayed menu max number is DisMenu_MaxNum
Calls:
Global:uc_menu_total_num, uc_menu_cur_idx
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Display_MenuCommon(void)
{
	if((uc_lcd_dis_flg & BIT3_MASK) != 0)
	{
		Ui_Display_MenuString();
	}

	if((uc_lcd_dis_flg & BIT4_MASK) != 0)
	{
		Ui_Display_Menu_Item_Idx(uc_menu_cur_idx,uc_menu_total_num);
	}
	if((uc_lcd_dis_flg & BIT6_MASK) != 0)
	{
		Ui_Display_SelectMenuFlag();
	}
}


/********************************************************************************/
void Ui_Display_SelectMenuFlag(void)
{
  	uc_lcd_column_num = 112;
	uc_lcd_page_num = uc_lcd_page_num_bk;
	Api_Display_Picture(PIC_SELECT_FLAG_IDX + uc_refresh_menu_num);
}
/********************************************************************************/

void Ui_Display_MenuString(void)
{
	uc8 i, j, tmp_max_num ,uc_lcd_page_num_tmp;
	
	if(uc_menu_total_num <= DisMenu_MaxNum)
	{
		tmp_max_num = uc_menu_total_num;
	}
	else
	{
		tmp_max_num = DisMenu_MaxNum;
	}
	
	uc_lcd_page_num = 2;
	uc_lcd_page_num_tmp = uc_lcd_page_num;

	for(i = uc_menu_top_idx, j = 0; j < tmp_max_num; i++, j++)
	{
		uc_lcd_column_num = 0;		   			    //display menu sel or not sel flg
	
		if(i == (uc_menu_cur_idx - 1))				//check displayed menu be selected?
		{
			uc_lcd_page_num_bk = uc_lcd_page_num;
			Api_Display_Picture(PIC_MENU_SEL_IDX);
			b_lcd_reverse_flg = 1;				
		}
		else
		{
			Api_Display_Picture(PIC_MENU_NOSEL_IDX);
			b_lcd_reverse_flg = 0;				
		}
		uc_lcd_page_num = uc_lcd_page_num_tmp;
		uc_lcd_column_num = 10;						//display menu string
		if(b_lcd_reverse_flg == 1)
		{
			Api_Dis_FileBrowser_Reverse();
		}
		uc_lcd_page_num = uc_lcd_page_num_tmp;
		Api_Display_Picture(i + MenuStr_Table_Ebook[uc_window_no] + ALL_MENU_NUMBER * uc_language_idx);
	
		b_lcd_reverse_flg = 0;				
		uc_lcd_page_num = uc_lcd_page_num + 1;
		uc_lcd_page_num_tmp = uc_lcd_page_num;
	}
}
/*******************************************************************************
display browser item idx
*******************************************************************************/
void Ui_Display_Menu_Item_Idx(uc8 numr_idx,uc8 total_num)										
{
	uc_lcd_page_num = 1;											
	uc_lcd_column_num = 37;	
									
	Api_Display_8x6_ZiKu(16);						//display "<<"	
	Api_Display_8x6_ZiKu(numr_idx/100%10);						
	Api_Display_8x6_ZiKu(numr_idx%100/10);						
	Api_Display_8x6_ZiKu(numr_idx%100%10);					
	Api_Display_8x6_ZiKu(15);						//display "/"		
	Api_Display_8x6_ZiKu(total_num/100%10);		
	Api_Display_8x6_ZiKu(total_num%100/10);		
	Api_Display_8x6_ZiKu(total_num%100%10);					
	Api_Display_8x6_ZiKu(17);						//display ">>"	
}

/*******************************************************************************
display browser item flag
*******************************************************************************/
void Ui_DisplayBrowser_ItemFlag(void)
{
	if(uc_fs_item_type == 1)						//1:txt file; 2:folder
	{
		Api_Display_Picture(PIC_FILEBROW_FILE_IDX);		//txt file flag
	}
	else
	{
		Api_Display_Picture(PIC_FILEBROW_FOLDER_IDX);
	}

	uc_lcd_page_num--;
}

/*******************************************************************************
display dummy folder
*******************************************************************************/
void Ui_Display_DummyFolder(void)
{
	uc_lcd_page_num_bk = uc_lcd_page_num;
 	uc_lcd_column_num = 16;				//display dummy folder ".."
	Api_Display_Unicode('.');
	Api_Display_Unicode('.');
	if(b_lcd_reverse_flg == 1)
	{
		Api_Dis_FileBrowser_Reverse();
		b_lcd_reverse_flg = 0;
	}
	uc_lcd_page_num = uc_lcd_page_num_bk;
	uc_lcd_column_num = 0;
	Api_Display_Picture(PIC_FILEBROW_FOLDER_IDX);
	uc_lcd_page_num = uc_lcd_page_num_bk;
}

/*******************************************************************************
display browser item name
*******************************************************************************/
void Ui_DisplayBrowser_ItemName(void)
{
	uc8 i = 0;

	while(1)
	{
		if(uc_lcd_column_num <= 112)
		{
			if(i < uc_fs_item_name_length)
			{
				Api_Display_Unicode(ui_fs_item_name[i]);
				i++;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
}

/*******************************************************************************
Function: 1.Ui_Display_EbookDel()
Description: display delete fill 
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Display_EbookDel(void)
{	

	if((uc_lcd_dis_flg & BIT4_MASK) != 0)
	{
		uc_lcd_page_num = 2;	
		uc_lcd_column_num = 0;
		Api_Display_Picture(NORM_STR_DELETE_FILE_IDX + ALL_MENU_NUMBER * uc_language_idx);	//display file 
		uc_lcd_page_num = 4;	
		uc_lcd_column_num = 0;
		Api_Display_Picture(PIC_NOTICE_YES_NO_IDX + uc_menu_cur_idx - 1);			 //display "YES" & "NO"

	}
}
/**************************************************************************************/
void Ui_Display_DelAllMenu(void)
{																				//no delete all
	if((uc_lcd_dis_flg & BIT3_MASK) != 0)
	{
		uc_lcd_page_num = 2;
		uc_lcd_column_num = 0;
		Api_Display_Picture(PIC_FILEBROW_FOLDER_IDX);
		
		uc_lcd_page_num = 2;
		uc_lcd_column_num = 16;
		if(b_fs_subdir_stk_pt == 1)
		{
			Api_Display_Picture(PIC_ROOT_NAME_IDX);
		}
		else
		{
			Ui_DisplayBrowser_ItemName();
		}
	}

	if((uc_lcd_dis_flg & BIT4_MASK) != 0)
	{
		uc_lcd_page_num = 4;
		uc_lcd_column_num = 0;
		Api_Display_Picture(PIC_MENU_ALL_YES_NO_IDX + uc_menu_cur_idx - 1);
	}
}

/*******************************************************************************
display delete string
*******************************************************************************/
void Ui_Display_StringDeleting(void)
{
	uc_lcd_page_num = 4;	
	uc_lcd_column_num = 0;
 	Api_Display_Picture(NORM_STR_DELETING_IDX + ALL_MENU_NUMBER * uc_language_idx);
	Api_Delay_1ms(250);
	Api_Delay_1ms(250);
}


/*******************************************************************************
display delete string
*******************************************************************************/
void Ui_Display_NoContent(void)
{
	Api_LcdDriver_Clr_AllPage();
	uc_lcd_page_num = 3;	
	uc_lcd_column_num = 0;
 	Api_Display_Picture(NORM_STR_NOCONTENT_IDX + ALL_MENU_NUMBER * uc_language_idx);
	Api_Delay_1ms(250);
	Api_Delay_1ms(250);
}

/*******************************************************************************
display looding string
********************************************************************************/
void Ui_DisplayEbook_Loading(void)
{
	Api_LcdDriver_Clr_AllPage();
	uc_lcd_page_num = 3;	
	uc_lcd_column_num = 0;
	Api_Display_Picture(NORM_STR_LOADING_IDX + ALL_MENU_NUMBER * uc_language_idx);
	if(ul_fs_file_length < 51200)	//if file length < 50k, and delay 500ms for display "loading..."
	{
		Api_Delay_1ms(250);
		Api_Delay_1ms(250);
	}
}
 /*******************************************************************************
display draw SDcard
*******************************************************************************/
void Ui_Display_DrawSD(void)
{
	Api_LcdDriver_Clr_AllPage();		//clr dis lrc area
	uc_lcd_page_num = 3;
	uc_lcd_column_num = 0;
	Api_Display_Picture(NORM_STR_DRAW_CARD_IDX + ALL_MENU_NUMBER * uc_language_idx);
	Api_Delay_1ms(250);
	Api_Delay_1ms(250);
}
 /*******************************************************************************
Ui_Display_InsertCard
*******************************************************************************/
/*void Ui_Display_InsertCard(void)
{
	Api_LcdDriver_Clr_AllPage();		//clr dis lrc area
	uc_lcd_page_num = 4;
	uc_lcd_column_num = 0;
	Api_Display_Picture(NORM_STR_INSERT_CARD_IDX + ALL_MENU_NUMBER * uc_language_idx);
	Api_Delay_1ms(250);
	Api_Delay_1ms(250);
}
/*******************************************************************************
display no file
*******************************************************************************/
void Ui_Display_NoFile(void)
{
	Api_LcdDriver_Clr_AllPage();		//clr dis lrc area
	uc_lcd_page_num = 3;
	uc_lcd_column_num = 0;
	if(b_sd_flash_flg == 1)		//SD/MMC care exsit, and select flash
	{
	Api_Display_Picture(NORM_STR_SD_NOFILE_IDX + ALL_MENU_NUMBER * uc_language_idx);
	uc_menu_cur_idx_bk = 1;
	}
	else
	{ 	
	Api_Display_Picture(NORM_STR_FLASH_NOFILE_IDX + ALL_MENU_NUMBER * uc_language_idx);
	uc_menu_cur_idx_bk = 2;
	}
	Api_Delay_1ms(250);
	Api_Delay_1ms(250);
}

 /*******************************************************************************
Function: Ui_Display_MSWP()
Description: display "SD/MMC card has been WP" 
*******************************************************************************/
void Ui_Display_MSWP(void)
{
	Api_LcdDriver_Clr_AllPage();		//clr dis lrc area
	uc_lcd_page_num = 4;
	uc_lcd_column_num = 0;
	Api_Display_Picture(NORM_STR_PROTECT_CARD_IDX + ALL_MENU_NUMBER * uc_language_idx);
	Api_Delay_1ms(250);
	Api_Delay_1ms(250);	
}
#endif