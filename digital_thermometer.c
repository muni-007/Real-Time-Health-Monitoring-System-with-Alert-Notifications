#include<reg51.h>
#include<real_clock.h>
sbit rs=P3^2;
sbit en=P3^3;
sbit rd=P3^5;
sbit wr=P3^6;
sbit inv=P3^7;
sfr port=0xA0;
sfr adc=0x90;
sbit buzzer=P3^0;
unsigned int t1=0;
unsigned char t2=0;
unsigned char temp,pulse;


void delay(unsigned int ms){
	unsigned int i;
	for(i=0;i<ms;i++);
}
void lcd_init(unsigned char x){
	port=x;
	rs=0;
	en=1;
	delay(10);
	en=0;
}
void lcd_comd(){
	lcd_init(0x38);
	delay(10);
	lcd_init(0x0C);
	delay(10);
	lcd_init(0x01);
	delay(10);
	lcd_init(0x80);
}
void lcd_data(unsigned char var){
	port=var;
	rs=1;
	en=1;
	delay(10);
	en=0;
}
void lcd_str(unsigned char *str){
	while(*str){
		lcd_data(*str++);
	}
}

void uart_char(unsigned char x){
  SBUF=x;
	while(TI==0);
	TI=0;
}

void send_number(unsigned char num){
  
	if(num>99){
	uart_char((num/100)+'0');
		uart_char(((num%100)/10)+'0');
		uart_char((num%10)+'0');
	}
	else{
	uart_char((num/10)+'0');
	uart_char((num%10)+'0');
	}
}
void uart_str(unsigned char *s){
while(*s){
uart_char(*s++);
}
}

void convert(unsigned char value){
	unsigned char x,d1,d2,d3; //255
x=value/10;  //25
d3=value%10; //5
d2=x%10;    //5
d1=x/10;     //2
lcd_data(d1+0x30);
lcd_data(d2+0x30);
lcd_data(d3+0x30);	
}
void counter() interrupt 1
{
	
TF0=0;
}
void timer1() interrupt 3
{
TF1=0;
	t1++;
}

int main(){
	unsigned char hour,minute,second;
	TMOD=0x26;
	TL0=0x00;
	TH0=0x00;
	TL1=0xFD;
	TH1=0xFD;
	SCON=0x50;
	//PCON|=0X80;
	IE=0x8A;
	
	TR0=1;
	TR1=1;
	buzzer=0;
	
//set_ds1307(0x13,0x05,0x30);
lcd_comd();
	lcd_str(" welcome ");
	lcd_init(0xC0);
	lcd_str(" Muneeswar ");
	delay(65000);
	lcd_init(0x01);
	lcd_init(0x80);
	lcd_str("Temperature: ");
	lcd_init(0x86);
	while(1){
		get_ds1307(&hour,&minute,&second);
		rd=1;
		wr=0;
		delay(200);
		wr=1;
		while(inv==1);
		rd=0;
		inv=1;
		lcd_init(0x8D);
		temp=adc*2;
		convert(temp);
		if(temp>100){
		buzzer=1;
			uart_str("!ALERT");
			uart_char(0x0D);
			uart_str("TEMPERATURE IS HIGH");
			uart_char(0x0D);
				send_number(hour);
				uart_char(':');
				send_number(minute);
				uart_char(':');
				send_number(second);
				uart_str(" Temperature rate: ");
				send_number(temp);
			uart_char(0x0D);
			delay(10000);
		}
		else{
		buzzer=0;
		}
		
		lcd_init(0xC0);
		lcd_str(" pulse: ");
		if(t1>20000){
			t1=0;
		t2++;
		}
		if(t2>157){  //1 min
		t2=0;
			lcd_init(0xC8);
			pulse=TL0*2;
			convert(pulse);
			lcd_init(0xCC);
			lcd_str("BPM");
			if(pulse>100){
		    buzzer=1;
				uart_str("!ALERT");
				uart_char(0x0D);
				uart_str("HeartBeat is HIGH");
				uart_char(0x0D);
				send_number(hour);
				uart_char(':');
				send_number(minute);
				uart_char(':');
				send_number(second);
				uart_str(" Heart rate: ");
				send_number(pulse);
			uart_char(0x0D);
				delay(65000);
		}
		else if(pulse<30){
			buzzer=1;
			uart_str("!ALERT");
				uart_char(0x0D);
				uart_str("HeartBeat is LOW");
			uart_char(0x0D);
				send_number(hour);
				uart_char(':');
				send_number(minute);
				uart_char(':');
				send_number(second);
				uart_str(" Heart rate: ");
				send_number(pulse);
			uart_char(0x0D);
				delay(65000);
		}
		else{
		buzzer=0;
		}
			TL0=0;
		}
	}
}