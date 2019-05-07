/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_poweroff.c
Creator: zj.zong					Date: 2009-04-22
Description:
Others:
Version: V0.1
History:
	V0.1	2009-04-22		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_POWEROFF_DEF

#include "sh86278_api_define.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"

extern void Api_Delay_1ms(uc8 time_cut);

/*******************************************************************************
Function: Ap_Waiting_KeyPlay_Release()
Description: waiting key play release
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Waiting_KeyPlay_Release(void)
{
Label_Wait_ReleasePlay:
	mReset_WatchDog;

	if((MR_PADATA & PLAYKEY_MASK) != 0)		//wating for play key release
	{
		goto Label_Wait_ReleasePlay;
	}
	else
	{
		Api_Delay_1ms(20);
		if((MR_PADATA & PLAYKEY_MASK) != 0)
		{
			goto Label_Wait_ReleasePlay;
		}
		else
		{
			Api_Delay_1ms(20);
			if((MR_PADATA & PLAYKEY_MASK) != 0)
			{
				goto Label_Wait_ReleasePlay;
			}
		}
	}
}


/*******************************************************************************
Function: Ap_Delay_1s()
Description: delay 1s
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Delay_1s(void)
{
	Api_Delay_1ms(250);	
	Api_Delay_1ms(250);
	Api_Delay_1ms(250);
	Api_Delay_1ms(250);
}

#endif	