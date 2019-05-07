/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_sys_display.c
Creator: zj.zong				Date: 2009-05-27
Description: display function
Others:
Version: V0.1
History:
	V0.1	2009-05-27		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_SYS_DISPLAY_DEF

#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include ".\UI\UI_System\ui_sys_header.h"
#include ".\UI\UI_System\ui_sys_var_ext.h"

extern void Ui_Display_Battery(void);
extern void Ui_Display_HoldPic(void);

extern void Api_Delay_1ms(uc8 timer_cnt);
extern void Api_Display_Unicode(ui16 unicode_idx);
extern void Api_Display_Picture(ui16 ui_pic_idx);
extern void Api_Display_8x6_ZiKu(uc8 ziku_idx);
extern void Api_LcdDriver_Clr_AllPage(void);
extern void Api_LcdDriver_ClrOne_Page(uc8 num_idx);
extern void Api_Dis_FileBrowser_Reverse(void);

void Ui_Display_Memory_Info(ui16 tmp_dis_memory);
void Ui_Display_Sys_Contrast(void);
void Ui_Display_Sys_BklightTime(void);
void Ui_Display_Sys_Menu_Idle(void);
void Ui_Display_Sys_Menu_Sleep(void);
void Ui_Display_Sys_Product_Memory(void);
void Ui_Display_Sys_Product_Version(void);
void Ui_Display_Sys_Menu_Common(void);
void Ui_Display_SysProduct_Date(void);
void Ui_Display_SysPduct_Version(void);
void Ui_Display_SelectMenuFlag(void);
void Ui_Display_MenuIdx(void);
void Ui_Display_SelectMenuFlag(void);
void Ui_Display_MenuString(void);

code ui16 MenuStr_Table_Sys[]=
{
	MENU_STR_SYS_MENU_IDX,
	MENU_STR_SYS_MENU_BKLIGHT_TIME_IDX,
	MENU_STR_SYS_MENU_POWEROFF_IDX,
	MENU_STR_SYS_MENU_POWEROFF_IDLE_IDX,
	MENU_STR_SYS_MENU_POWEROFF_SLEEP_IDX,
	MENU_STR_SYS_MENU_REPLAY_IDX,
	0x0000,			 //Contrast
	MENU_STR_SYS_MENU_LANGUAGE_IDX,
	0X0000,			// Memory
	0X0000,			// Version
	MENU_STR_SYS_MENU_FACTORY_IDX,
	MENU_STR_SYS_MENU_DISK_TYPE_IDX,
	MENU_REC_QUALITY_BASE_IDX,
	PIC_SYS_STR_MENU_AMT_IDX,

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
/* 	if((uc_lcd_dis_flg & BIT1_MASK) != 0)
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
			case WINDOW_SYS_MENU_CONTRAST:
				Ui_Display_Sys_Contrast();
				break;
	
		   	case WINDOW_SYS_MENU_MEMORY:
				Ui_Display_Sys_Product_Memory();
				break;
	
			case WINDOW_SYS_MENU_VERSION:
				Ui_Display_Sys_Product_Version();
				break;
	
			default:
			   	Ui_Display_Sys_Menu_Common();
			   	break;
		}
//	}
	b_lcd_clrall_flg = 0;
	uc_lcd_dis_flg = 0;
}


/*******************************************************************************
display system contrast
********************************************************************************/
void Ui_Display_Sys_Contrast(void)
{
	if((uc_lcd_dis_flg & BIT4_MASK) != 0)
	{
		uc_lcd_page_num = 2;
		uc_lcd_column_num = 0;
		Api_Display_Picture(PIC_SYS_BKGRADE_BASE_IDX+uc_menu_cur_idx-1);
	}
}

/*******************************************************************************
display system product memory window  
********************************************************************************/
void Ui_Display_Sys_Product_Memory(void)
{
	ul32 tmp_memory;
	ui16 tmp_dis_memory;
	
	uc_lcd_column_num = 0;	//display "flash memory" string
	uc_lcd_page_num = 2;
	Api_Display_Picture(PIC_SYS_MEMORYINFO_IDX);

	uc_lcd_column_num = 76;		//display memory "xxxM/xxxxM"
	uc_lcd_page_num = 3;

	tmp_memory = (ul32)uc_fs_fat_sector_max_num4 << 24;
	tmp_memory |= (ul32)uc_fs_fat_sector_max_num3 << 16;
	tmp_memory |= (ul32)uc_fs_fat_sector_max_num2 << 8;
	tmp_memory |= (ul32)uc_fs_fat_sector_max_num1;
	tmp_dis_memory = (ui16)(tmp_memory >> 11);	//convert sector to "M"
	Ui_Display_Memory_Info(tmp_dis_memory);		//display xxxM

	uc_lcd_column_num = 76;						//display memory "xxxM/xxxxM"
	uc_lcd_page_num =  5;

	tmp_dis_memory = (ui16)(ul_fs_spare_space >> 11);	//convert sector to "M"
	Ui_Display_Memory_Info(tmp_dis_memory);		//display xxxxM	
}

/*******************************************************************************
display system sleep window  
********************************************************************************/
void Ui_Display_Sys_Product_Version(void)
{
	if((uc_lcd_dis_flg & BIT4_MASK) != 0)
	{
		uc_lcd_page_num = 2;
		uc_lcd_column_num = 0;
		Api_Display_Picture(PIC_SYS_VERSION_IDX);	  //version flag
	  
		Ui_Display_SysProduct_Date();				//display date "xxxx-xx-xx"
		Ui_Display_SysPduct_Version();				//display versino "Vx.xx"
	}
}

void Ui_Display_SysPduct_Version(void)
{
	uc8 i;
	uc_lcd_page_num = 4;
	uc_lcd_column_num = 48;		//display version "Vx.xx"
	for(i = 0; i < 10; i++)
	{
		Api_Display_Unicode(uc_sys_ver_buf[i]);
	}
}



void Ui_Display_SysProduct_Date(void)
{
	uc8 i;
	uc_lcd_page_num = 2;
	uc_lcd_column_num = 48;		//display date "xxxx/xx/xx"
	for(i = 0; i < 10; i++)
	{
		Api_Display_Unicode(uc_sys_date_buf[i]);
	}
}


/*******************************************************************************
display system memory information  
********************************************************************************/
void Ui_Display_Memory_Info(ui16 tmp_dis_memory)
{
	if(tmp_dis_memory >= 10000)
	{
		Api_Display_8x6_ZiKu(tmp_dis_memory / 10000);
	}
	
	if(tmp_dis_memory >= 1000)
	{
		Api_Display_8x6_ZiKu(tmp_dis_memory / 1000 % 10);
	}
	
	if(tmp_dis_memory >= 100)
	{
		Api_Display_8x6_ZiKu(tmp_dis_memory / 100 % 10);
	}
	
	if(tmp_dis_memory >= 10)
	{
		Api_Display_8x6_ZiKu(tmp_dis_memory / 10 % 10);
	}
	
	Api_Display_8x6_ZiKu(tmp_dis_memory % 10);
	
	Api_Display_8x6_ZiKu(12);						//display "M"
}

/*******************************************************************************
display system menu common
********************************************************************************/
void Ui_Display_Sys_Menu_Common(void)
{	
	if((uc_lcd_dis_flg & BIT3_MASK) != 0)
	{
		Ui_Display_MenuString();
	}

	if((uc_lcd_dis_flg & BIT4_MASK) != 0)
	{
		Ui_Display_MenuIdx();
	}
	if((uc_lcd_dis_flg & BIT6_MASK) != 0)
	{
		Ui_Display_SelectMenuFlag();
	}	
}

/*******************************************************************************
display system menu String
********************************************************************************/
void Ui_Display_MenuString(void)
{
	uc8 i, j, tmp_max_num ,uc_lcd_page_num_tmp,tmp_amt_top_idx,tmp_menu_cur_idx;
	
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

	if((uc_window_no == WINDOW_SYS_MENU) && ((uc_user_option3&0x04)== 0))//System menu window no amt
	{
		if(uc_menu_cur_idx >= 7)
		{
			tmp_menu_cur_idx = uc_menu_cur_idx - 1;	
		}
		else
		{
			tmp_menu_cur_idx = uc_menu_cur_idx; 	
		}
	}
	else
	{
		tmp_menu_cur_idx = uc_menu_cur_idx; 	
	}	

  
	for(i = uc_menu_top_idx, j = 0; j < tmp_max_num; i++, j++)
	{
		uc_lcd_column_num = 0;		   			    //display menu sel or not sel flg
	
		if(i == (tmp_menu_cur_idx - 1))				//check displayed menu be selected?
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
	
		if((uc_user_option3&0x04)!= 0)			 //have amt
		{
			tmp_amt_top_idx = 0;
		}
		else
		{
			if(uc_window_no == WINDOW_SYS_MENU)
			{
				if((uc_menu_top_idx >= 0) && (uc_menu_top_idx <= 5))
				{
					if(j >= (5 - uc_menu_top_idx))
					{
						tmp_amt_top_idx = 1;	
					}
					else
					{
						tmp_amt_top_idx = 0;	
					}						
				}
				else
				{
					tmp_amt_top_idx = 1;	
				}	
			}
			else
			{
				tmp_amt_top_idx = 0;
			}														
		}	

		Api_Display_Picture(i + tmp_amt_top_idx + MenuStr_Table_Sys[uc_window_no] + ALL_MENU_NUMBER * uc_language_idx);
	
		b_lcd_reverse_flg = 0;				
		uc_lcd_page_num = uc_lcd_page_num + 1;
		uc_lcd_page_num_tmp = uc_lcd_page_num;
	}
}

/*******************************************************************************
display system menu idx
********************************************************************************/
void Ui_Display_MenuIdx(void)										
{
	uc8 uc_menu_cur_idx_tmp, uc_menu_total_num_tmp;

	uc_lcd_page_num = 1;											
	uc_lcd_column_num = 43;	

   	uc_menu_cur_idx_tmp = uc_menu_cur_idx;
	uc_menu_total_num_tmp = uc_menu_total_num;

	if((uc_window_no == WINDOW_SYS_MENU) && ((uc_user_option3&0x04)== 0))//System menu window no amt
	{
		if(uc_menu_cur_idx > 5)
		{
			uc_menu_cur_idx_tmp -= 1;
		}
		uc_menu_total_num_tmp = SYS_MENU_NUM - 1;
	}	
									
	Api_Display_8x6_ZiKu(16);						//display "<<"		
	Api_Display_8x6_ZiKu(uc_menu_cur_idx_tmp/10);						
	Api_Display_8x6_ZiKu(uc_menu_cur_idx_tmp%10);					
	Api_Display_8x6_ZiKu(15);						//display "/"		
	Api_Display_8x6_ZiKu(uc_menu_total_num_tmp/10);						
	Api_Display_8x6_ZiKu(uc_menu_total_num_tmp%10);					
	Api_Display_8x6_ZiKu(17);						//display ">>"		

}

/*******************************************************************************
display sleect system menu flag
********************************************************************************/
void Ui_Display_SelectMenuFlag(void)
{
  	uc_lcd_column_num = 112;
	uc_lcd_page_num = uc_lcd_page_num_bk;
	Api_Display_Picture(PIC_SELECT_FLAG_IDX + uc_refresh_menu_num);
}
#endif