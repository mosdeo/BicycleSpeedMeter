void TxCRFL(void)
{
	SBUF='\r';
	while(TI==0);
	TI=0;
	
	SBUF='\n';
	while(TI==0);
	TI=0;
}

void P42XOR(void)
{
	P42=~P42;
}

void GetTempByDS1821(void)
{
	// 硂琌浪代放
	reset_DS();
	write_1byte(0xee);
	TEMP=read_temp();
	display(TEMP);
	
}

void putnum(unsigned int inputNum,char SizeOfDigital)
{
	unsigned char ucBuf[5],n;
	
	ucBuf[4]=inputNum/10000;
	ucBuf[3]=(inputNum%10000)/1000;
	ucBuf[2]=(inputNum%1000)/100;
	ucBuf[1]=(inputNum%100)/10;
	ucBuf[0]=(inputNum%10);
	
	for (n=1;n<=SizeOfDigital;n++)
	{
		SBUF=TABLE[ucBuf[SizeOfDigital-n]];
		while(TI==0);
		TI=0;
	}
}

void putch(char inputChar)
{
	SBUF=inputChar;
	while(TI==0);
	TI=0;
}

void WLDS_puts(char *a)
{
	while(*a!=0)
	{
		putch(*a);
		a++;
	}
}

void TxAll(void)
{//Ω癳虫ó絏┮Τ戈癟
 // $BicycleMCU,<Speed>,<Mile>,<Temp>,<CR><LF>

	WLDS_puts("$BicycleMCU,");
	putnum(unDispSpeed,2);
	putch(',');
	putnum(unDispMileage,5);
	putch(',');
	putnum((int)TEMP,3);
	WLDS_puts(",\r\n");
}

void TxSpeed(void)
{//癳计,秈ボ硉じ
 //ㄒ'5''5'┪'6''6'
	
	//癳计
	SBUF=TABLE[(unDispSpeed%100)/10];
	while(TI==0);
	TI=0;
	
	//癳计
	SBUF=TABLE[unDispSpeed%10];
	while(TI==0);
	TI=0;
	
	SBUF='\n';
	while(TI==0);
	TI=0;
}

void TxTempearture(void)
{
	unsigned char n;
	
	for (n=0;n<3;n++)
	{
		SBUF=TABLE[NUMBER[n]];
		while(TI==0);
		TI=0;
	}
	
	SBUF='\n';
	while(TI==0);
	TI=0;
}