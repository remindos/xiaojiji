/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_voice_fseek.c
Creator: zj.zong					Date: 2009-05-23
Description: point to the specifically position in file
Others:
Version: V0.1
History:
	V0.1	2009-05-22		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_VOICE_FSEEK_DEF

#include "sh86278_sfr_reg.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "..\..\UI\UI_Voice\ui_voice_header.h"
#include "..\..\UI\UI_Voice\ui_voice_var_ext.h"

extern bit FS_Fseek(ul32 offset);
extern bit FS_Fread();

void Ap_FseekFor_ForwardBackward(void);
void Ap_FseekFor_ABReplay(void);
/*******************************************************************************
Function: Ap_Fseek_InFile()
Description: point to the appointed position in file
Calls:
Global:uc_dynamicload_idx:    4--fast; 8--(A-B) replay; 
   	   uc_voice_status:       bit7--1:file end; 0:file ok

Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Fseek_InFile(void)
{
	switch(uc_dynamicload_idx)
	{							 		//forward or backward
		case 30:
	   		Ap_FseekFor_ForwardBackward();
			break;

		case 31:						//A-B replay
			 Ap_FseekFor_ABReplay();
			 break;
		
		case 32:						// for seek Voice header
			 FS_Fseek(0);
			 break;

		default:
		break;
	}
	uc_dynamicload_idx = 0;

}
/*******************************************************************************
Function: Ap_FseekFor_ABReplay()
Description: point to the appointed position in file
Calls:
Global:ul_voice_replay_a_byte: A- byte offset
       ui_voice_replay_a_time: A- time
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_FseekFor_ABReplay(void)
{
	ui_voice_cur_time = ui_voice_replay_a_time;	//init current time
		
	FS_Fseek(ul_voice_replay_a_byte);			//point to the A- position
	uc_fs_assign_buf_flg = SFC_APP_DBUF1;		//assign dbuf(0x5000) to filesystem
	FS_Fread();
}



/*******************************************************************************
Function: Ap_FseekFor_ForwardBackward()
Description: point to the appointed position in file
Calls:
Global:ul_voice_byterate:
       ui_voice_cur_time:
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_FseekFor_ForwardBackward(void)
{
	ul32 tmp_byte_offset;

	tmp_byte_offset = (ul_voice_byterate * ui_voice_cur_time) & 0xfffffe00;

	if(tmp_byte_offset == 0)
	{
		tmp_byte_offset = 512;					//skip wav header(512 bytes)
	}
	
	if(FS_Fseek(tmp_byte_offset))
	{
		uc_fs_assign_buf_flg = SFC_APP_DBUF1;	//assign dbuf(0x5000) to filesystem
		FS_Fread();								//read one sector
	}
	else												 
	{
		b_voice_file_end = 1;					//bit7 file end
	}
}


#endif