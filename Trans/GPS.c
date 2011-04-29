#include "mbed.h"
#include <string.h>

Serial gsm(p28,p27);
Serial pc(USBTX,USBRX);

int main() {

    gsm.baud(9600);
    pc.baud(9600);

    char buf[40];
    char buf1[40];
    char buf2= 0x1A;

    gsm.printf("AT\r\n");
    gsm.scanf("%s",buf);
    pc.printf("%s\n",buf);
    gsm.scanf("%s",buf1);
    pc.printf("%s\n",buf1);

    gsm.printf("AT+CMGF=1\r\n");
    gsm.scanf("%s",buf);
    gsm.scanf("%s",buf1);
    pc.printf("%s\n",buf);
    pc.printf("%s\n",buf1);
    
    
    gsm.printf("AT+CMGS=\"+14842380812\"\r\n");
    gsm.scanf("%s",buf);
    gsm.scanf("%s",buf1);
    pc.printf("%s\n",buf);
    pc.printf("%s\n",buf1);
    
    gsm.printf("Hellow World Finally %c\r\n",buf2);
    gsm.scanf("%s",buf);
    gsm.scanf("%s",buf1);
    pc.printf("%s\n",buf);
    pc.printf("%s\n",buf1);
    
    pc.printf("message sent");
    return 0;
}
