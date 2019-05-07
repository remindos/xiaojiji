/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_rec_header.h
Creator: zj.zong			Date: 2009-06-11
Description: define Macro in record mode
Others:
Version: V0.1
History:
	V0.1	2009-06-11		zj.zong
			1. build this module
********************************************************************************/
#ifndef RECORD_HEADER
#define RECORD_HEADER


/******************************timer define***************************/
#define REC_1S_TIME			2

/***************************music play state**************************/
#define STATE_RECORD		0x01
#define STATE_PAUSE			0x02
#define STATE_STOP			0x03

/*********************Window Define In Record mode**********************/
#define	WINDOW_REC		        	0x00
#define	WINDOW_REC_MENU_QUALITY		0x03
#define	WINDOW_REC_MENU_SD	        0x01
#define	WINDOW_REC_MENU_FLASH	    0x02
/*****************Music Display Column & Row Address*******************/
#define DisMenu_MaxNum			3//6


#endif