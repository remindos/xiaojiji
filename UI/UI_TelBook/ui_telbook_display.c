/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_telbook_display.c
Creator: zj.zong			Date: 2009-06-508
Description: display function
Others:
Version: V0.1
History:
	V0.1	2009-06-08		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_TELBOOK_DISPLAY_DEF

#include "sh86278_sfr_reg.h"
#include "ui_resident.h"
#include "hwdriver_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_api_define.h"
#include ".\UI\UI_TelBook\ui_telbook_header.h"
#include ".\UI\UI_TelBook\ui_telbook_var_ext.h"

extern void Ui_Display_Battery(void);
extern void Ui_Display_HoldPic(void);

extern void _media_readsector(void);
extern void Ap_Exchange_HighLowByte(void);
extern void Ap_Init_EF_LogicAddr(void);
extern void Api_Dis_FileBrowser_Reverse(void);
extern void Api_Delay_1ms(uc8 timer_cnt);
extern ui16 Api_Merge_2Byte_Sub(uc8 lo,uc8 hi);
extern ui16 Api_Isn_ConvertTo_Unicode(ui16 isn_index);
extern void Api_Display_Unicode(ui16 unicode_idx);
extern void Api_Display_Picture(ui16 ui_pic_idx);
extern void Api_LcdDriver_Clr_AllPage(void);
extern void	Api_Display_8x6_ZiKu(uc8 ziku_idx);

void Ui_Delay_500ms(void);
void Ui_Display_MenuString(void);
void Ui_Display_SelectMenuFlag(void);
void Ui_Display_TelBookMain(void);
void Ui_Display_TelBook_InOut(void);
void Ui_Display_TelBookName(void);
void Ui_Display_NameList(void);
void Ui_Display_TelBookInfo(void);
void Ui_display_StringImporting(void);
void Ui_display_StringImport_Ok(void);
void Ui_display_StringExporting(void);
void Ui_display_StringExport_Ok(void);
void Ui_Display_TelBookInfo_Common(void);
void Ui_Display_String_NoSpace(void);
void Ui_Display_String_NoTelFile(void);
void Ui_Display_NoticeStrCommon(void);
void Ui_Display_Menu_Item_Idx(uc8 num_idx,uc8 Total_num);
void Ui_Display_NameIfo_List(void);
//void Ui_Display_InsertCard(void);

uc8 code tmp_info_offset[6] = {0,16, 32, 64, 96, 112};

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
			case WINDOW_TELBOOK_MAIN:
				Ui_Display_TelBookMain();
				break;
	
			case WINDOW_TELBOOK_NAME:
				Ui_Display_TelBookName();
				break;
	
			case WINDOW_TELBOOK_INFO:
				 Ui_Display_TelBookInfo();
				break;
	
			default:
				Ui_Display_TelBook_InOut();
				break;
		}
//	}
	b_lcd_clrall_flg = 0;
	uc_lcd_dis_flg = 0;
}

/*******************************************************************************
Function: Ui_Display_TelBookMain()
Description: display telbook main menu
Calls:
Global: uc_menu_cur_idx
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Display_TelBookMain(void)
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

/*******************************************************************************
Function: Ui_Display_SelectMenuFlag()
Description:
Calls:
Global:
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Display_SelectMenuFlag(void)
{
  	uc_lcd_column_num = 112;
	uc_lcd_page_num = uc_lcd_page_num_bk;
	Api_Display_Picture(PIC_SELECT_FLAG_IDX + uc_refresh_menu_num);
}

/*******************************************************************************
Function: Ui_Display_MenuString()
Description:
Calls:
Global:
Input: Null
Output:Null
Others:
********************************************************************************/
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
		Api_Display_Picture(i + MENU_TEL_STR_MENU_IDX + ALL_MENU_NUMBER * uc_language_idx);
	
		b_lcd_reverse_flg = 0;				
		uc_lcd_page_num = uc_lcd_page_num + 1;
		uc_lcd_page_num_tmp = uc_lcd_page_num;
	}
}


/*******************************************************************************
Function: Ui_Display_Menu_Item_Idx()
Description:
Calls:
Global:
Input: Null
Output:Null
Others:
********************************************************************************/
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
Function: Ui_Display_TelBookName()
Description: clr menu area exept title area 
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ui_Display_TelBookName(void)
{
	if((uc_lcd_dis_flg & BIT3_MASK) != 0)
	{
		Ui_Display_NameList();
	}
	
	if((uc_lcd_dis_flg & BIT5_MASK) != 0)
	{
		Ui_Display_Menu_Item_Idx(uc_tel_namelist_sel_idx + 1,uc_tel_namelist_num);
	}
}

/*******************************************************************************
Function: Ui_Display_NameList()
Description: display telbook name list 
Calls:
Global: uc_tel_namelist_num
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Display_NameList(void)
{
	uc8 i, j, tmp_name_num;
	ui16 tmp_offset;
	
	if(uc_tel_namelist_num < DisMenu_MaxNum)		  //current page max number
	{
		tmp_name_num = uc_tel_namelist_num;
	}
	else
	{
		tmp_name_num = DisMenu_MaxNum;
	}
	uc_lcd_page_num = 2;
	uc_lcd_column_num = 0;
	for(i = 0; i < 3; i++)
	{
	  	if((i+uc_tel_namelist_top_idx) == uc_tel_namelist_sel_idx)		   	 //set reverse flag
		{
			b_lcd_reverse_flg = 1;
		}
		else
		{
			b_lcd_reverse_flg = 0;
		}

		tmp_offset = (ui16)(i + uc_tel_namelist_top_idx) * 16;	//name max number is 16
		for(j = 0; j < 16; j++)
		{
			uc_tel_dis_info_buf[j] = uc_dbuf0[j + tmp_offset];
		}
		uc_lcd_column_num = 0;
		Ui_Display_TelBookInfo_Common();	 //display telbook name
	
		uc_lcd_page_num_bk = uc_lcd_page_num;
		if(b_lcd_reverse_flg == 1)
		{
			Api_Dis_FileBrowser_Reverse();	//reverse current select telbook				
		}
		uc_lcd_page_num = uc_lcd_page_num_bk;

		b_lcd_reverse_flg = 0;
		uc_lcd_page_num += 2;		//(lcd row) + (menu height), prepare to display next menu
	}
}


/*******************************************************************************
Function: Ui_Display_TelBookInfo()
Description: display telbook infomation 
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ui_Display_TelBookInfo(void)
{
	if((uc_lcd_dis_flg & BIT2_MASK) != 0)
	{
		Ui_Display_Menu_Item_Idx(uc_menu_cur_idx,uc_menu_total_num);
	}
	
	if((uc_lcd_dis_flg & BIT3_MASK) != 0)
	{
		Ui_Display_NameIfo_List();
	}
}

/*******************************************************************************
Function: Ui_Display_NameIfo_List()
Description: display telbook name iformation list 
Calls:
Global: uc_tel_namelist_num
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Display_NameIfo_List(void)
{
	uc8 i, j,k;
	ui16 tmp_offset;
		
	Ap_Init_EF_LogicAddr();
	sfc_logblkaddr0 = TELBOOK_EF_START_ADDR + uc_tel_namelist_sel_idx / 4; //get telbook address
	sfc_reserved_area_flag = 1;				//read flash reserved area
	buf_offset0 = 0x00;						//buffer offset: 0x0800
	buf_offset1 = 0x00;
	sfc_occ_buf_flag = SFC_APP_DBUF1;		//accupy dbuf0(0x5000)
	_media_readsector();
	sfc_reserved_area_flag = 0;					//clr reserved area flag

	Ap_Exchange_HighLowByte();		 
	
	uc_lcd_page_num = 2;
	for(i = uc_menu_top_idx, j = 0; j < DisMenu_MaxNum; i++, j++)
	{
		tmp_offset = (uc_tel_namelist_sel_idx % 4) * 128 + tmp_info_offset[i];
		
		uc_lcd_column_num = 0;					//display information flg

		uc_lcd_page_num_bk = uc_lcd_page_num;
		Api_Display_Picture(PIC_TEL_INFO_LIST_IDX + i);

		uc_lcd_page_num = uc_lcd_page_num_bk;

		if(i == (uc_menu_cur_idx - 1))				//check displayed menu be selected?
		{
			b_lcd_reverse_flg = 1;				
		}
		else
		{
			b_lcd_reverse_flg = 0;				
		}

		uc_lcd_column_num = DIS_MENU_FIRST_STR_X;	//display menu string
		for(k = 0; k < 16; k++)
		{
			uc_tel_dis_info_buf[k] = uc_dbuf1[k + tmp_offset];
		}

		Ui_Display_TelBookInfo_Common();

		uc_lcd_page_num = uc_lcd_page_num_bk;
		if(b_lcd_reverse_flg == 1)
		{
			Api_Dis_FileBrowser_Reverse();
		}
		uc_lcd_page_num = uc_lcd_page_num_bk;

		b_lcd_reverse_flg = 0;				
		uc_lcd_page_num += 2;		//(lcd row) + (menu height), prepare to display next menu
	}
}

/*******************************************************************************
Function: Ui_Display_TelBook_InOut()
Description: display telbook export or import 
Calls:
Global: ui_tel_string_idx
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Display_TelBook_InOut(void)
{
	Api_LcdDriver_Clr_AllPage();
	
	if(uc_window_no == WINDOW_TELEBOOK_IN)
	{
		ui_tel_string_idx = NORM_STR_IN_IDX;
	}
	else
	{
		ui_tel_string_idx = NORM_STR_OUT_IDX;
	}
   	uc_lcd_page_num = 2;
	uc_lcd_column_num = 0;		//display import or export string
	Api_Display_Picture(ui_tel_string_idx + ALL_MENU_NUMBER * uc_language_idx);

	uc_lcd_page_num = 4;
	uc_lcd_column_num = 0;		//display "YES" & "NO"
	Api_Display_Picture(PIC_NOTICE_YES_NO_IDX + uc_menu_cur_idx - 1);
	
}

/*******************************************************************************
display telbook information common:telbook name,tel,email,qq,tax...
********************************************************************************/


void Ui_Display_TelBookInfo_Common(void)
{
	uc8 i = 0;
	ui16 tmp_unicode;
	
	while(1)
	{
		if(uc_lcd_column_num > DIS_TEL_NAME_X_END)
		{
			break;
		}
		
		if(uc_tel_dis_info_buf[i] == 0)
		{
			break;
		}
		
		tmp_unicode = Api_Isn_ConvertTo_Unicode(uc_tel_dis_info_buf[i]);
		if(tmp_unicode == 0)
		{
			tmp_unicode = Api_Isn_ConvertTo_Unicode(Api_Merge_2Byte_Sub(uc_tel_dis_info_buf[i+1],uc_tel_dis_info_buf[i]));
			i += 2;
		}
		else
		{
			i++;
		}
		Api_Display_Unicode(tmp_unicode);
	}
}

/*******************************************************************************
display telbook string import
********************************************************************************/
void Ui_Display_StringImporting(void)
{
	ui_tel_string_idx = NORM_STR_IMPORTING_IDX;
	Ui_Display_NoticeStrCommon();
	Ui_Delay_500ms();
}

/*******************************************************************************
display telbook string import ok
********************************************************************************/
void Ui_Display_StringImport_Ok(void)
{
	ui_tel_string_idx = NORM_STR_IMPORT_OK_IDX;
	Ui_Display_NoticeStrCommon();
	Ui_Delay_500ms();
}

/*******************************************************************************
display telbook string export 
********************************************************************************/
void Ui_Display_StringExporting(void)
{
	ui_tel_string_idx = NORM_STR_EXPORTING_IDX;
	Ui_Display_NoticeStrCommon();
	Ui_Delay_500ms();
}

/*******************************************************************************
display telbook string export ok
********************************************************************************/
void Ui_Display_StringExport_Ok(void)
{
	ui_tel_string_idx = NORM_STR_EXPORT_OK_IDX;
	Ui_Display_NoticeStrCommon();
	Ui_Delay_500ms();
}

/*******************************************************************************
display telbook string no space
********************************************************************************/
void Ui_Display_String_NoSpace(void)
{
	Api_LcdDriver_Clr_AllPage();
	uc_lcd_page_num = 3;
	uc_lcd_column_num = 0;		//display no space

	Api_Display_Picture(NORM_STR_NOSPACE_IDX + ALL_MENU_NUMBER * uc_language_idx);
	Ui_Delay_500ms();
}


/*******************************************************************************
display telbook string no file
********************************************************************************/
void Ui_Display_String_NoTelFile(void)
{
	Api_LcdDriver_Clr_AllPage();	
 
 	uc_lcd_page_num = 3;
	uc_lcd_column_num = 0;		//display no telbook file string
	Api_Display_Picture(NORM_STR_NO_TELFILE_IDX + ALL_MENU_NUMBER * uc_language_idx);
	Ui_Delay_500ms();
}


/*******************************************************************************
display notice common
********************************************************************************/
void Ui_Display_NoticeStrCommon(void)
{  
	Api_LcdDriver_Clr_AllPage();

	uc_lcd_page_num = 3;
	uc_lcd_column_num = 0;	
	Api_Display_Picture(ui_tel_string_idx + ALL_MENU_NUMBER * uc_language_idx);
}

/*******************************************************************************
delay 500ms
********************************************************************************/
void Ui_Delay_500ms(void)
{
	Api_Delay_1ms(250);
	Api_Delay_1ms(250);
}


/*void Ui_Display_InsertCard(void)
{
	Api_LcdDriver_Clr_AllPage();		//clr dis lrc area
	uc_lcd_page_num = 4;
	uc_lcd_column_num = 0;
	Api_Display_Picture(NORM_STR_INSERT_CARD_IDX + ALL_MENU_NUMBER * uc_language_idx);
	Ui_Delay_500ms();
}*/


#endif