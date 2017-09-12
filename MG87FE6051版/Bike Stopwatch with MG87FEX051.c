#include <REG_MG87FL2051-6051.H>
#include <stdio.h>


//==== 宣告 DS1821 專用register ========
#define DQ_IO P37
unsigned char NUMBER[3];
char code TABLE[13]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x20,0x2d,0x2e};//  空白- .
char TEMP;
bit bdata work=0;
//==== DS1821.h必須include在以上各register之後
#include <DS1821.h>
//==== 宣告 DS1821 專用register 結束 ===


//==== 宣告 LCM 專用register ========
#define RS P35
#define RW P34 //0寫1讀
#define LCD_Enable  P33	//負緣觸發
//以下兩個define使用的Port必須相同
#define BF   P10
#define LCDP P1
//==== LCM.h必須include在以上各register之後
#include <LCM.h>
//==== 宣告 LCM 專用register 結束 ===


//==== 宣告 速度表 專用register ========
#define MagneticSW P32 		//前輪磁性開關,start/stop 接至P3.2(INT0)
/*宣告T0計時相關宣告*/  // THx TLx 計算參考 7-9頁 
#define  count_M1   3694		// T0(MODE 1)之計量值,1mSec @ 44326800Hz,SYSCLK/12
#define  TH_M1  (65636-count_M1)>>8// T0(MODE 1)計量高8位元 
#define  TL_M1  (65636-count_M1)&0x00FF// T0(MODE 1)計量低8位元
/* 宣告基本變數 */
unsigned int  mSecCounter=0;		// 每個觸發間隔(每圈)mSec數
unsigned int  unDisp_mSec=65536;		// 顯示用的mSec數
unsigned int  unDispSpeed=0;			// 顯示用的速率
unsigned int  unDispMileage=0;		//顯示用的累計里程
unsigned int  unCycleCounter=0;		// 累計圈數,計算里程用,26in*pi*65535=136km(最大計程)
void debouncer(void); 		// 宣告防彈跳函數
//==== 宣告 速度表 專用register結束 ========


//==== 開機畫面 ========
unsigned char code MSG_Welcome_Line1[]="  NTNU Lin Kao-Yuan ";
unsigned char code MSG_Welcome_Line2[]="Bicycle's MCU System";
unsigned char code MSG_Welcome_Line3[]="     0958889115";
unsigned char code MSG_Welcome_Line4[]="  mosdeo@gmail.com";
//==== 開機畫面 ========

unsigned char StringForLCD_Line1[13]="";
unsigned char StringForLCD_Line2[19]="";
unsigned char countMOD=0;	

// void P42XOR(void);
// void GetTempByDS1821(void);
// void TxSpeed(void);
// void TxTempearture(void);
// void putnum(unsigned int inputNum);

#include "function.h"

main()
{
	IE=0x00;
	EA=ET0=EX0=ES=1;
	IP=0x00;
	PT0=1;  // TF0最優先(for mSec計算)
	SCON=0x70; /*Serial Port mode2*/
	TCON=0x00,// 0000 0000 設定INT0 採位準觸發(6-4頁)
	IT0=1;//不可用位準觸發,否則整個磁鐵經過的時間都會中斷

	TMOD=0x21;				// 0010 0001,T1採mode 2、T0採mode 1
	TH0=TH_M1; TL0=TL_M1;	// 設置T0計數量高8位元、低8位元 
	TH1=TL1=250; 
	
	MagneticSW=1;		// 規劃MagneticSW輸入 
	
	init_LCM();	//取代清除螢幕
	NewType();
	
	write_inst(0x80);write_string(MSG_Welcome_Line1);
	write_inst(0xC0);write_string(MSG_Welcome_Line2);
	write_inst(0x94);write_string(MSG_Welcome_Line3);
	write_inst(0xD4);write_string(MSG_Welcome_Line4);
	delay1ms(3000);
	
	TR1=1;TR0=1;					// 啟動T1,T0


	while(1)
	{			
		// Visable WDT
		P43=~P43;
		
		//=== 複雜的計算要在清除螢幕前完成，不然花太多時間put char到LCD，螢幕容易閃動
		unDispSpeed=(unsigned int)(7468/unDisp_mSec); //計算速度
		unDispMileage=(unsigned int)(2.075*unCycleCounter
		//unCycleCounter<<1	//*2
		//+unCycleCounter>>4	//*0.0625
		//+unCycleCounter>>6	//*0.015625
		); //計算累計里程,圈數*2.075
		
		// 產生準備顯示在第一二行的字串
		sprintf(StringForLCD_Line1,"Speed:%2dkm/h",unDispSpeed);
		if(9999>=unDispMileage)
			sprintf(StringForLCD_Line2,"Total:%4dm,Temp:%c%c",unDispMileage,TABLE[NUMBER[1]],TABLE[NUMBER[2]]);
		else
			sprintf(StringForLCD_Line2,"Total:%7dm,T:%c%c",unDispMileage,TABLE[NUMBER[1]],TABLE[NUMBER[2]]);
		
		NewType();
		init_LCM(); //取代清除螢幕
		//第一行顯示
			write_inst(0x80);write_string(StringForLCD_Line1);
		//第二行顯示
			write_inst(0xC0);write_string(StringForLCD_Line2); write_inst(0xD3);write_char(0x00); //顯示度C   
		
		
		//第三,四行交替顯示
		if(64>countMOD%128){
		  write_inst(0x94);write_string(MSG_Welcome_Line1);
			write_inst(0xD4);write_string(MSG_Welcome_Line2);
		}
		else{
			write_inst(0x94);write_string(MSG_Welcome_Line3);
			write_inst(0xD4);write_string(MSG_Welcome_Line4);
		}
		++countMOD;
		
		
			// 不太重要的資訊最後顯示
		  //(countMOD>>4)%5 ,將countMOD的頻率除以16,取0~4的週期變化
			write_inst(0x8D +(countMOD>>4)%5);write_char(0x01); //顯示"林"
 			write_inst(0x8E +(countMOD>>4)%5);write_char(0x02); //顯示"高"
 			write_inst(0x8F +(countMOD>>4)%5);write_char(0x03); //顯示"遠"
		
		GetTempByDS1821();
		
		delay1ms(25);
		
		
	}

}


void serial_INT(void) interrupt 4
{//PC擷取溫度用
		unsigned char ucQUERY;	
	
		if(RI)
		{		
			ucQUERY=SBUF;
			RI=0;
			
			switch(ucQUERY)
			{
				case '3':
					TxAll();
					break;
				case 'L':
					P42XOR();
					break;
				case 'T':
					TxTempearture();
					break;
				case 'S':
					TxSpeed();
					break;
				default:
					break;
			}
		}
}

//== T0中斷副程式- 毫秒計數器，中斷順位最高(PT0=1) ===================
void T0_1s(void) interrupt 1// T0中斷副程式開始 
{	TH0=TH_M1; TL0=TL_M1;	// 設置T0計數量高8位元、低8位元 
	mSecCounter++;
	
	// 超過5秒沒感應(==時速低於1.5km/hr)就時速=0
	(5000<=mSecCounter)&&(unDisp_mSec=65534,mSecCounter=65534);
}							// T0中斷副程式結束  
//==int0中斷副程式- 碼表之 感應 ==================
void int0_sw(void) interrupt 0	// int0中斷副程式開始 
{	
	unDisp_mSec=mSecCounter; //存下數值
	mSecCounter=0;		// 歸零
	unCycleCounter++; //每次中斷就加一圈
	
}								// int 0中斷副程式結束  
// //===防彈跳函數=====================================
// void debouncer(void)			// 防彈跳函數開始 
// {	int i;						// 宣告變數i
// 	for(i=0;i<2400;i++); 		// 連數2400次，約20ms
// }								// 防彈跳函數結束 

// // 40km/hr => 0.1867秒/圈＠26"單車
// // 除非時速達到 (0.1867/0.02)*40=373.4km/hr
// // 否則僅20ms的防彈跳延遲函數不影響計速

