#include <AT89X52.H>
#include <stdio.H>

//==== 宣告 DS1821 專用register ========
sbit DS1821_Vcc=P0^0;
sbit DQ_IO=P0^1;
sbit DS1821_GND=P0^2;
unsigned char NUMBER[3];
char code TABLE[13]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x20,0x2d,0x2e};//  空白- .
char TEMP;
bit bdata work=0;
//==== DS1821.h必須include在以上各register之後
#include <DS1821.h>
//==== 宣告 DS1821 專用register 結束 ===


//==== 宣告 LCM 專用register ========
#define RS P0_4
#define RW P0_5 //0寫1讀
#define LCD_Enable  P0_6	//負緣觸發
//以下兩個define使用的Port必須相同
#define BF P2_7
#define LCDP P2
//==== LCM.h必須include在以上各register之後
#include <LCM.h>
//==== 宣告 LCM 專用register 結束 ===


//==== 宣告 速度表 專用register ========
sbit MagneticSW_Vcc=P3^1;
sbit MagneticSW=P3^2; 		//前輪磁性開關,start/stop 接至P3.2(INT0)(德源燒錄版PB7)
sbit MagneticSW_GND=P3^3;
/*宣告T0計時相關宣告*/  // THx TLx 計算參考 7-9頁 
#define  count_M1   1000			// T0(MODE 1)之計量值,1mSec 
#define  TH_M1  (65636-count_M1)>>8// T0(MODE 1)計量高8位元 
#define  TL_M1  (65636-count_M1)&0x00FF// T0(MODE 1)計量低8位元
unsigned char Line1_MsgOfLCD[16]=""; //顯示在LCD第一行的字串
/* 宣告基本變數 */
unsigned int  mSecCounter=0;		// 每個觸發間隔(每圈)mSec數
unsigned int  unDisp_mSec=65536;		// 顯示用的mSec數
unsigned int  unDispSpeed=0;			// 顯示用的速率
unsigned int  unDispMileage=0;		//顯示用的累計里程
unsigned int  unCycleCounter=0;		// 累計圈數,計算里程用,26in*pi*65535=136km(最大計程)
void debouncer(void); 		// 宣告防彈跳函數
//==== 宣告 速度表 專用register結束 ========


//==== 開機畫面 ========
unsigned char code MSG_Welcome_Line1[]="NTNU MT Lin-G.Y.";
unsigned char code MSG_Welcome_Line2[]="Rider MCU System";
unsigned char code MSG_Welcome_Line3[]="TEL: 0958889115 ";
unsigned char code MSG_Welcome_Line4[]="mosdeo@gmail.com";
//==== 開機畫面 ========

unsigned char StringForLCD_Line1[16]="";
unsigned char StringForLCD_Line2[16]="";
unsigned char DS1821_j; //for "for loop"

main()
{
	IE=0x83;	// 1000 0011,啟用INT0、TF0(6-4頁)
	IP=0x00;PT0=1;  // TF0最優先(for mSec計算)
  SCON=0x70;
  TCON=0x00,// 0000 0000 設定INT0 採位準觸發(6-4頁)
	//不可用位準觸發,否則整個磁鐵經過的時間都會中斷 <- 目前位準觸發仍有彈跳問題
	TMOD=0x21;				// 0010 0001,T1採mode 2、T0採mode 1
  TH0=TH_M1; TL0=TL_M1;	// 設置T0計數量高8位元、低8位元 
	TH1=0xFD; //Baud rate
  EX1=0; //外部中斷INT1( P3.3)停用
	
	//	DS1821電源設定
	DS1821_Vcc=1;
	DS1821_GND=0;

	// 磁性開關電源設定
	//MagneticSW_Vcc=1;
	MagneticSW_GND=0;
	
	MagneticSW=1;				// 規劃MagneticSW輸入 
	P0_3=1;			// 規劃SW輸入
	
	init_LCM();
	NewType();
	write_inst(0x01);	//清除螢幕
	write_inst(0x80);write_string(MSG_Welcome_Line1);
	write_inst(0xC0);write_string(MSG_Welcome_Line2);
	delay_ms(2000);
	
	write_inst(0x01);	//清除螢幕
	write_inst(0x80);write_string(MSG_Welcome_Line3);
	write_inst(0xC0);write_string(MSG_Welcome_Line4);
	delay_ms(2000);
	
	TR1=1;TR0=1;					// 啟動T1,T0等待MagSW第一次觸發才啟動(7-7頁)


/*--------------------------------------------
/*sbit EX0  = 0xA8;       /* 1=Enable External interrupt 0 */
/*sbit ET0  = 0xA9;       /* 1=Enable Timer 0 interrupt */
/*sbit EX1  = 0xAA;       /* 1=Enable External interrupt 1 */
/*sbit ET1  = 0xAB;       /* 1=Enable Timer 1 interrupt */
/*sbit ES   = 0xAC;       /* 1=Enable Serial port interrupt */
/*sbit ET2  = 0xAD;       /* 1=Enable Timer 2 interrupt */

/*sbit EA   = 0xAF;       /* 0=Disable all interrupts */

//IE=0;
//EA=1;
//ET0=1;
//EX0=1;


	while(1)
	{	 
		// Visable WDT
		P1_5=~P1_5;
		P1_6=~P1_5;
		
		DS1821_j++;
		if(127==(DS1821_j&127)) //每128次輪迴(16秒)重新初始化螢幕，對抗螢幕線路脫落問題
		{	
			init_LCM();
			NewType();
		}
		
		if(7==(DS1821_j&7))	//	8次輪迴(1秒)才檢測一次溫度
		{	P1_7=0; //	Visable DS1821 action start
			reset_DS();
			write_1byte(0xee);
			TEMP=read_temp();
			display(TEMP);
			if(1==work)TI=1; // PC有下指令才做發射串列中斷
			P1_7=1; //	Visable DS1821 action stop
		}
		
		
		//=== 複雜的計算要在清除螢幕前完成，不然花太多時間put char到LCD，螢幕容易閃動
		unDispSpeed=(unsigned int)(6883/unDisp_mSec); //計算速度
		unDispMileage=(unsigned int)(2.075*unCycleCounter); //計算累計里程
		// 產生準備顯示在第一二行的字串
		sprintf(StringForLCD_Line1,"Speed:%2dkm/h",unDispSpeed);
		sprintf(StringForLCD_Line2,"Total:%5dm%c%c%c",unDispMileage,TABLE[NUMBER[0]],TABLE[NUMBER[1]],TABLE[NUMBER[2]]);
		
		write_inst(0x01);	//清除螢幕
			write_inst(0x80);write_string(StringForLCD_Line1); //第一行顯示
			write_inst(0xC0);write_string(StringForLCD_Line2); //第二行顯示
			write_inst(0xCF);write_char(0x00); //顯示度C
		
			// 不太重要的資訊最後顯示
			write_inst(0x8D);write_char(0x01); //顯示"林"
 			write_inst(0x8E);write_char(0x02); //顯示"高"
 			write_inst(0x8F);write_char(0x03); //顯示"遠"
			delay1ms(125);
			

		
		
	}

}

void serial_INT(void) interrupt 4
{//PC擷取溫度用
    unsigned char n;
      if(TI==1)
	    { TI=0;
				for (n=0;n<3;n++)
				{	SBUF=TABLE[NUMBER[n]];
					while(TI==0);
					TI=0;
				}
      }
			 
      if(RI==1)
			{		RI=0;
					switch(SBUF)
					 {
						 case 0x41:
						 work=1;
						 //LED=0;
								 break;
						 case 0x42:
						 work=0;
						 //LED=1;
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
{	P1_3=0; //Visable extern interrput
	unCycleCounter++; //每次中斷就加一圈
	unDisp_mSec=mSecCounter; //存下數值
	mSecCounter=0;		// 歸零
	while(MagneticSW==0);				// 等待放開PB7
	debouncer();				// 防彈跳
	P1_3=1;	//Visable extern interrput
}								// int 0中斷副程式結束  
//===防彈跳函數=====================================
void debouncer(void)			// 防彈跳函數開始 
{	int i;						// 宣告變數i
	for(i=0;i<2400;i++); 		// 連數2400次，約20ms
}								// 防彈跳函數結束 

// 40km/hr => 0.1867秒/圈＠26"單車
// 除非時速達到 (0.1867/0.02)*40=373.4km/hr
// 否則僅20ms的防彈跳延遲函數不影響計速