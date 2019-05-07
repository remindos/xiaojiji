/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_ebook_init.c
Creator: zj.zong			Date: 2009-05-05
Description: init ebook parameter
Others:
Version: V0.1
History:
	V0.1	2009-05-05		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_EBOOK_INIT_PLAY_DEF

#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_EBook\ui_ebook_header.h"
#include "..\..\UI\UI_EBook\ui_ebook_var_ext.h"

extern bit FS_Get_LongName(void);
extern void FS_Get_83Name(void);
extern ui16 Api_Merge_2Byte_Sub(uc8 lo,uc8 hi);
extern ui16 Api_Isn_ConvertTo_Unicode(ui16 isn_index);

void Ap_Get_Ebook_Mark(void);
void Ap_Process_83Name(void);
void Ap_Save_BookMark1(void);
void Ap_Save_BookMark2(void);
void Ap_Save_BookMark3(void);
void AP_InitPlay_Ebook_Sub(void);
void Ap_Get_Ebook_FileName(void);
/*******************************************************************************
Function: AP_InitPlay_Ebook_Sub()
Description: init ebook parameter
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void AP_InitPlay_Ebook_Sub(void)
{
	switch(uc_dynamicload_idx)
	{
		case 30:
		  	Ap_Get_Ebook_FileName();		  //get file name
			break;

		case 31:
			Ap_Get_Ebook_Mark();
			break;
	}
	uc_dynamicload_idx = 0;
}

/*******************************************************************************
Function: Ap_Get_Ebook_FileName()
Description: get ebook name
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Get_Ebook_FileName(void)
{
	if(FS_Get_LongName() == 0)
	{                                                           
		FS_Get_83Name();							//rts is uc_fs_item_83_name
		Ap_Process_83Name();                                      
	}
}


/*******************************************************************************
Function: Ap_Process_83Name()
Description: process 8.3 format item name, and convert isn into unicode
Calls:
Global:uc_fs_item_83_name[]
       ui_fs_item_name[]:  current item name(unicode)
       uc_fs_item_name_length: current item name length
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Process_83Name(void)
{
	uc8 i;
	ui16 tmp_unicode;

	uc_fs_item_name_length = 0;						//init the file length = 0

	if(uc_fs_item_83_name[8] != 0x20)				//check mp3 or wav file
	{
		uc_fs_item_83_name[11] = uc_fs_item_83_name[10];
		uc_fs_item_83_name[10] = uc_fs_item_83_name[9];
		uc_fs_item_83_name[9] = uc_fs_item_83_name[8];
		uc_fs_item_83_name[8] = '.';				//add "."
	}
	
	for(i = 0; i < 12; i++)
	{
		if(uc_fs_item_83_name[i] != 0x20)			//check is "space"
		{
			tmp_unicode = Api_Isn_ConvertTo_Unicode(Api_Merge_2Byte_Sub(uc_fs_item_83_name[i], 0x00));
			if(tmp_unicode == 0)
			{
				tmp_unicode = Api_Isn_ConvertTo_Unicode(Api_Merge_2Byte_Sub(uc_fs_item_83_name[i+1], uc_fs_item_83_name[i]));
				i++;
			}

			ui_fs_item_name[uc_fs_item_name_length] = tmp_unicode;
			uc_fs_item_name_length++;
		}
	}
}



/*******************************************************************************
Function: Ap_Get_Ebook_Mark()
Description: get ebook mark
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/	
void Ap_Get_Ebook_Mark(void)
{
	if((uc_ebook_mark1_cluster1 == uc_fs_begin_cluster1) && (uc_ebook_mark1_cluster2 == uc_fs_begin_cluster2))
	{
		if((uc_ebook_mark1_length1 == (uc8)ul_fs_file_length) && (uc_ebook_mark1_length2 == (uc8)(ul_fs_file_length>>8)))
		{
			ui_ebook_page_idx = ui_ebook_mark1;
			if(ui_ebook_page_idx > ui_ebook_page_num)
			{
				ui_ebook_page_idx = 1;
			}
			goto Label_Save_Mark1; 			  //save same file in same mark
		}
	}
	if((uc_ebook_mark2_cluster1 == uc_fs_begin_cluster1) && (uc_ebook_mark2_cluster2 == uc_fs_begin_cluster2))
	{
		if((uc_ebook_mark2_length1 == (uc8)ul_fs_file_length) && (uc_ebook_mark2_length2 == (uc8)(ul_fs_file_length>>8)))
		{
			ui_ebook_page_idx = ui_ebook_mark2;

			if(ui_ebook_page_idx > ui_ebook_page_num)
			{
				ui_ebook_page_idx = 1;
			}
			goto Label_Save_Mark2; 			//save same file in same mark
		}
	}	
	if((uc_ebook_mark3_cluster1 == uc_fs_begin_cluster1) && (uc_ebook_mark3_cluster2 == uc_fs_begin_cluster2))
	{
		if((uc_ebook_mark3_length1 == (uc8)ul_fs_file_length) && (uc_ebook_mark3_length2 == (uc8)(ul_fs_file_length>>8)))
		{
			ui_ebook_page_idx = ui_ebook_mark3;

			if(ui_ebook_page_idx > ui_ebook_page_num)
			{
				ui_ebook_page_idx = 1;
			}
			goto Label_Save_Mark3; 		//save same file in same mark						
		}
	}
	if ((uc_ebookmark_flag & 0x01)== 0)	//bit0,bit1,bit2:  1 is saved ebookmark,0 is not saved 
	{									//bit3,bit4,bit5:  1 last saved bookmark (for next read),0 is not saved
		Ap_Save_BookMark1();			//bit0,bit1,bit2,bit3,bit4,bit5: (for next save)
	}
	else if ((uc_ebookmark_flag & 0x02) == 0)
	{
		Ap_Save_BookMark2();		
	}
	else if ((uc_ebookmark_flag & 0x04) == 0)
	{
		Ap_Save_BookMark3();		
	}
	else if ((uc_ebookmark_flag & 0x08) != 0)
	{
Label_Save_Mark2:
		Ap_Save_BookMark2();		
	}
	else if ((uc_ebookmark_flag & 0x10) != 0)
	{
Label_Save_Mark3:
		Ap_Save_BookMark3();		
	}
	else if ((uc_ebookmark_flag & 0x20) != 0)
	{
Label_Save_Mark1:
		Ap_Save_BookMark1();		
	}
}

void Ap_Save_BookMark1(void)		 	//bit0,bit1,bit2:  1 is saved bookmark,0 is not 
{									 	//bit3,bit4,bit5:  1 last saved bookmark (for next read),0 is not
	uc_ebookmark_flag |= 0x09;			//set bit0,bit3 
	uc_ebookmark_flag &= 0x0f;			//clr bit4,bit5
	uc_ebook_mark1_cluster1 = uc_fs_begin_cluster1;	//backup current txt file parameter
	uc_ebook_mark1_cluster2 = uc_fs_begin_cluster2;
	uc_ebook_mark1_length1 = (uc8)ul_fs_file_length;
	uc_ebook_mark1_length2 = (uc8)(ul_fs_file_length >> 8);
}

void Ap_Save_BookMark2(void)
{
	uc_ebookmark_flag |= 0x12;	  		//set bit1,bit4 	
	uc_ebookmark_flag &= 0x17;			//clr bit3,bit5
  	uc_ebook_mark2_cluster1 = uc_fs_begin_cluster1;	//backup current txt file parameter
	uc_ebook_mark2_cluster2 = uc_fs_begin_cluster2;
	uc_ebook_mark2_length1 = (uc8)ul_fs_file_length;
	uc_ebook_mark2_length2 = (uc8)(ul_fs_file_length >> 8);
}

void Ap_Save_BookMark3(void)
{
	uc_ebookmark_flag |= 0x24;		   	//set bit2,bit5 	
	uc_ebookmark_flag &= 0x27;			//clr bit3,bit4
  	uc_ebook_mark3_cluster1 = uc_fs_begin_cluster1;	//backup current txt file parameter
	uc_ebook_mark3_cluster2 = uc_fs_begin_cluster2;
	uc_ebook_mark3_length1 = (uc8)ul_fs_file_length;
	uc_ebook_mark3_length2 = (uc8)(ul_fs_file_length >> 8);
}


#endif