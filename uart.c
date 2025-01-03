#include <xc.h>
    //setting TX/RX

char mystring[20];
int lenStr = 0, num = 0;

void UART_Initialize() {
           
    /*       TODObasic   
           Serial Setting      
        1.   Setting Baud rate
        2.   choose sync/async mode 
        3.   enable Serial port (configures RX/DT and TX/CK pins as serial port pins)
        3.5  enable Tx, Rx Interrupt(optional)
        4.   Enable Tx & RX
    */
    
    TRISCbits.TRISC6 = 1;            // always 1
    TRISCbits.TRISC7 = 1;            // always 1
    
    //  Setting baud rate = 57600(for AS608)
    TXSTAbits.SYNC = 0;     // use async. mode
    BAUDCONbits.BRG16 = 0;          
    TXSTAbits.BRGH = 0;
    SPBRG = 0;             // table lookup
    
   //   Serial enable
    RCSTAbits.SPEN = 1;     // enable asynchronous serial port, set and dont change     
    PIR1bits.TXIF = 1;       // set when TXREG is empty(flag bit)
    PIR1bits.RCIF = 0;      // will set when reception is complete
    TXSTAbits.TXEN = 1;     // enable transmission, set and dont change      
    RCSTAbits.CREN = 1;      // will be cleared when error occurred, set and dont change       
    PIE1bits.TXIE = 0;       // if interrupt is desired, set TXIE (transmitter)
    IPR1bits.TXIP = 0;       // interrupt is high priority or low priority      
    PIE1bits.RCIE = 1;       // if interrupt is desired, set RCIE (reciever)     
    IPR1bits.RCIP = 0;       // interrupt is high priority or low priority  
            
    }

void UART_Write(unsigned char data)  // Output on Terminal
{
    while(!TXSTAbits.TRMT);     // TRMT is 0 when there's still something inside TSR register
    TXREG = data;              //write to TXREG will send data 
}


void UART_Write_Text(char* text) { // Output on Terminal, limit:10 chars
    for(int i=0;text[i]!='\0';i++)
        UART_Write(text[i]);
}

void ClearBuffer(){
    for(int i = 0; i < 10 ; i++)
        mystring[i] = '\0';
    lenStr = 0;
}

void MyusartRead()
{
    /* TODObasic: try to use UART_Write to finish this function */
    mystring[lenStr] = RCREG;       // the data which has finished processing(can be used)
    
    if(mystring[(lenStr-1)%10] >= '0' && mystring[(lenStr-1)%10] <= '9'  && mystring[lenStr] >= '0' && mystring[lenStr] <= '9'){
        num = (mystring[lenStr] - '0') + (mystring[(lenStr-1)%10] - '0')*10;
    }
    
    if(mystring[lenStr] == '\r'){
        UART_Write('\n');
    }
    UART_Write(mystring[lenStr]);
    
    
    lenStr++;
    lenStr%=10;
    
    return ;
}

char *GetString(){
    return mystring;
}

//void detect_Mode(){
//    if(mystring[(lenStr-5)%10] == 'm' && mystring[(lenStr-1)%10] == '1'){
//        state = 1;
//        num = mystring[lenStr] - '0';
//    }
//    else if(mystring[(lenStr-5)%10] == 'm' && mystring[(lenStr-1)%10] == '2'){
//        state = 2;
//        num = mystring[lenStr] - '0';
//    }
//}


// void interrupt low_priority Lo_ISR(void)
void __interrupt(low_priority)  Lo_ISR(void)
{
    if(RCIF)
    {
        if(RCSTAbits.OERR)
        {
            CREN = 0;
            Nop();
            CREN = 1;
        }
        
        MyusartRead();
    }
    
   // process other interrupt sources here, if required
    return;
}