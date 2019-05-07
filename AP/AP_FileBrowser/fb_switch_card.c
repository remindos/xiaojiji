/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: fb_switch_card.c
Creator: aj.zhao					Date: 2009-06-11
Description: init sd/mmc card and switch card
Others:
Version: V0.1
History:
	V0.1	2009-06-11		aj.zhao
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef FB_SWITCH_CARD_DEF

#include "sh86278_sfr_reg.h"
#include "sh86278_api_define.h"
#include "ui_resident_var_ext.h"
#include "ui_resident.h"

#ifdef TARGET_Movie_B_SwitchCard
	#include "..\..\UI\Ui_Movie\ui_movie_var_ext.h"
#endif
#ifdef TARGET_Music_C_SwitchCard
	#include "..\..\UI\Ui_Music\ui_music_var_ext.h"
#endif 
#ifdef TARGET_Record_8_SwitchCard
	#include "..\..\UI\Ui_Record\ui_rec_var_ext.h"
#endif
#ifdef TARGET_Voice_8_SwitchCard
	#include "..\..\UI\Ui_Voice\ui_voice_var_ext.h"
#endif
#ifdef TARGET_Photo_8_SwitchCard
	#include "..\..\UI\Ui_Photo\ui_photo_var_ext.h"
#endif
#ifdef TARGET_Ebook_8_SwitchCard
	#include "..\..\UI\Ui_Ebook\ui_ebook_var_ext.h"
#endif
	


extern bit	FS_Finit(void);
extern void MSI_Init(void);
extern void Api_DynamicLoad_PowerOff(void);
extern void Api_Delay_1ms(uc8 timer_cnt);

#ifndef TARGET_Record_8_SwitchCard
#ifndef	TARGET_Voice_8_SwitchCard
extern void FS_EnterRootDir(void);
extern void FS_GetItemNum_InDir(void);
void Fb_SwitchTo_Root(void);
#endif
#endif

/*******************************************************************************
Function: Fb_SwitchCard()
Description: include(FS_Finit and MSI_Init)
Calls:
Global:uc_dynamicload_idx, uc_msi_status
Input: Null
Output:Null
Others:
*******************************************************************************/
void Fb_SwitchCard(void)
{
//		uc8 i,j=0;
	switch(uc_dynamicload_idx)	
	{
		case 30:
			if(FS_Finit()==0)
			{
				uc_poweroff_type = POWEROFF_FLASH_ERR;
				Api_DynamicLoad_PowerOff();
			}
			/*for(j=0; j<4; j++)
			{
				while(FS_Finit()==0)
				{
					i++;
					if(i>=6)
					{
						uc_poweroff_type = POWEROFF_FLASH_ERR;
						Api_DynamicLoad_PowerOff();
					}
					Api_Delay_1ms(80);
				}*/
/*(	switch(uc_dynamicload_idx)	
	{
		case 30:

			{
				if(FS_Finit()==0)
				{
	
				uc_poweroff_type = POWEROFF_FLASH_ERR;
				Api_DynamicLoad_PowerOff();
			    }
	}*/	
		//	}		
			break;

		case 31:
			MSI_Init();								//init msi
			break;
			
		case 32:
#ifndef TARGET_Record_8_SwitchCard
#ifndef TARGET_Voice_8_SwitchCard
			Fb_SwitchTo_Root();
#endif
#endif
			break;	
	}

	uc_dynamicload_idx = 0;
}


/*******************************************************************************
Function: Fb_SwitchTo_Root()
Description:enter root dir after switch card from flash(sd card) to sd card(flash)
			and get item num for display browser 
Calls:
Global:ui_fb_top_item_idx, ui_fb_sel_item_idx
Input: Null
Output:Null
Others:
*******************************************************************************/
#ifndef TARGET_Record_8_SwitchCard
#ifndef TARGET_Voice_8_SwitchCard
void Fb_SwitchTo_Root(void)
{
	FS_EnterRootDir();
	FS_GetItemNum_InDir();
	ui_fb_top_item_idx = 0;
	ui_fb_sel_item_idx = 0;
}
#endif
#endif

#endif