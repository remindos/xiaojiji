/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_ebook_get_pagenum.c
Creator: zj.zong			Date: 2009-05-05
Description: init ebook parameter
Others:
Version: V0.1
History:
	V0.1	2009-05-05		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_EBOOK_GET_PAGE_NUMBER_DEF

#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_EBook\ui_ebook_header.h"
#include "..\..\UI\UI_EBook\ui_ebook_var_ext.h"

extern bit FS_Fread(void);
extern void FS_WrFlash_Interface(void);
extern ui16 Api_Isn_ConvertTo_Unicode(ui16 isn_index);
extern ui16 Api_Merge_2Byte_Sub(uc8 lo,uc8 hi);
extern void Ap_Exchange_HighLow_Byte(void);
extern void Ap_Get_OneByte(void);		  

void Ap_Clear_Dbuf0(void);
void Ap_WriteInfo_ToFlash(void);
void Ap_GetEbook_PageNum(void);
void Ap_SaveStart_BytePage(void);
/*******************************************************************************
Function: Ap_GetEbook_PageNum()
Description: init ebook parameter
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_GetEbook_PageNum(void)
{
	uc8 tmp_line_num, tmp_column_addr, tmp_enter_flg;
	ui16 tmp_unicode;
	
	tmp_line_num = 0;
	tmp_enter_flg = 0;
	tmp_column_addr = DIS_EBOOK_X;
	Ap_Clear_Dbuf0();

	uc_fs_assign_buf_flg = SFC_APP_DBUF1;	//assign dbuf(0x5000) to fs
	if(FS_Fread() == 0)
	{
		return;								//the txt file is null 0bytes;
	}
	if(ui_dbuf1[0] == 0xfeff)
	{
		uc_ebook_unicode_flg = 1;			//unicode little endian
		ui_ebook_byte_idx = 2;				//offset is 2
		ul_dbuf0[0] = 2;
	}
	else if(ui_dbuf1[0] == 0xfffe)
	{
		uc_ebook_unicode_flg = 2;				//unicode big endian
		ui_ebook_byte_idx = 2;					//offset is 2
		ul_dbuf0[0] = 2;
		Ap_Exchange_HighLow_Byte();
	}
	else
	{
		Ap_Exchange_HighLow_Byte();
	}
	
	while(1)
	{
		if(tmp_line_num >= DIS_EBOOK_LINE_MAX_NUM)
		{
			tmp_line_num = 0;
			
			if(ui_ebook_page_num >= DIS_EBOOK_PAGE_MAX_NUM)	//page number is overflow
			{
				break;
			}
			
			ui_ebook_page_num++;
			tmp_enter_flg = 0;
			Ap_SaveStart_BytePage();		//save the start byte in one page, if fill one sector, and write info flash
		}
		
		Ap_Get_OneByte();					//read one byte "ISN"
		if(b_ebook_file_end == 1)
		{
			if((tmp_line_num != 0) || (tmp_column_addr != DIS_EBOOK_X))
			{
				ui_ebook_page_num++;	//last page
			}
			break;
		}

/*********1.txt is "ASCII" format*********/
		if(uc_ebook_unicode_flg == 0)	
		{
			if((ui_ebook_val & 0x80) != 0)	//check "ASCII"?
			{
				if(b_ebook_first_unicode_flg == 0)
				{
					b_ebook_first_unicode_flg = 1;
					tmp_unicode = Api_Isn_ConvertTo_Unicode(ui_ebook_val);
				}
				else
				{
					if(b_ebbok_unicode_8_16_flg == 0)
					{
						tmp_unicode = 1;	//16*8
					}
					else
					{
						tmp_unicode = 0;	//16*16
					}
				}
	
				if(tmp_unicode != 0)		//ISN is one byte
				{
					b_ebbok_unicode_8_16_flg = 0;
					goto unicode_8x16;
				}
				else						//ISN is two bytes
				{
//unicode_16x16:
					b_ebbok_unicode_8_16_flg = 1;
					if(tmp_column_addr <= DIS_EBOOK_X_END_ADDR)
					{
						tmp_enter_flg = 0;
						tmp_column_addr += 16;
						if(tmp_column_addr > DIS_EBOOK_X_END_ADDR)
						{
							goto line_num_add1_1;
						}
					}
					else
					{
line_num_add1_1:
						tmp_enter_flg = 1;
						tmp_column_addr = DIS_EBOOK_X;//reset lcd column start address
						tmp_line_num++;				//line number + 1
					}
					
					Ap_Get_OneByte();					//appoint to the next ISN
				}
			}
			
			else							//ISN is one byte
			{
unicode_8x16:
				if(ui_ebook_val == 0x0d)			//the character is "Enter", switch to the next line
				{
check_enter:
					Ap_Get_OneByte();					//skip "0x0A", appoint to the next ISN
					if(ui_ebook_val == 0x0a)
					{
						if(tmp_enter_flg == 0)
						{
							tmp_column_addr = DIS_EBOOK_X;//reset lcd column start address
							tmp_line_num++;				//line number + 1
						}
						else
						{
							tmp_enter_flg = 0;
						}
					}
					else if(ui_ebook_val == 0x0d)
					{
						if(tmp_column_addr <= DIS_EBOOK_X_END_ADDR)
						{
							tmp_enter_flg = 0;
							tmp_column_addr += 8;
							if(tmp_column_addr > DIS_EBOOK_X_END_ADDR)
							{
								goto line_num_add1_3;
							}
						}
						else
						{
line_num_add1_3:
							tmp_enter_flg = 1;
							tmp_column_addr = DIS_EBOOK_X;//reset lcd column start address
							tmp_line_num++;				//line number + 1
						}
	
						goto check_enter;
					}
				}
				else
				{
					if(tmp_column_addr <= DIS_EBOOK_X_END_ADDR)
					{
						tmp_enter_flg = 0;
						tmp_column_addr += 8;
						if(tmp_column_addr > DIS_EBOOK_X_END_ADDR)
						{
							goto line_num_add1_2;
						}
					}
					else
					{
line_num_add1_2:
						tmp_enter_flg = 1;
						tmp_column_addr = DIS_EBOOK_X;//reset lcd column start address
						tmp_line_num++;				//line number + 1
					}
				}
			}
		}
		
/*********2.txt is "Unicode" format*********/
		else
		{
			if((ui_ebook_val & 0xff00) != 0)		//unicode is 16*16
			{
					if(tmp_column_addr <= DIS_EBOOK_X_END_ADDR)
					{
						tmp_enter_flg = 0;
						tmp_column_addr += 16;
						if(tmp_column_addr > DIS_EBOOK_X_END_ADDR)
						{
							goto line_num_add1_4;
						}
					}
					else
					{
line_num_add1_4:
						tmp_enter_flg = 1;
						tmp_column_addr = DIS_EBOOK_X;	//reset lcd column start address
						tmp_line_num++;					//line number + 1
					}
			}
			else										//unicode is 8*16
			{
				if(ui_ebook_val == 0x000d)				//the character is "Enter", switch to the next line
				{
check_enter_1:
					Ap_Get_OneByte();						//skip "0x0A", appoint to the next ISN
					if(ui_ebook_val == 0x000a)
					{
						if(tmp_enter_flg == 0)
						{
							tmp_column_addr = DIS_EBOOK_X;//reset lcd column start address
							tmp_line_num++;				//line number + 1
						}
						else
						{
							tmp_enter_flg = 0;
						}
					}
					else if(ui_ebook_val == 0x000d)
					{
						if(tmp_column_addr <= DIS_EBOOK_X_END_ADDR)
						{
							tmp_enter_flg = 0;
							tmp_column_addr += 8;
							if(tmp_column_addr > DIS_EBOOK_X_END_ADDR)
							{
								goto line_num_add1_5;
							}
						}
						else
						{
line_num_add1_5:
							tmp_enter_flg = 1;
							tmp_column_addr = DIS_EBOOK_X;//reset lcd column start address
							tmp_line_num++;				//line number + 1
						}
	
						goto check_enter_1;
					}
				}
				else
				{
					if(tmp_column_addr <= DIS_EBOOK_X_END_ADDR)
					{
						tmp_enter_flg = 0;
						tmp_column_addr += 8;
						if(tmp_column_addr > DIS_EBOOK_X_END_ADDR)
						{
							goto line_num_add1_6;
						}
					}
					else
					{
line_num_add1_6:
						tmp_enter_flg = 1;
						tmp_column_addr = DIS_EBOOK_X;//reset lcd column start address
						tmp_line_num++;				//line number + 1
					}
				}
			}
		}
	}
	
	sfc_logblkaddr0 = (uc8)ui_ebook_sfc_addr;
	sfc_logblkaddr1 = (uc8)(ui_ebook_sfc_addr >> 8);
	sfc_logblkaddr2 = 0x00;
	sfc_logblkaddr3 = 0x00;
	sfc_occ_buf_flag = SFC_APP_DBUF0;		//occupy dbuf(0x4000)
	sfc_reserved_area_flag = 1;				//write flash reserved area
	buf_offset0 = 0x00;						//buffer offset: 0x0000
	buf_offset1 = 0x00;
	b_sfc_immediate_wr_flg = 1;				//last write
	FS_WrFlash_Interface();						//write one page
	sfc_reserved_area_flag = 0;		
}

/********************************************************************************
clr dbuf0
********************************************************************************/
void Ap_Clear_Dbuf0(void)
{
	uc8 i;
	for(i = 0; i <= 0x7f; i++)
	{
		ul_dbuf0[i] = 0;
	}
}

/********************************************************************************
write information to dbuf0
********************************************************************************/
void Ap_WriteInfo_ToFlash(void)
{
	sfc_logblkaddr0 = (uc8)ui_ebook_sfc_addr;
	sfc_logblkaddr1 = (uc8)(ui_ebook_sfc_addr >> 8);
	sfc_logblkaddr2 = 0x00;
	sfc_logblkaddr3 = 0x00;
	sfc_occ_buf_flag = SFC_APP_DBUF0;		//occupy dbuf(0x4000)
	sfc_reserved_area_flag = 1;				//write flash reserved area
	buf_offset0 = 0x00;						//buffer offset: 0x0000
	buf_offset1 = 0x00;
	b_sfc_immediate_wr_flg = 0;
	FS_WrFlash_Interface();						//write one page
	sfc_reserved_area_flag = 0;					//clr reserved area flag
		
	ui_ebook_sfc_addr++;					//sfc reserved address + 1 sectors
}

/*******************************************************************************
1. save the start byte in one page, length is 4bytes
2. one sector can store 128 pages(128 * 4bytes = one sector)
3. the start byte of first page is 0x00000000
********************************************************************************/
void Ap_SaveStart_BytePage(void)
{
	ui16 tmp_page_pt;
	
	tmp_page_pt = ui_ebook_page_num & 0x07f;
	ul_dbuf0[tmp_page_pt] = ul_fs_byte_offset - 512 + ui_ebook_byte_idx;
	
	if(tmp_page_pt >= 0x7f)
	{
		Ap_WriteInfo_ToFlash();
		Ap_Clear_Dbuf0();
	}
}

#endif