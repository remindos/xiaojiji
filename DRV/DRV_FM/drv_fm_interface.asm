/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: drv_fm_interface.asm
Creator: andyliu					Date: 2009-07-18
Description: api function interface, be called by UI or AP
Others:
Version: V0.1
History:
	V0.1	2009-07-18		andyliu
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef DRV_FM_INTERFACE

EXTRN	CODE	(Api_RadioInit)
EXTRN	CODE	(_Api_RadioOpen)
EXTRN	CODE	(Api_RadioClose)
EXTRN	CODE	(_Api_RadioSetFreq)
EXTRN	CODE	(Api_RadioGetFreq)
EXTRN	CODE	(Api_RadioGetStereoFlag)
EXTRN	CODE	(_Api_RadioAutoSearch)
EXTRN	CODE	(_Api_RadioSetMute)
EXTRN	CODE	(_Api_RadioSetStereo)

/*	API_FM_INTERFACE_SEG	SEGMENT		CODE
						RSEG		API_FM_INTERFACE_SEG*/

/*******************************************************************************
Function:
         1. Api_RadioInit()
         2. _Api_RadioOpen()
         3. Api_RadioClose()
         4. _Api_RadioSetFreq()
         5. Api_RadioGetFreq()
         6. Api_RadioGetStereoFlag()
         7. _Api_RadioAutoSearch()
         8. _Api_RadioSetMute()
         9. _Api_RadioSetStereo()
Description: api function interface, be called by UI or AP
Calls:
Global:
Input: Null
Output:Null
Others:
********************************************************************************/
CSEG	AT		2500H
		JMP		Api_RadioInit


CSEG	AT		2503H
		JMP		_Api_RadioOpen


CSEG	AT		2506H
		JMP		Api_RadioClose
		
		
CSEG	AT		2509H
		JMP		_Api_RadioSetFreq
		
		
CSEG	AT		250CH
		JMP		Api_RadioGetFreq 
		
		
CSEG	AT		250FH
		JMP		Api_RadioGetStereoFlag
		
		
CSEG	AT		2512H
		JMP		_Api_RadioAutoSearch
		
		
CSEG	AT		2515H
		JMP		_Api_RadioSetMute
		
		
CSEG	AT		2518H
		JMP		_Api_RadioSetStereo


#endif
        END