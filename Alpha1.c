#include "mbed.h"
#define CHUNK_SIZE 200

Serial pc(USBTX, USBRX);
AnalogIn acc_z(p15);
AnalogIn acc_x(p16);
AnalogIn acc_y(p17);
AnalogIn gyro_x(p18);
AnalogIn gyro_y(p19);
AnalogIn flexi(p20);

Timer timer;
Ticker data_interrupt;
Ticker hold_interrupt;

int for_count = 0;

int line_count = 0;
int file_count = 0;
int i = 0;
int state = 0; // 0 is rest, 1 is walk, 2 is falling
int t = timer.read_us();
float ax = acc_x.read();
float ay = acc_y.read();
float az = acc_z.read();
float gx = gyro_x.read();
float gy = gyro_y.read();

DigitalOut buzzer(p5);
DigitalIn button(p6);


char temp_num = '0';
FILE *fp;
LocalFileSystem local("local");
char filename[17] = "/local/data0.csv";

void store_data(){

    fprintf(fp, "%d,%f,%f,%f,%f,%f\r\n",t,ax,ay,az,gx,gy);
    pc.printf("l%d,f%d,%d,%f,%f,%f,%f,%f\r\n",line_count,file_count,t,ax,ay,az,gx,gy); 
    
    if (line_count == (CHUNK_SIZE - 1)) {
        pc.printf("MOVING TO NEW FILE------------------------------------------------------------------------------------------------------------------------------------------------------------");
        fclose(fp);
        temp_num = '0' + file_count;
        filename[11] = temp_num;
        
        fp = fopen(filename, "w");

        
        if (file_count == 7) {
            file_count = 0;
        }
        else {
            file_count++;
        }
        line_count = 0;
    }
    else {
        line_count++;
    }  
}

void read_data(){
    t = timer.read_us();
    ax = acc_x.read();
    ay = acc_y.read();
    az = acc_z.read();
    gx = gyro_x.read();
    gy = gyro_y.read();  

    store_data();
}

int is_holding = 0;
int hold_count = 0;

void get_hold(){
	//If it is holding, check to transition to fall
	if (is_holding == 0) {	
		if(flexi.read() > .35) {
			hold_count++;	
			if (hold_count == 2) {
				is_holding = 1;
				hold_count = 0;
				//data_interrupt.attach_us(&read_data, 500);
			}
		}
		//DEBUG
		pc.printf("NOT_held, hold_count = %d\r\n", hold_count);
		
	}
	else if (is_holding == 1) {
		if(flexi.read() < .35) {
			hold_count--;	
			if (hold_count == -2) {
				is_holding = 0;
				hold_count = 0;
				//data_interrupt.detach();
			}
		}
		//DEBUG
		pc.printf("HELD, hold_count = %d\r\n", hold_count);
	}
}
    
    
    


int main() {
    fp = fopen(filename, "w");
    hold_interrupt.attach(&get_hold, 1);
    /*  timer.start();
    
    if (state == 0){
    data_interrupt.detach();
    }
    */

    for(for_count = 0; for_count < 200000; for_count++){
        //read_data();
        //pc.printf("after_read");
    }

}
