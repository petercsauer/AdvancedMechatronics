#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include <stdio.h>
#include <math.h> //generate sine wave
#define PI 3.14159265

// DEVCFG0
#pragma config DEBUG = OFF // disable debugging
#pragma config JTAGEN = OFF // disable jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // disable flash write protect
#pragma config BWP = OFF // disable boot write protect
#pragma config CP = OFF // disable code protect

// DEVCFG1
#pragma config FNOSC = FRCPLL // use internal oscillator with pll
#pragma config FSOSCEN = OFF // disable secondary oscillator
#pragma config IESO = OFF // disable switching clocks
#pragma config POSCMOD = OFF // RC mode
#pragma config OSCIOFNC = OFF // disable clock output
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // disable clock switch and FSCM
#pragma config WDTPS = PS1048576 // use largest wdt
#pragma config WINDIS = OFF // use non-window mode wdt
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations

unsigned short genSine(unsigned int i, unsigned short v);
unsigned short genTriangle(unsigned int i, unsigned short v);
void initSPI();
unsigned char spi_io(unsigned char o);
void writeUART1(const char * string);

char m[100];

int main() {

    __builtin_disable_interrupts(); // disable interrupts while initializing things

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    TRISBbits.TRISB4 = 1; //B4 is input
    TRISAbits.TRISA4 = 0; //A4 is output
    LATAbits.LATA4 = 0; //A4 is low
    
    initSPI();

    U1RXRbits.U1RXR = 0b0001; // U1RX is B6
    RPB7Rbits.RPB7R = 0b0001; // U1TX is B7
    
    // turn on UART3 without an interrupt
    U1MODEbits.BRGH = 0; // set baud to NU32_DESIRED_BAUD
    U1BRG = ((48000000 / 115200) / 16) - 1;

    // 8 bit, no parity bit, and 1 stop bit (8N1 setup)
    U1MODEbits.PDSEL = 0;
    U1MODEbits.STSEL = 0;

    // configure TX & RX pins as output & input pins
    U1STAbits.UTXEN = 1;
    U1STAbits.URXEN = 1;

    // enable the uart
    U1MODEbits.ON = 1;
    
    __builtin_enable_interrupts();
    
    unsigned int i = 0;
    while (1) {
        unsigned short v = 511;
        unsigned char c, c1;
        unsigned short p, p1;
        unsigned short vf, vf1;
        for (i=0; i<100; i=i+1){

            //writeUART1(m); //call on writeUART1 to send PIC a message
            c = 1;
            vf = genSine(i, v);
            p = (c<<15);
            p = p | (0b111<<12);
            p = p | (vf<<2);
            
            c1 = 0;
            vf1 = genTriangle(i, v);
            p1 = (c1<<15);
            p1 = p1 | (0b111<<12);
            p1 = p1 | (vf1<<2);
            
            LATAbits.LATA0 = 0; //bring CS low
            spi_io(p>>8); //write the byte
            spi_io(p);
            LATAbits.LATA0 = 1; //bring CS high */
            
            LATAbits.LATA0 = 0; //bring CS low
            spi_io(p1>>8); //write the byte
            spi_io(p1);
            LATAbits.LATA0 = 1; //bring CS high
            
            _CP0_SET_COUNT(0); //delay here
            while (_CP0_GET_COUNT() < 48000000 / (100)) { // 1kHz
                ;
            }
        }
        
        sprintf(m, "CYCLE DONE\r\n");
        writeUART1(m);
        
    }
}

unsigned short genSine(unsigned int i, unsigned short v){
    double ret, val;
    val = PI/180;
    ret = sin(8*i*val); //2Hz frequency
    
    ret = 512.0*(sin((4*PI*(i/(100/2.0)))))+512.0;

    return(ret);
}
double tri = 0;
unsigned short genTriangle(unsigned int i, unsigned short v){
    double tri;
    if (i < 50){
        tri = abs((1024/25)*i - 1023);
    }
    else {
        tri = abs((1024/25)*(i-50) - 1023);
    }
    
    
    sprintf(m, "tri = %lf\r\n", tri);
    writeUART1(m);
    return(tri);
}

void initSPI(){
    //Pin B14 has to be SCK1
    // Turn off analog pins
    ANSELA = 0; // 1 for analog
    // Make A0 an output pin for CS
    TRISAbits.TRISA0 = 0;
    LATAbits.LATA0 = 1;
    //Make A1 SDO1
    RPA1Rbits.RPA1R = 0b0011;
    //Make B5 SDI1
    SDI1Rbits.SDI1R = 0b0001;
    
    //setup SPI1
    SPI1CON = 0; //turn off the SPI module and reset it
    SPI1BUF; //clear the rx buffer by reading from it
    SPI1BRG = 1000; //1000 for 12 kHz, 1 for 12 MHz; // baud rate to 10 MHz (SPI4BRG = (4000000/(2*desired)-1))
    SPI1STATbits.SPIROV = 0; //clear the overflow bit
    SPI1CONbits.CKE = 1; // data changes when clock goes from hi to lo (bc CKP is 0)
    SPI1CONbits.MSTEN = 1; // master operation
    SPI1CONbits.ON = 1; // turn on SPI
}

// send a byte via SPI and return the response
unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}

// Write a character array using UART3
void writeUART1(const char * string) {
  while (*string != '\0') {
    while (U1STAbits.UTXBF) {
      ; // wait until tx buffer isn't full
    }
    U1TXREG = *string;
    ++string;
  }
}
