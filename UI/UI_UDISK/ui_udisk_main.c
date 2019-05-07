
/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_udisk_main.c
Creator: zj.zong			Date: 2009-07-16
Description:
Others:
Version: V0.1
History:
	V0.1	2009-07-16		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"

#ifdef	UI_UDISK_MAIN
#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "sh86278_sfr_reg.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include ".\UI\UI_UDISK\ui_udisk_var_ext.h"

extern void Ui_MsgManager(void);
extern void Ui_Init_USBVar(void);
extern void Ui_Load_PowerOn(void);
extern void Ui_Display_USBWindow(uc8 USBStatus);

extern bit  UDiskInitAll(void);
extern void UDiskHandle(void);
extern void UDdiskExit(void);
extern unsigned char UDiskGetStatus(void);
extern void _sfc_phy_read_page(void);
extern void _media_readsector(void);
//extern void check_led(void);

/*******************************************************************************
Function: udisk_main()
Description: init 
Calls:
Global:Null
Input: Null
Output:Null
Others:
******************************************************************************/

void udisk_main(void)
{
	mReset_WatchDog;					//clr watchdog
	mSet_MCUClock_22M;
//ui_init
	Ui_Init_USBVar();
	if(!UDiskInitAll())
		return;

	while(1)
	{
		mReset_WatchDog;					//clr watchdog
		Ui_MsgManager();
		UDiskHandle();			//UDiskModule
		USBStatus = UDiskGetStatus();	//UDisk_API	
		if((USBStatus == 0x30)||(USBStatus == 0x40)||(USBStatus == 0x50)||(USBStatus == 0x60))
			break;
		Ui_Display_USBWindow(USBStatus);
		//if(usb_startflag != 0)
		//{
			MR_PADATA |= 0x78;
		//	MR_PADATA &= 0x87;		
		/*}
		else
		{
			check_led();
		}
		  */
		if((MR_USBCON & 0x1) == 0) break;	//wang
	}	

	UDdiskExit();  			//UDiskModule
	Ui_Load_PowerOn();		//
}


#endif


