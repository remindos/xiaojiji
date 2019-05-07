/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_ebook_header.h
Creator: zj.zong			Date: 2009-05-04
Description: define macro in ebook mode
Others:
Version: V0.1
History:
	V0.1	2009-05-04		zj.zong
			1. build this module
********************************************************************************/
#ifndef EBOOK_HEADER
#define EBOOK_HEADER


#define	EBOOK_SFC_START_ADDR			0x009A

/***********************Ebook Menu Number Define***********************/
#define Ebook_MenuFlash_Num			3		 //allen
#define Ebook_MenuSD_Num			4		 //allen
/*********************Window Define in voice mode**********************/
#define	WINDOW_EBOOK				0
#define WINDOW_EBOOK_MENU_SD		1			//allen	
#define WINDOW_EBOOK_MENU_FLASH		2
#define	WINDOW_EBOOK_BROWSER		3			//allen
#define	WINDOW_EBOOK_DELETE			4			
#define WINDOW_EBOOK_DELETE_ALL		5			//allen
/*****************ebook Display Column & Row Address*******************/

#define	DIS_EBOOK_X					0
#define	Dis_BrowserItem_MaxNum		3
#define	DIS_EBOOK_LINE_MAX_NUM		3
#define	DIS_EBOOK_X_END_ADDR		112
#define	DIS_EBOOK_PAGE_MAX_NUM		32768
#define DisMenu_MaxNum			3//6

#endif