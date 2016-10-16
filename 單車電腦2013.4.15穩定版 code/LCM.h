//	LCM �禡�w
//	�@��:�L����
//	���:2013/1/22
//	���禡�w�ק��<<�һ�89S51-C�y�� �i�q�M>>�Ҵ��Ѫ��{���X
//	�å[�H��}
//	�ϥΫe�ݭn #include<string.h>,#include<intrins.h>
#include<intrins.h>
#include <string.H>
//====�ˬd���L���================================
inline;void check_BF(void)
{	LCD_Enable=0;					// �T��Ū�g�ʧ@ 
	do						// do-while�j��}�l 
	{ 	BF=1;				// �]�wBF����J 
  	RS = 0; RW = 1;LCD_Enable = 1;	// Ū��BF��AC
	}while(BF == 1);		// ���L�~�� 
}							//	check_BF()��Ƶ��� 
//==== ������ ================================
void delay1ms(int x)
{	int i,j;				// �ŧi�ܼ� 
	for (i=1;i<x;i++)		// ����x��,����X*1ms
		for (j=1;j<120;j++)
			;// ����120��,����1ms
}							// delay1ms()��Ƶ���
//==== �g�J���O��� ================================
void write_inst(char inst)
{	check_BF();				// �ˬd�O�_���L  
	RS = 0; RW = 0; LCD_Enable = 1;	// 
	LCDP = inst;			// �g�J���O��LCM
	_nop_(); //����1us
	LCD_Enable=0;
	check_BF();				// �ˬd�O�_���L 
}							// write_inst()��Ƶ��� 
//==== �g�J�r����ƨ�� ============================
void write_char(char chardata)
{	check_BF();				// �ˬd�O�_���L 
	LCDP = chardata;		// LCMŪ�J�r�� 
	RS = 1; RW = 0 ;LCD_Enable = 1;	// �g�J��Ʀ�LCM
	_nop_(); //����1us
	check_BF();				// �ˬd�O�_���L 
}							// write_char()��Ƶ���
//==== �g�J�r���ƨ�� ===( by �L���� )============
void write_string(char *ptrStr)
{	unsigned char i;
	for(i=0;i<strlen(ptrStr);i++)
		write_char(ptrStr[i]);
}							// write_string()��Ƶ���
//====��l�]�w���(8�줸�ǿ�Ҧ�)===================
void init_LCM(void)
{	write_inst(0x30);	// �]�w�\��-8�줸-�򥻫��O 
	write_inst(0x30);	// �]�w�\��-8�줸-�򥻫��O 
	write_inst(0x38);	// �^��LCM�ۮe�]�w�A����LCM�i���� 
	write_inst(0x38);	// �^��LCM�]�w��C�A����LCM�i���� 
	write_inst(0x08);	// ��ܥ\��-����ܹ�-�L���-��Ф��{ 
	write_inst(0x01);	// �M����ܹ�(��0x20,I/D=1)
	write_inst(0x0e);	// ��J�Ҧ�-��}���W-����ܹ� 
	write_inst(0x06);	// ��ܥ\��-�}��ܹ�-�L���-��Ф��{ 
}						// init_LCM()��Ƶ��� 
//====�g�J�۫ئr��===================
void NewType(void)
{	char i;
	char code cDegreesC[]={0x1C,0x14,0x1b,0x04,0x04,0x04,0x03,0x00};
	char code cLIN[]= {0x0A,0x1F,0x0A,0x0E,0x0E,0x1B,0x0A,0x00};
	char code cGAO[]= {0x04,0x1F,0x0A,0x0E,0x1F,0x15,0x11,0x00};
	char code cYUAN[]={0x1A,0x07,0x0A,0x1D,0x0F,0x1A,0x17,0x00};	
	
	//	�g�J��C���Ÿ�
	RS = 0; RW = 0 ;
	LCD_Enable = 1;
	LCDP=0x40; //���wGCRAM��}
	LCD_Enable=0; check_BF();
	RS = 1; RW = 0;
	for(i=0;i<8;i++)
		write_char(cDegreesC[i]);

	//	�g�J"�L"
	RS = 0; RW = 0 ;
	LCD_Enable = 1;
	LCDP=0x48; //���wGCRAM��}
	LCD_Enable=0; check_BF();
	RS = 1; RW = 0;
	for(i=0;i<8;i++)
		write_char(cLIN[i]);
	
	//	�g�J"��"
	RS = 0; RW = 0 ;
	LCD_Enable = 1;
	LCDP=0x50; //���wGCRAM��}
	LCD_Enable=0; check_BF();
	RS = 1; RW = 0;
	for(i=0;i<8;i++)
		write_char(cGAO[i]);
	
	//	�g�J"��"
	RS = 0; RW = 0 ;
	LCD_Enable = 1;
	LCDP=0x58; //���wGCRAM��}
	LCD_Enable=0; check_BF();
	RS = 1; RW = 0;
	for(i=0;i<8;i++)
		write_char(cYUAN[i]);
}

