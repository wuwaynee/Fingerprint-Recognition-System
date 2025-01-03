#define _XTAL_FREQ 18432000

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// BEGIN CONFIG
#pragma config OSC = INTIO67      // Oscillator Selection bits (HS oscillator)
#pragma config WDT = OFF      // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT0 = OFF       // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config WRT1 = OFF
#pragma config WRT2 = OFF
#pragma config WRT3 = OFF
#pragma config CP0 = OFF        // Flash Program Memory Code Protection bit (Code protection off)
#pragma config CP1 = OFF
#pragma config CP2 = OFF
#pragma config CP3 = OFF
#pragma config DEBUG = OFF
// END CONFIG

#define uchar unsigned char
#define uint unsigned int

#define SWPORTdir TRISA
#define SWPORT PORTA
#define enrol PORTAbits.RA0
#define match PORTAbits.RA1
#define delet PORTAbits.RA3
#define ok PORTAbits.RA2
#define up PORTAbits.RA5
#define down PORTAbits.RA0
#define LEDdir TRISC3
#define LED PORTCbits.RC3
#define HIGH 1
#define LOW 0
#define PASS 0
#define ERROR 1
#define checkKey(id) id=up<down?++id:down<up?--id:id;

#define ldata PORTD
#define rs PORTEbits.RE0
#define rw PORTEbits.RE1
#define en PORTEbits.RE2

uchar buf[20];
uchar buf1[20];

volatile uint index=0;
volatile int flag=0;
uint msCount=0;
uint g_timerflag=1;
volatile uint count=0;

uchar data[10];
uint id=1;


enum{CMD, DATA, SBIT_CREN=4, SBIT_TXEN, SBIT_SPEN,};


const char passPack[]={0xEF, 0x1, 0xFF, 0xFF, 0xFF, 0xFF, 0x1, 0x0, 0x7, 0x13, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1B};
const char f_detect[]={0xEF, 0x1, 0xFF, 0xFF, 0xFF, 0xFF, 0x1, 0x0, 0x3, 0x1, 0x0, 0x5};
const char f_imz2ch1[]={0xEF, 0x1, 0xFF, 0xFF, 0xFF, 0xFF, 0x1, 0x0, 0x4, 0x2, 0x1, 0x0, 0x8};
const char f_imz2ch2[]={0xEF, 0x1, 0xFF, 0xFF, 0xFF, 0xFF, 0x1, 0x0, 0x4, 0x2, 0x2, 0x0, 0x9};
const char f_createModel[]={0xEF,0x1,0xFF,0xFF,0xFF,0xFF,0x1,0x0,0x3,0x5,0x0,0x9};
char f_storeModel[]={0xEF,0x1,0xFF,0xFF,0xFF,0xFF,0x1,0x0,0x6,0x6,0x1,0x0,0x1,0x0,0xE};
const char f_search[]={0xEF, 0x1, 0xFF, 0xFF, 0xFF, 0xFF, 0x1, 0x0, 0x8, 0x1B, 0x1, 0x0, 0x0, 0x0, 0xA3, 0x0, 0xC8};
char f_delete[]={0xEF,0x1,0xFF,0xFF,0xFF,0xFF,0x1,0x0,0x7,0xC,0x0,0x0,0x0,0x1,0x0,0x15};

void lcdwrite(unsigned char value, int a){
    return;
}

void msdelay(unsigned int itime){
    int i,j;
    for (i = 0; i < itime; i++)
        for(j = 0; j<135; j++);
}

void lcddata(unsigned char value){
    ldata = (value & 0xF0) | (ldata & 0x0F); // Send higher nibbles;
    rs = 1;
    rw = 0;
    en = 1;
    msdelay(1);
    en = 0;
    
    ldata = ((value << 4) & 0xF0) | (ldata & 0x0F); // Send lower nibble
    rs = 1;
    rw = 0;
    en = 1;
    msdelay(1);
    en = 0;
}

void lcdcmd(unsigned char value){
    ldata = (value & 0xF0) | (ldata & 0x0F);// Send higher nibble
    rs = 0;
    rw = 0;
    en = 1;
    msdelay(1);
    en = 0;
    
    ldata = ((value << 4) & 0xF0) | (ldata & 0x0F); // Send lower nibble
    rs = 0;
    rw = 0;
    en = 1;
    msdelay(1);
    en = 0;
}


void lcdprint(char *str){
    while(*str){
        lcddata(*str++);
    }
}


void lcdbegin(){   
    msdelay(50);
    lcdcmd(0x02);
    msdelay(50);
    lcdcmd(0x28);
    msdelay(15);
    lcdcmd(0x0C);
    msdelay(15);
    lcdcmd(0x06);
    msdelay(15);
    lcdcmd(0x01);
}

void lcdinst(){
    lcdcmd(0x80);
    lcdprint("1-Match 2-Enroll");
    lcdcmd(0xc0);
    lcdprint("3-Delete Finger");
    __delay_ms(10);
}


void serialbegin(uint baudrate){
    SPBRG = (18432000UL/(long)(64UL*baudrate))-1;       // baud rate @18.432000Mhz Clock

    TXSTAbits.SYNC = 0;                                 //Setting Asynchronous Mode, ie UART
    RCSTAbits.SPEN = 1;                                 //Enables Serial Port
    TRISCbits.TRISC7 = 1;                                         //As Prescribed in Datasheet
    TRISCbits.TRISC6 = 0;                                         //As Prescribed in Datasheet
    RCSTAbits.CREN = 1;                                 //Enables Continuous Reception
    TXSTAbits.TXEN = 1;                                 //Enables Transmission
    GIE  = 1;                                           // ENABLE interrupts
    INTCONbits.PEIE = 1;                                // ENable peripheral interrupts.
    PIE1bits.RCIE   = 1;                                // ENABLE USART receive interrupt
    PIE1bits.TXIE   = 0;                                // disable USART TX interrupt
    PIR1bits.RCIF = 0;
}


void serialwrite(char ch){
    while(TXIF==0);  // Wait till the transmitter register becomes empty
    TXIF=0;          // Clear transmitter flag
    TXREG=ch;        // load the char to be transmitted into transmit reg
}


void serialprint(char *str){
    while(*str){
        serialwrite(*str++);
    }
}

//void interrupt SerialRxPinInterrupt(void){
void SerialRxPinInterrupt(void){
    if((PIR1bits.RCIF == 1) && (PIE1bits.RCIE == 1)){
        uchar ch=RCREG; 
        buf[index++]=ch;
        if(index>0)
            flag=1;

        RCIF = 0; // clear rx flag
    }  
}


void serialFlush(){
    for(int i=0;i<sizeof(buf);i++){
        buf[i]=0;
    }
}


int sendcmd2fp(char *pack, int len){
    uint res=ERROR;
    serialFlush();
    index=0;
    __delay_ms(100);

    for(int i=0;i<len;i++){
      serialwrite(*(pack+i));
    }
    
    __delay_ms(1000);

    if(flag == 1){
        if(buf[0] == 0xEF && buf[1] == 0x01){
            if(buf[6] == 0x07){   // ack
                if(buf[9] == 0){
                    uint data_len= buf[7];
                    data_len<<=8;
                    data_len|=buf[8];
                    for(int i=0;i<data_len;i++)
                        data[i]=0;

                    for(int i=0;i<data_len-2;i++){
                        data[i]=buf[10+i];
                    }

                    res=PASS;
                }
                else{
                    res=ERROR;
                }
            }
        }

      index=0;
      flag=0;
      return res;
      
    }   
}


uint getId(){
    uint id=0;
    lcdcmd(1);
    while(1){
        lcdcmd(0x80);
        checkKey(id);
        sprintf(buf1,"Enter Id:%d  ",id);
        lcdprint(buf1);
        
        __delay_ms(200);

        if(ok == LOW)
            return id;
    }
}


void matchFinger(){
    lcdcmd(1);
    lcdprint("Place Finger"); 
    lcdcmd(192);

    __delay_ms(2000);

    if(!sendcmd2fp(&f_detect[0],sizeof(f_detect))){
        if(!sendcmd2fp(&f_imz2ch1[0],sizeof(f_imz2ch1))){
            if(!sendcmd2fp(&f_search[0],sizeof(f_search))){
                lcdcmd(1);
                lcdprint("Finger Found");
                uint id= data[0];
                id<<=8;
                id+=data[1];
                uint score=data[2];
                score<<=8;
                score+=data[3];      
                sprintf(buf1,"Id:%d  Score:%d",id,score);
                lcdcmd(192);
                lcdprint(buf1); 

                LED=1;

                __delay_ms(1000);

                LED=0;
            }
            else{
                lcdcmd(1);
                lcdprint("Not Found");
            }
        }
    }
    else{
        lcdprint("No Finger"); 
    }
    __delay_ms(2000);
}


void enrolFinger(){
    lcdcmd(1);
    lcdprint("Enroll Finger");
    __delay_ms(2000);
    lcdcmd(1);
    lcdprint("Place Finger"); 
    lcdcmd(192);
    __delay_ms(1000);
    if(!sendcmd2fp(&f_detect[0],sizeof(f_detect))){
        if(!sendcmd2fp(&f_imz2ch1[0],sizeof(f_imz2ch1))){
            lcdprint("Finger Detected");
            __delay_ms(1000);
            lcdcmd(1);
            lcdprint("Place Finger");
            lcdcmd(192);
            lcdprint("    Again   "); 
            __delay_ms(2000);

            if(!sendcmd2fp(&f_detect[0],sizeof(f_detect))){
                if(!sendcmd2fp(&f_imz2ch2[0],sizeof(f_imz2ch2))){
                    lcdcmd(1);
                    lcdprint("Finger Detected");
                    __delay_ms(1000);
                    if(!sendcmd2fp(&f_createModel[0],sizeof(f_createModel))){
                        id=getId();
                        f_storeModel[11]= (id>>8) & 0xff;
                        f_storeModel[12]= id & 0xff;
                        f_storeModel[14]= 14+id; 
                        if(!sendcmd2fp(&f_storeModel[0],sizeof(f_storeModel))){
                            lcdcmd(1);
                            lcdprint("Finger Stored");
                            sprintf(buf1,"Id:%d",id);
                            lcdcmd(192);
                            lcdprint(buf1);
                            __delay_ms(1000);
                        }
                        else{
                            lcdcmd(1);
                            lcdprint("Finger Not Stored");
                        }
                    }
                    else{
                        lcdprint("Error");
                    }
                }
                else{
                    lcdprint("Error");  
                }
            }
            else{
                lcdprint("No Finger"); 
            }
        } 
    }
    else{
        lcdprint("No Finger"); 
    }

    __delay_ms(2000);
}


void deleteFinger(){
    id=getId();
    
    f_delete[10]=id>>8 & 0xff;
    f_delete[11]=id & 0xff;
    f_delete[14]=(21+id)>>8 & 0xff;
    f_delete[15]=(21+id) & 0xff;

    if(!sendcmd2fp(&f_delete[0],sizeof(f_delete))){
        lcdcmd(1);
        sprintf(buf1,"Finger ID %d ",id);
        lcdprint(buf1);
        lcdcmd(192);
        lcdprint("Deleted Success");
    }
    else{
        lcdcmd(1);
        lcdprint("Error");
    }

    __delay_ms(2000);
}


int main(){            
    void (*FP)();  

    ADCON1=0b00000110;
    LEDdir= 0;
    SWPORTdir=0x0F;
    SWPORT=0xF0;
    serialbegin(57600);

    TRISD = 0x00;
    ADCON1 = 0x0F;
    TRISE = 0x00;
    
    lcdbegin();
    lcdprint("Fingerprint");
    lcdcmd(192);
    lcdprint("Interfacing");

    __delay_ms(200);

    lcdcmd(1);
    lcdprint("Using PIC18F4520");
    lcdcmd(192);
    lcdprint("Circuit Digest");

    __delay_ms(200);

    index=0;    

    while(sendcmd2fp(&passPack[0],sizeof(passPack))){
       lcdcmd(1);
       lcdprint("FP Not Found");
       __delay_ms(200);
       index=0;
    }

    lcdcmd(1);
    lcdprint("FP Found");

    __delay_ms(100);

    lcdinst();

    while(1){ 
        FP=match<enrol?matchFinger:enrol<delet?enrolFinger:delet<enrol?deleteFinger:lcdinst;
        FP();
    } 

    return 0;
}

