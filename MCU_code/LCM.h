//	LCM 函式庫
//	作者:林高遠
//	日期:2013/1/22
//	本函式庫修改自<<例說89S51-C語言 張義和>>所提供的程式碼
//	並加以改良
//	使用前需要 #include<string.h>,#include<intrins.h>
#include <intrins.h>
#include <string.H>
#define _nop_() _nop_();_nop_();_nop_();_nop_()

char InvSquence(char InChar)
{
	char s = 0x00;
	unsigned char i;
	for (i = 0; i < 8; ++i){
		s <<= 1;
		s |= InChar & 1;
		InChar >>= 1;
	}
	return s;
}

//====檢查忙碌函數================================
inline;void check_BF(void)
{	LCD_Enable=0;					// 禁止讀寫動作 
	do						// do-while迴圈開始 
	{ 	BF=1;				// 設定BF為輸入 
  	RS = 0; RW = 1;LCD_Enable = 1;	// 讀取BF及AC
	}while(BF == 1);		// 忙碌繼續等 
}							//	check_BF()函數結束 
//==== 延遲函數 ================================
void delay1ms(int x)
{	int i,j;				// 宣告變數 
	for (i=1;i<4*x;i++)		// 執行x次,延遲X*1ms
		for (j=1;j<120;j++)
			;// 執行120次,延遲1ms
}							// delay1ms()函數結束
//==== 寫入指令函數 ================================
void write_inst(char inst)
{	check_BF();				// 檢查是否忙碌  
	RS = 0; RW = 0; LCD_Enable = 1;	// 
	LCDP = InvSquence(inst);			// 寫入指令至LCM
	_nop_(); //延遲1us
	LCD_Enable=0;
	check_BF();				// 檢查是否忙碌 
}							// write_inst()函數結束 
//==== 寫入字元資料函數 ============================
void write_char(char chardata)
{	check_BF();				// 檢查是否忙碌 
	LCDP = InvSquence(chardata);		// LCM讀入字元 
	RS = 1; RW = 0 ;LCD_Enable = 1;	// 寫入資料至LCM
	_nop_(); //延遲1us
	check_BF();				// 檢查是否忙碌 
}							// write_char()函數結束
//==== 寫入字串資料函數 ===( by 林高遠 )============
void write_string(char *ptrStr)
{	unsigned char i;
	for(i=0;i<strlen(ptrStr);i++)
		write_char(ptrStr[i]);
}							// write_string()函數結束
//====初始設定函數(8位元傳輸模式)===================
void init_LCM(void)
{	write_inst(0x30);	// 設定功能-8位元-基本指令 
	write_inst(0x30);	// 設定功能-8位元-基本指令 
	write_inst(0x38);	// 英文LCM相容設定，中文LCM可忽略 
	write_inst(0x38);	// 英文LCM設定兩列，中文LCM可忽略 
	write_inst(0x08);	// 顯示功能-關顯示幕-無游標-游標不閃 
	write_inst(0x01);	// 清除顯示幕(填0x20,I/D=1)
	write_inst(0x0e);	// 輸入模式-位址遞增-關顯示幕 
	write_inst(0x06);	// 顯示功能-開顯示幕-無游標-游標不閃 
}						// init_LCM()函數結束 
//====寫入自建字形===================
void NewType(void)
{	char i;
	char code cDegreesC[]={0x1C,0x14,0x1b,0x04,0x04,0x04,0x03,0x00};
	char code cLIN[]= {0x0A,0x1F,0x0A,0x0E,0x0E,0x1B,0x0A,0x00};
	char code cGAO[]= {0x04,0x1F,0x0A,0x0E,0x1F,0x15,0x11,0x00};
	char code cYUAN[]={0x1A,0x07,0x0A,0x1D,0x0F,0x1A,0x17,0x00};	
	
	//	寫入度C的符號
	write_inst(0x40);//指定GCRAM位址
	for(i=0;i<8;i++)
		write_char(cDegreesC[i]);

	//	寫入"林"
	write_inst(0x48);//指定GCRAM位址
	for(i=0;i<8;i++)
		write_char(cLIN[i]);
	
	//	寫入"高"
	write_inst(0x50);//指定GCRAM位址
	for(i=0;i<8;i++)
		write_char(cGAO[i]);
	
	//	寫入"遠"
	write_inst(0x58);//指定GCRAM位址
	for(i=0;i<8;i++)
		write_char(cYUAN[i]);
}

