/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_poweron_dynamicload.c
Creator: zj.zong					Date: 2009-04-24
Description: interface of poweron for dynamic loading functions
Others:
Version: V0.1
History:
	V0.1	2009-04-24		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef	AP_DYNAMICLOAD_POWERON_DEF

#include "sh86278_sfr_reg.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"

extern void Api_DynamicLoad_CodeCpm(void);		  //allen
extern void Api_DynamicLoad_CodeMpm1_8kExecute(void);
void Ap_PowerOn_DynamicLoad_MainMenu(void);

void Ap_PowerOn_DynamicLoad_Music(void);
void Ap_PowerOn_DynamicLoad_Record(void);
void Ap_PowerOn_DynamicLoad_Voice(void);
void Ap_PowerOn_DynamicLoad_FM(void);
void Ap_PowerOn_DynamicLoad_System(void);
void Ap_PowerOn_DynamicLoad_Ebook(void);
void Ap_PowerOn_DynamicLoad_Telbook(void);

/*******************************************************************************
Function:   Ap_PowerOn_DynamicLoad_MainMenu()
Description: interface for dynamic loading Main Menu
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ap_PowerOn_DynamicLoad_MainMenu(void)
{
	uc_load_code_idx = CODE_MAINMENU_IDX;
	Api_DynamicLoad_CodeMpm1_8kExecute();	//load code into MPM1
}

/*******************************************************************************
Function:   Ap_PowerOn_DynamicLoad_Music()
Description: interface for dynamic loading Music mode
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ap_PowerOn_DynamicLoad_Music(void)
{
	uc_load_code_idx = CODE_MUSIC_0_MAIN_CPM_IDX;
	Api_DynamicLoad_CodeCpm();					//load code into CPM //allen
	uc_load_code_idx = CODE_MUSIC_0_MAIN_IDX;
	Api_DynamicLoad_CodeMpm1_8kExecute();		//load code into MPM1
}

/*******************************************************************************
Function:   Ap_PowerOn_DynamicLoad_Record()
Description: interface for dynamic loading Record mode
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ap_PowerOn_DynamicLoad_Record(void)
{
	uc_load_code_idx = CODE_RECORD_0_MAIN_CPM_IDX;
	Api_DynamicLoad_CodeCpm();					//load code into CPM //allen
	uc_load_code_idx = CODE_RECORD_0_MAIN_IDX;
	Api_DynamicLoad_CodeMpm1_8kExecute();		//load code into MPM1
}


/*******************************************************************************
Function:   Ap_PowerOn_DynamicLoad_Voice()
Description: interface for dynamic loading Voice mode
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ap_PowerOn_DynamicLoad_Voice(void)
{	
    uc_load_code_idx = CODE_VOICE_0_MAIN_CPM_IDX;
	Api_DynamicLoad_CodeCpm();					//load code into CPM //allen
	uc_load_code_idx = CODE_VOICE_0_MAIN_IDX;
	Api_DynamicLoad_CodeMpm1_8kExecute();		//load code into MPM1
}


/*******************************************************************************
Function:   Ap_PowerOn_DynamicLoad_FM()
Description: interface for dynamic loading FM mode
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ap_PowerOn_DynamicLoad_FM(void)
{
	uc_load_code_idx = CODE_FM_0_MAIN_IDX;
	Api_DynamicLoad_CodeMpm1_8kExecute();		//load code into MPM1
}

/*******************************************************************************
Function:   Ap_PowerOn_DynamicLoad_Ebook()
Description: interface for dynamic loading Ebook mode
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ap_PowerOn_DynamicLoad_Ebook(void)
{
    uc_load_code_idx = CODE_EBOOK_0_MAIN_CPM_IDX;
	Api_DynamicLoad_CodeCpm();					//load code into CPM //allen
	uc_load_code_idx = CODE_EBOOK_0_MAIN_IDX;
	Api_DynamicLoad_CodeMpm1_8kExecute();		//load code into MPM1
}

/*******************************************************************************
Function:   Ap_PowerOn_DynamicLoad_System()
Description: interface for dynamic loading System mode
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ap_PowerOn_DynamicLoad_System(void)
{
	uc_load_code_idx = CODE_SYS_MODE_IDX;
	Api_DynamicLoad_CodeMpm1_8kExecute();		//load code into MPM1
}

/*******************************************************************************
Function:   Ap_PowerOn_DynamicLoad_Telbook()
Description: interface for dynamic loading Telbook mode
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ap_PowerOn_DynamicLoad_Telbook(void)
{
	uc_load_code_idx = CODE_TEL_0_MAIN_IDX;
	Api_DynamicLoad_CodeMpm1_8kExecute();		//load code into MPM1
}



#endif