/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_music_id3.c
Creator: zj.zong					Date: 2009-04-27
Description: get id3 info and write id3 info into flash 0xa500 0059
Others:
Version: V0.1
History:
	V0.1	2009-04-27		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_MUSIC_ID3_PROCESS_DEF

#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "..\..\UI\UI_Music\ui_music_header.h"
#include "..\..\UI\UI_Music\ui_music_var_ext.h"

extern bit FS_Fread();
extern bit FS_Fseek(ul32 offset);
extern ui16 Api_Merge_2Byte_Sub(uc8 lo,uc8 hi);
extern ul32 Api_Merge_4Byte(uc8 l1,uc8 l2,uc8 h1);
extern ul32 Api_Merge_4Byte7Bit(uc8 l1,uc8 l2,uc8 h1);
extern ui16 Api_Isn_ConvertTo_Unicode(ui16 isn_index);

uc8 Ap_GetId3_Tag(uc8 *tmp_buf);
bit Ap_Read_OneSector(void);
void Ap_Process_Id3(void);
void Ap_Get_Id3(void);
void Ap_InitId3_Variable();
void Ap_GetId3_TagV2(void);
void Ap_GetId3_TagV1(void);
void Ap_GetId3_TagV2_2(void);
void Ap_GetId3_TagV2_34(void);
void Ap_GetId3_TagLen_V2_2(void);
void Ap_GetId3_TagLen_V2_34(void);
void Ap_GetVbr_Frame();
void Ap_Get_Tag2_Vbr_Frame();
void Ap_Get_Last128Byte(void);
void Ap_Check_SectorOver(void);
void Ap_Exchange_HighLow_Byte(void);
void Ap_Id3_Convert_Unicode(uc8 *tmp_buf,uc8 tmp_len);

#ifdef AP_PROCESS_ID3_DEF
/*******************************************************************************
Function: Ap_Process_Id3()
description: convert ISN into unicode, get id3 length, get id3 info buffer
Calls:
Global:uc_id3_artist_buf,uc_id3_title_buf,uc_id3_album_buf (id3 info buffer)
       uc_id3_title_length,uc_id3_artist_length,uc_id3_album_length (id3 length)
	   uc_id3_length--mp3 id3 info length
       ui_id3_buf--mp3 id3 info buffer
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Process_Id3(void)
{
	uc8 i;

	for(i = 0; i < 100; i++)			//clr id3 buffer
	{
		ui_id3_buf[i] = 0;				//id3 buffer length is 200bytes
	}

	if(uc_id3_title_length != 0)
	{
		ui_id3_byte_offset = 0;
		Ap_Id3_Convert_Unicode(uc_id3_title_buf, uc_id3_title_length);	//convert title into unicode
	}
	
	if(uc_id3_artist_length != 0)
	{
		ui_id3_buf[uc_id3_length] = 0x0020;	//"SPACE"
		uc_id3_length++;
		ui_id3_byte_offset += uc_id3_title_length + 2;
		Ap_Id3_Convert_Unicode(uc_id3_artist_buf, uc_id3_artist_length);	//convert artist into unicode
	}

	if((uc_id3_title_length == 0) || (uc_id3_artist_length == 0))
	{
		if(uc_id3_album_length != 0)
		{
			ui_id3_buf[uc_id3_length] = 0x0020;		//"SPACE"
			uc_id3_length++;
			ui_id3_buf[uc_id3_length] = 0x0020;		//"SPACE"
			uc_id3_length++;						//add 2 "space"

			ui_id3_byte_offset += uc_id3_artist_length + 2;
			Ap_Id3_Convert_Unicode(uc_id3_album_buf, uc_id3_album_length);//convert album into unicode
		}
	}
}



/*******************************************************************************
Function: Ap_Id3_Convert_Unicode()
description: if id3 is ISN, and convert ISN into unicode
             if id3 is unicode ,and not convert
Calls:
Global:uc_id3_length--mp3 id3 info length
       ui_id3_buf--mp3 id3 info buffer
Input: uc_id3_xxx_buf--xxx is title & artist & album
       uc_id3_xxx_length---xxx is title & artist & album
Output:Null
Others:
********************************************************************************/
void Ap_Id3_Convert_Unicode(uc8 *tmp_buf, uc8 tmp_len)
{
	uc8 i;
	ui16 tmp_unicode;

	if((*(tmp_buf) == 0x01) && (*(tmp_buf+1) == 0xff) && (*(tmp_buf+2) == 0xfe)) 		//0xfffe--unicode flag
	{
		for(i = 0; i < (tmp_len-3)/2; i++)
		{
			tmp_unicode = Api_Merge_2Byte_Sub(*(tmp_buf+3+2*i), *(tmp_buf+3+2*i+1));
			ui_id3_buf[uc_id3_length] = tmp_unicode;
			uc_id3_length++;
		}
	}
	else if((*(tmp_buf) == 0x01) && (*(tmp_buf+1) == 0xfe) && (*(tmp_buf+2) == 0xff))	//0xfeff--unicode flag
	{
		for(i = 0; i < (tmp_len-3)/2; i++)
		{
			tmp_unicode = Api_Merge_2Byte_Sub(*(tmp_buf+3+2*i+1), *(tmp_buf+3+2*i));
			ui_id3_buf[uc_id3_length] = tmp_unicode;
			uc_id3_length++;
		}
	}
	else			//id3 is ISN--internal statement number
	{
		for(i = 0; i < tmp_len; i++)
		{
			if(*(tmp_buf+i) >= 0x20)
			{
				tmp_unicode = Api_Isn_ConvertTo_Unicode((ui16) *(tmp_buf + i));
				if(tmp_unicode == 0)
				{
					tmp_unicode = Api_Isn_ConvertTo_Unicode(Api_Merge_2Byte_Sub(*(tmp_buf + i + 1), *(tmp_buf + i)));
					i++;
				}
				ui_id3_buf[uc_id3_length] = tmp_unicode;
				uc_id3_length++;
			}
		}
	}
}
#endif




#ifdef AP_GET_MP3_ID3_DEF
/*******************************************************************************
Function: Ap_Get_Id3()
description: get mp3 id3 info(title, artist, album)
	id3v2.2:frame header length is 6byte
		frame ID $xx xx xx (T**)
		Size     $xx xx xx (24bit(bit0~7) integer)
		Flags    $xx xx
	id3v2.3:frame header length is 10byte
		frame ID $xx xx xx xx (T***)
		Size     $xx xx xx xx (32bit(bit0~7) integer)
		Flags    $xx xx
	id3v2.4:frame header length is 10byte
		frame ID $xx xx xx xx (T***)
		Size     $xx xx xx xx (28bit(bit0~6) integer)
		Flags    $xx xx
	id3v1:  header length is 3byte, total 128byte(the last 128bytes of mp3 file )
		frame ID $xx xx xx (TAG) --4byte
		tag1:title--30byte
		tag2:artist--30byte
		tag3:album--30byte
		tag4:year--4byte
		tag5:comment--30byte
Calls:
Global:uc_id3_artist_buf,uc_id3_title_buf,uc_id3_album_buf  (id3 info buffer)
       uc_id3_title_length,uc_id3_artist_length,uc_id3_album_length  (id3 length)
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Get_Id3(void)
{
	Ap_InitId3_Variable();
	
	if(Ap_Read_OneSector() == 0)
	{
		return;
	}

	if((uc_dbuf1[0] == 'I') && (uc_dbuf1[1] == 'D') && (uc_dbuf1[2] == '3'))
	{
		Ap_GetId3_TagV2();				//get the ID3V2 TAG

		Ap_Get_Tag2_Vbr_Frame();			//discard id3 info, point to the first frame
		Ap_GetVbr_Frame();				//get total frame number of vbr mp3

		if((uc_id3_album_length == 0) && (uc_id3_artist_length == 0) && (uc_id3_title_length == 0))
		{
			Ap_InitId3_Variable();
			Ap_GetId3_TagV1();			//get the ID3V1 TAG
			ul_fs_byte_offset = 0;		//point to the first byte
			FS_Fseek(ul_id3_size & 0xfffffe00);
		}
	}
	else
	{
		Ap_GetVbr_Frame();				//get vbr "Xing"
		Ap_GetId3_TagV1();				//get the ID3V1 TAG
		ul_fs_byte_offset = 0;			//point to the first byte
		FS_Fseek(ul_id3_size & 0xfffffe00);
	}
}



/*******************************************************************************
init id3 variable
********************************************************************************/
void Ap_InitId3_Variable(void)
{
	uc8 i;

	b_id3_artist_flg = 0;
	b_id3_album_flg = 0;
	b_id3_title_flg = 0;
	b_id3_overflow_flg = 0;

	uc_id3_length = 0;
	uc_id3_artist_length = 0;
	uc_id3_title_length = 0;
	uc_id3_album_length = 0;
	ui_id3_byte_offset = 0;
	ul_id3_size = 0;

	for(i = 0; i < 90; i++)				//clr id3 tag buffer
	{
		uc_id3_artist_buf[i] = 0;
	}
}



/*******************************************************************************
Function: Ap_GetId3_TagV2()
description: get mp3 id3 V2 tag(title, artist, album)
Calls:
Global:uc_id3_artist_buf,uc_id3_title_buf,uc_id3_album_buf (id3 info buffer)
       uc_id3_title_length,uc_id3_artist_length,uc_id3_album_length (id3 length)
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_GetId3_TagV2(void)
{
	uc_id3_version = uc_dbuf1[3];		//get the id3 version

	uc_id3_temp_input = uc_dbuf1[6];
	ul_id3_size = Api_Merge_4Byte7Bit(uc_dbuf1[9], uc_dbuf1[8], uc_dbuf1[7]);
	ui_id3_byte_offset = 10;			//id3 header is 10byte

	if(ul_id3_size >= ul_fs_file_length)
	{
		b_mp3_file_end = 1;		//mp3 file end
		return;
	}
	
	if(uc_id3_version == 2)
	{
		Ap_GetId3_TagV2_2();				//get the id3v2.2 tag
	}
	else if((uc_id3_version == 3) || (uc_id3_version == 4))
	{
		Ap_GetId3_TagV2_34();				//get the id3v2.3&v2.4 tag
	}
}



/*******************************************************************************
get mp3 id3 V2.2 tag(title, artist, album)
********************************************************************************/
void Ap_GetId3_TagV2_2(void)
{
get_id3_again_1:
	Ap_Check_SectorOver();												//id3 size overflow
	if(b_id3_overflow_flg == 1)
	{
		return;
	}

	if((b_id3_album_flg == 0) || (b_id3_artist_flg == 0) || (b_id3_title_flg == 0))
	{
		if(uc_dbuf1[ui_id3_byte_offset] == 'T')							//check "T" flag
		{
			if((uc_dbuf1[ui_id3_byte_offset+1] == 'A') && (uc_dbuf1[ui_id3_byte_offset+2] == 'L'))		//check"album"
			{
				Ap_GetId3_TagLen_V2_2();
				uc_id3_album_length = Ap_GetId3_Tag(uc_id3_album_buf);	//copy buffer
				b_id3_album_flg = 1;									//set album ok flag
			}
			else if((uc_dbuf1[ui_id3_byte_offset+1] == 'P') && (uc_dbuf1[ui_id3_byte_offset+2] == '1'))	//check"artist"
			{
				Ap_GetId3_TagLen_V2_2();
				uc_id3_artist_length = Ap_GetId3_Tag(uc_id3_artist_buf);	//copy buffer
				b_id3_artist_flg = 1;									//set artist ok flag
			}
			else if((uc_dbuf1[ui_id3_byte_offset+1] == 'T') && (uc_dbuf1[ui_id3_byte_offset+2] == '2'))	//check"title"
			{
				Ap_GetId3_TagLen_V2_2();
				uc_id3_title_length = Ap_GetId3_Tag(uc_id3_title_buf);	//copy buffer
				b_id3_title_flg = 1;									//set title ok flag
			}
			else
			{
				Ap_GetId3_TagLen_V2_2();
				Ap_GetId3_Tag(uc_dbuf0);									//for others id3 tag, invalid tag
			}
		}
		else
		{
			ui_id3_byte_offset++;
		}
		goto get_id3_again_1;
	}
}



/*******************************************************************************
get mp3 id3 V2.3 & V2.4 tag(title, artist, album)
********************************************************************************/
void Ap_GetId3_TagV2_34(void)
{
get_id3_again_2:
	Ap_Check_SectorOver();		//id3 size overflow
	if(b_id3_overflow_flg == 1)
	{
		return;
	}

	if((b_id3_album_flg == 0) || (b_id3_artist_flg == 0) || (b_id3_title_flg == 0))
	{
		if(uc_dbuf1[ui_id3_byte_offset] == 'T')							//check "T" flag
		{
			if((uc_dbuf1[ui_id3_byte_offset+1] == 'A') && (uc_dbuf1[ui_id3_byte_offset+2] == 'L') 
			    && (uc_dbuf1[ui_id3_byte_offset+3] == 'B'))				//check"album"
			{
				Ap_GetId3_TagLen_V2_34();
				uc_id3_album_length = Ap_GetId3_Tag(uc_id3_album_buf);	//copy buffer
				b_id3_album_flg = 1;
			}
			else if((uc_dbuf1[ui_id3_byte_offset+1] == 'P') && (uc_dbuf1[ui_id3_byte_offset+2] == 'E') 
			         && (uc_dbuf1[ui_id3_byte_offset+3] == '1'))		//check"artist"
			{
				Ap_GetId3_TagLen_V2_34();
				uc_id3_artist_length = Ap_GetId3_Tag(uc_id3_artist_buf);	//copy buffer
				b_id3_artist_flg = 1;
			}
			else if((uc_dbuf1[ui_id3_byte_offset+1] == 'I') && (uc_dbuf1[ui_id3_byte_offset+2] == 'T') 
			         && (uc_dbuf1[ui_id3_byte_offset+3] == '2'))		//check"title"
			{
				Ap_GetId3_TagLen_V2_34();
				uc_id3_title_length = Ap_GetId3_Tag(uc_id3_title_buf);	//copy buffer
				b_id3_title_flg = 1;
			}
			else
			{
				Ap_GetId3_TagLen_V2_34();
				Ap_GetId3_Tag(uc_dbuf0);									//for others id3 tag, invalid tag
			}
		}
		else
		{
			ui_id3_byte_offset++;
		}
		goto get_id3_again_2;
	}
}



/*******************************************************************************
 v2.2 tag length is 3byte(bit0~7 is valid)
********************************************************************************/
void Ap_GetId3_TagLen_V2_2(void)
{
	uc_id3_temp_input = 0x00;
	ul_id3_tag_size = Api_Merge_4Byte(uc_dbuf1[ui_id3_byte_offset+5], uc_dbuf1[ui_id3_byte_offset+4], uc_dbuf1[ui_id3_byte_offset+3]);
	ui_id3_byte_offset += 6;			//add frame header length, id3v2.2 frame header is 6byte
}



/*******************************************************************************
 v2.3 tag length is 4byte(bit0~7 is valid)
 v2.4 tag length is 4byte(bit0~6 is valid)
********************************************************************************/
void Ap_GetId3_TagLen_V2_34(void)
{
	if(uc_id3_version == 3)
	{
		uc_id3_temp_input = uc_dbuf1[ui_id3_byte_offset + 4];
		ul_id3_tag_size = Api_Merge_4Byte(uc_dbuf1[ui_id3_byte_offset+7], uc_dbuf1[ui_id3_byte_offset+6], uc_dbuf1[ui_id3_byte_offset+5]);
	}
	else
	{
		uc_id3_temp_input = uc_dbuf1[ui_id3_byte_offset+4];
		ul_id3_tag_size = Api_Merge_4Byte7Bit(uc_dbuf1[ui_id3_byte_offset+7], uc_dbuf1[ui_id3_byte_offset+6], uc_dbuf1[ui_id3_byte_offset+5]);
	}
	ui_id3_byte_offset += 10;			//add frame header length, id3v2.3,v2.4 frame header is 10byte
}



/*******************************************************************************
Function: Ap_GetId3_TagV1()
description: get mp3 id3 V1 tag(title, artist, album), 
             id3v1 tag is the last 128byte in mp3 file
Calls:
Global:uc_id3_artist_buf,uc_id3_title_buf,uc_id3_album_buf  (id3 info buffer)
       uc_id3_title_length,uc_id3_artist_length,uc_id3_album_length  (id3 length)
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_GetId3_TagV1(void)
{
	uc8 i = 0;
	ul_id3_tag_size = 30;				//id3v1 tag length maximum is 30
	ul_id3_size = 128;
	Ap_Get_Last128Byte();				//get the last 128byte
	
search_tag:
	if(i < 128)							//total 128byte
	{
		if((uc_dbuf1[i] == 'T') && (uc_dbuf1[i+1] == 'A') && (uc_dbuf1[i+2] == 'G'))
		{
			goto find_tag;
		}
		else
		{
			i++;
			goto search_tag;
		}	
	}
	else
	{
		return;							//not find "TAG"
	}

find_tag:
	ui_id3_byte_offset = i + 3;			//get the tag address
	uc_id3_title_length = Ap_GetId3_Tag(uc_id3_title_buf);		//get the title length and the data buffer

	uc_id3_artist_length = Ap_GetId3_Tag(uc_id3_artist_buf);		//get the artist length and the data buffer

	uc_id3_album_length = Ap_GetId3_Tag(uc_id3_album_buf);		//get the album length and the data buffer
}



/*******************************************************************************
Function: Ap_GetId3_Tag()
description: process title & artist & album buffer, get the length
Calls:
Global:Null
Input: tmp_buf--id3 tag buffer start address
Output:uc_id3_tmp_length--id3 tag length
Others:
********************************************************************************/
uc8 Ap_GetId3_Tag(uc8 *tmp_buf)
{
	uc_id3_tmp_length = 0;
	
	if( (uc_dbuf1[ui_id3_byte_offset] == 0x1) && (uc_dbuf1[ui_id3_byte_offset+1] == 0xff || uc_dbuf1[ui_id3_byte_offset+2] == 0xfe))
	{
		uc_id3_tmp_var1 = 0;			//id3 info is unicode format
	}
	else
	{
		uc_id3_tmp_var1 = 0x20;			//id3 info is internal statement number(ISN)
	}

	for(ul_id3_tmp_tag_size = 0; ul_id3_tmp_tag_size < ul_id3_tag_size; ul_id3_tmp_tag_size++)
	{
		Ap_Check_SectorOver();			//id3 size overflow
		if(b_id3_overflow_flg == 1)
		{
			break;
		}

		if(uc_id3_tmp_length < 30)						//id3 tag length maximum is 30byte
		{
			if(uc_dbuf1[ui_id3_byte_offset] >= uc_id3_tmp_var1)	//if ascii is lt 0x20, and not display
			{
				*(tmp_buf+uc_id3_tmp_length) = uc_dbuf1[ui_id3_byte_offset];
				uc_id3_tmp_length++;					//valid length + 1
			}
		}
		
		ui_id3_byte_offset++;
	}
	return uc_id3_tmp_length;
}



void Ap_Get_Last128Byte(void)
{
	uc8 i, j;

	ui_id3_tmp_offset = ul_fs_file_length % 512;
	if(ui_id3_tmp_offset >= 128)
	{
		FS_Fseek(ul_fs_file_length - ui_id3_tmp_offset);	//the last sector

		if(Ap_Read_OneSector() == 0)
		{
			return;
		}

		for(i = 0; i < 128; i++)
		{
			uc_dbuf1[i] = uc_dbuf1[ui_id3_tmp_offset - 128 + i];
		}
	}
	else
	{
		FS_Fseek(ul_fs_file_length - ui_id3_tmp_offset - 512);

		if(Ap_Read_OneSector() == 0)
		{
			return;
		}
		
		for(i = 0; i < (128 - ui_id3_tmp_offset); i++)
		{
			uc_dbuf1[i] = uc_dbuf1[384 + ui_id3_tmp_offset + i];//512-128+tmp_offset+1
		}
		
		if(ui_id3_tmp_offset == 0)
		{
			goto update_decbuf_lable;
		}
		
		uc_fs_assign_buf_flg = SFC_APP_DBUF1;				//assign dbuf(0x5000) to fs
		if(FS_Fread() == 0)									//get the last sector
		{
			goto update_decbuf_lable;
		}
		Ap_Exchange_HighLow_Byte();
		
		for(j = 0; i < 128; i++, j++)
		{
			uc_dbuf0[i] = uc_dbuf1[j];
		}

update_decbuf_lable:
		for(i = 0; i < 128; i++)
		{
			uc_dbuf1[i] = uc_dbuf0[i];	//copy to decbuf from playbuf0
		}
	}
}



/*******************************************************************************
exchange high with low byte
********************************************************************************/
void Ap_Exchange_HighLow_Byte(void)
{
	ui16 i;
	for(i = 0; i < 256; i++)						//data in memory 0x5000
	{
		uc_id3_tmp_var2 = uc_dbuf1[i * 2];
		uc_dbuf1[i * 2] = uc_dbuf1[i * 2 + 1];
		uc_dbuf1[i * 2 + 1] = uc_id3_tmp_var2;
	}
}



/*******************************************************************************
check id3 size overflow, check file end
********************************************************************************/
void Ap_Check_SectorOver(void)
{
	if(b_id3_overflow_flg == 1)
	{
		return;
	}
	if(ui_id3_byte_offset >= 512)		//get the mp3 file next sector
	{
		ui_id3_byte_offset -= 512;
		
		if(ul_fs_byte_offset < ul_id3_size)
		{
			if(Ap_Read_OneSector() == 0)
			{
				b_id3_overflow_flg = 1;	//set id3 overflow flag
				return;
			}
				
			b_id3_overflow_flg = 0;		//ok
			return;
		}
		else
		{
			b_id3_overflow_flg = 1;	//id3 size overflow
			return;
		}
	}
	else if((long)ui_id3_byte_offset > ul_id3_size)
	{
		b_id3_overflow_flg = 1;		//id3 overflow
		return;
	}
	else
	{
		b_id3_overflow_flg = 0;		//ok
		return;
	}
}



/*******************************************************************************
Function: Ap_GetVbr_Frame()
description: check vbr mp3, if it's vbr and get the total frame number
Calls:
Global:ul_mp3_vbr_total_frame: VBR MP3 total frame number
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_GetVbr_Frame(void)
{
	ui16 i,j;

	ul_mp3_vbr_total_frame = 0;
	uc_vbr_offset = 0;
	uc_vbr_decbuf_backup_511 = 0;

search_frame_header:
	for(i = 0; i < 512; i++)
	{
		if(i == 0)
		{
			if((uc_vbr_decbuf_backup_511 == 0xff) && ((uc_dbuf1[0]&0xe0) == 0xe0))	//0xFFE0
			{
				if((uc_dbuf1[2] & 0xc0) != 0xc0)	//stereo
				{
					uc_vbr_offset = 36;
				}
				else								//mono
				{
					uc_vbr_offset = 21;
				}
				goto find_frame_header;
			}
		}

		if(i == 511)
		{
			uc_vbr_decbuf_backup_511 = uc_dbuf1[511];
		}
		else
		{
			if((uc_dbuf1[i] == 0xff) && ((uc_dbuf1[i+1]&0xe0) == 0xe0))		//0xFFE0
			{
				if((uc_dbuf1[i+3]&0xc0) != 0xc0)	//stereo
				{
					uc_vbr_offset = 36;
				}
				else								//mono
				{
					uc_vbr_offset = 21;
				}
				goto find_frame_header;
			}
		}
	}

	if(Ap_Read_OneSector() == 0)		 		// data in meory 0x5000
	{
		return;
	}

	goto search_frame_header;

find_frame_header:
	for(j = 0; j < 512; j++)
	{
		uc_dbuf0[j] = uc_dbuf1[j];
	}

	if(Ap_Read_OneSector() == 0)
	{
		return;
	}

	for(j = 0; j < 512; j++)
	{
		uc_dbuf0[j + 512] = uc_dbuf1[j];
	}

	if((uc_dbuf0[i+uc_vbr_offset]=='X') && (uc_dbuf0[i+uc_vbr_offset+1]=='i') 
	    && (uc_dbuf0[i+uc_vbr_offset+2]=='n') && (uc_dbuf0[i+uc_vbr_offset+3]=='g'))	//"Xing VBR"
	{
		goto get_vbr_frame_num;
	}
	else if((uc_dbuf0[i+36]=='V') && (uc_dbuf0[i+37]=='B') && (uc_dbuf0[i+38]=='R') && (uc_dbuf0[i+39]=='I'))	//"VBRI"
	{
		uc_vbr_offset = 42;
get_vbr_frame_num:
		ul_mp3_vbr_total_frame |= uc_dbuf0[i + uc_vbr_offset + 8] << 24;			 //data in memory 0x4000
		ul_mp3_vbr_total_frame |= uc_dbuf0[i + uc_vbr_offset + 9] << 16;
		ul_mp3_vbr_total_frame |= uc_dbuf0[i + uc_vbr_offset + 10] << 8;
		ul_mp3_vbr_total_frame |= uc_dbuf0[i + uc_vbr_offset + 11];
	}
}



/*******************************************************************************
discard id3 info, point to the first frame
********************************************************************************/
void Ap_Get_Tag2_Vbr_Frame(void)
{
	FS_Fseek((ul_id3_size+10)&0xffffffe0);	
	goto read_data;

discard_id3_info:
	if(ul_fs_byte_offset > (ul_id3_size + 10))
	{
		return;
	}

read_data:
	if(Ap_Read_OneSector() == 0)
	{
		return;
	}

	goto discard_id3_info;
}



bit Ap_Read_OneSector(void)
{
	uc_fs_assign_buf_flg = SFC_APP_DBUF1;	//assign dbuf(0x5000) to fs
	if(FS_Fread() == 0)					//get the first sector(512bytes id3 info), output is "decbuf"
	{
		b_mp3_file_end = 1;				//mp3 file end
		return 0;						//read file end
	}
	
	Ap_Exchange_HighLow_Byte();
	return 1;
}
#endif

#endif