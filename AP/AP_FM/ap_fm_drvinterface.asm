/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: drv_fm_interface.asm
Creator: andyliu					Date: 2009-07-18
Description: api function interface, be called by UI
Others:
Version: V0.1
History:
	V0.1	2009-07-18		andyliu
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_FM_INTERFACE

PUBLIC	Api_RadioInit
PUBLIC	_Api_RadioOpen
PUBLIC	Api_RadioClose
PUBLIC	_Api_RadioSetFreq
PUBLIC	Api_RadioGetFreq
PUBLIC	Api_RadioGetStereoFlag
PUBLIC	_Api_RadioAutoSearch
PUBLIC	_Api_RadioSetMute
PUBLIC	_Api_RadioSetStereo


	AP_FM_INTERFACE_SEG	SEGMENT		CODE
						RSEG		AP_FM_INTERFACE_SEG
/*******************************************************************************
Function:
         1. Api_RadioInit()
         2. _Api_RadioOpen()
         3. Api_RadioClose()
         4. _Api_RadioSetFreq()
         5. Api_RadioGetFreq()
         6. Api_RadioGetStereoFlag()
         7. Api_RadioAutoSearch()
         8. _Api_RadioSetMute()
         9. _Api_RadioSetStereo()
Description: api function interface, be called by UI or AP
Calls:
Global:
Input: Null
Output:Null
Others:
********************************************************************************/
Api_RadioInit:
	JMP		2500H


_Api_RadioOpen:
	JMP		2503H


Api_RadioClose:
	JMP		2506H
		
		
_Api_RadioSetFreq:
	JMP		2509H
		
		
Api_RadioGetFreq:
	JMP		250CH
		
		
Api_RadioGetStereoFlag:
	JMP		250FH
		
		
_Api_RadioAutoSearch:
	JMP		2512H
		
		
_Api_RadioSetMute:
	JMP		2515H
		
		
_Api_RadioSetStereo:
	JMP		2518H


#endif
        END