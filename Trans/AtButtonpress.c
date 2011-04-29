#include "mbed.h"
#include <string.h>



DigitalOut myled1(LED1);
DigitalOut myled2(LED2);
DigitalOut myled3(LED3);
DigitalOut myled4(LED4);
Serial pc(USBTX, USBRX);
Serial gsm(p28,p27);
AnalogIn acc_z(p15);
AnalogIn acc_x(p16);
AnalogIn acc_y(p17);
AnalogIn gyro_x(p18);
AnalogIn gyro_y(p19);
AnalogIn flexi(p20);

DigitalOut buzzer(p5);
DigitalIn button(p12);

Timer timer;
int main() {

timer.start();
buzzer = 1;
   
gsm.baud(9600);

char buf[40];
char buf1[40];
char buf2= 0x1A;

while(1){
       if (button){


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
            buzzer = 0;

       }     
       else
            buzzer = 0;
    //pc.printf("t:%d, ax:%f,ay:%f,az:%f, gx:%f,gy:%f, flx:%f\r\n",timer.read_us(), acc_x.read(), acc_y.read(), acc_z.read(), gyro_x.read(), gyro_y.read(), flexi.read());
    }

}
