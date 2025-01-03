#include<pic18f4520.h>

#pragma config OSC = INTIO67
#pragma config PWRT = OFF
#pragma config WDT = OFF
#pragma config DEBUG = OFF, LVP = OFF

void lcdcmd(unsigned char value);
void lcddata(unsigned char value);
void msdelay(unsigned int itime);

#define ldata PORTD
#define rs PORTEbits.RE0
#define rw PORTEbits.RE1
#define en PORTEbits.RE2

void main(){
    TRISD = 0x00;
    ADCON1 = 0x0F;
    TRISE = 0x00;
    msdelay(50);
    lcdcmd(0x38);
    msdelay(50);
    lcdcmd(0x0E);
    msdelay(15);
    lcdcmd(0x01);
    msdelay(15);
    lcdcmd(0x06);
    msdelay(15);
    lcdcmd(0x80);
    lcddata('T');
    msdelay(50);
    lcddata('E');
    msdelay(50);
    lcddata('S');
    msdelay(50);
    lcddata('T');
}

void lcdcmd(unsigned char value){
    ldata = value;
    rs = 0;
    rw = 0;
    en = 1;
    msdelay(1);
    en = 0;
}

void lcddata(unsigned char value){
    ldata = value;
    rs = 1;
    rw = 0;
    en = 1;
    msdelay(1);
    en = 0;
}

void msdelay(unsigned int itime){
    int i,j;
    for (i = 0; i < itime; i++)
        for(j = 0; j<135; j++);
}
    
