/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_ebook_save_para.c
Creator: zj.zong			Date: 2009-05-04
Description: save ebook parameter
Others:
Version: V0.1
History:
	V0.1	2009-05-04		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_EBOOK_SAVE_PARA_DEF

#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_EBook\ui_ebook_header.h"
#include "..\..\UI\UI_EBook\ui_ebook_var_ext.h"



extern void FS_WrFlash_Interface(void);
extern void _media_readsector(void);
void Ap_Save_Ebook_Para(void);
/*******************************************************************************
Function: Ap_Save_Ebook_Para()
Description: save ebook parameter
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Save_Ebook_Para(void)
{
	sfc_logblkaddr0 = 0x02;							//0x0000 0202
	sfc_logblkaddr1 = 0x02;
	sfc_logblkaddr2 = 0x00;
	sfc_logblkaddr3 = 0x00;
	sfc_reserved_area_flag = 1;						//read flash reserved area
	sfc_occ_buf_flag = SFC_APP_DBUF0;				//occupy dbuf(0x4000)
	buf_offset0 = 0x00;								//buffer offset :0x0000
	buf_offset1 = 0x00;
	_media_readsector();							//read one sector
	uc_dbuf0[339] = uc_save_mode_idx;			 //save mode

   	uc_dbuf0[250] = uc_ebook_mark1_cluster1;		//save EbookMark1 file cluster
	uc_dbuf0[251] = uc_ebook_mark1_cluster2;
	uc_dbuf0[252] = uc_ebook_mark1_length1;			//save EbookMark1 file length
	uc_dbuf0[253] = uc_ebook_mark1_length2;	
	uc_dbuf0[254] = (uc8)(ui_ebook_mark1 >> 8);  	 //save EbookMark1 page idx
	uc_dbuf0[255] = (uc8)ui_ebook_mark1;
	
   	uc_dbuf0[256] = uc_ebook_mark2_cluster1;		//save EbookMark2 file cluster
	uc_dbuf0[257] = uc_ebook_mark2_cluster2;
	uc_dbuf0[258] = uc_ebook_mark2_length1;			//save EbookMark2 file length
	uc_dbuf0[259] = uc_ebook_mark2_length2;	
  	uc_dbuf0[260] = (uc8)(ui_ebook_mark2 >> 8);   	//save EbookMark2 page idx
	uc_dbuf0[261] = (uc8)ui_ebook_mark2;	

   	uc_dbuf0[262] = uc_ebook_mark3_cluster1;		//save EbookMark3 file cluster
	uc_dbuf0[263] = uc_ebook_mark3_cluster2;
	uc_dbuf0[264] = uc_ebook_mark3_length1;			//save EbookMark3 file length
	uc_dbuf0[265] = uc_ebook_mark3_length2;	
	uc_dbuf0[266] = (uc8)(ui_ebook_mark3 >> 8);   //save EbookMark3 page idx
	uc_dbuf0[267] = (uc8)ui_ebook_mark3;

	uc_dbuf0[268] = b_sd_flash_flg;				 //save flash-sd
	uc_dbuf0[269] =	uc_ebook_play_mode;			    // 1:auto play, 0:manual
	uc_dbuf0[270] =	uc_ebook_play_gap;				// play gap 2s
	uc_dbuf0[271] = uc_ebookmark_flag;				// save flag


	b_sfc_immediate_wr_flg = 1;
	FS_WrFlash_Interface();
	sfc_reserved_area_flag = 0;					//clr reserved area flag
}

#endif