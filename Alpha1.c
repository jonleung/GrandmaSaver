#include "mbed.h"
#include <stdio.h>


#define DATA_READ_RATE 600 //in microseconds
#define FALL_WAIT_MULTIPLE 20 //x*DATA_READ_RATE = total wait time
#define CHUNK_SIZE 20000

int state = 0; // 0 is rest, 1 is walk, 2 is falling
//TODO, do I need this?
//int i = 0;


Serial pc(USBTX, USBRX);
Timer timer;

AnalogIn acc_z(p15);
AnalogIn acc_x(p16);
AnalogIn acc_y(p17);
AnalogIn gyro_x(p18);
AnalogIn gyro_y(p19);
AnalogIn flexi(p20);

DigitalOut buzzer(p5);
DigitalIn button(p6);


int t = 0;
float ax = 0;
float ay = 0;
float az = 0;
float gx = 0;
float gy = 0;











//*****************************************
// activate_fall_analysis
//*****************************************
void activate_fall_analysis() {
    while(1) {
        pc.printf("I fell!!! FUCK\r\n");
        buzzer = 1;
        if (button) {
            buzzer = 0;
            exit(0);
        }
    }
}










//*****************************************
// analyze_z
//*****************************************
int fall_count = 0;
int is_horiz = 0;

void analyze_z() {

    if (is_horiz == 0) {    
        if(az >= .66) {
            //DEBUG
            pc.printf("falling... fall_count = %d\r\n", fall_count);    
            
            fall_count++;
            if (fall_count >= FALL_WAIT_MULTIPLE) { // 3 seconds if 
                is_horiz = 1;
                fall_count = 0;
                activate_fall_analysis();
            }
        }
        else if (az <.66 && fall_count > 0) {
            fall_count--;
        }
    }
}









//*****************************************
// store_data
//*****************************************
FILE *fp;
LocalFileSystem local("local");
char chared_file_count = '0';
char filename[17] = "/local/data0.csv";
int line_count = 0;
int file_count = 0;

void store_data(){
    
    //DEBUG
    //pc.printf("STORE:l%d,f%d,%d,%f,%f,%f,%f,%f\r\n",line_count,file_count,t,ax,ay,az,gx,gy);
    fprintf(fp, "%d,%f,%f,%f,%f,%f\r\n",t,ax,ay,az,gx,gy);
    
    if (line_count == (CHUNK_SIZE - 1)) { //Reached file limit
        //DEBUG
        pc.printf("MOVING TO NEW FILE----------------------------------------------------------");
        
        //Close Old
        fclose(fp);
        
        //Open New
        chared_file_count = '0' + file_count;
        filename[11] = chared_file_count;
        fp = fopen(filename, "w");
        
        //Update Count
        if (file_count == 7) {
            file_count = 0;}
        else {
            file_count++;}
        line_count = 0;
    }
    else {
        line_count++;}  
}









//*****************************************
// read_sensors
//*****************************************

Ticker read_sensors_interrupt;

void read_sensors(){
    t = timer.read_us();
    ax = acc_x.read();
    ay = acc_y.read();
    az = acc_z.read();
    gx = gyro_x.read();
    gy = gyro_y.read();  

    store_data();
    analyze_z();
}










//*****************************************
// get_held
//*****************************************

Ticker get_held_interrupt;

int f = 0;
int is_held = 0;
int hold_count = 0;

void get_held(){

    f = flexi.read();

    if (is_held == 0) {    
        if(flexi.read() > .35) {
            hold_count++;    
            if (hold_count == 2) {
                is_held = 1;
                hold_count = 0;
                read_sensors_interrupt.attach_us(&read_sensors, DATA_READ_RATE);
            }
        }
        //DEBUG
        pc.printf("NOT_held, hold_count = %d\r\n", hold_count);
    }
    else if (is_held == 1) {
        if(flexi.read() < .35) {
            hold_count--;    
            if (hold_count == -2) {
                is_held = 0;
                hold_count = 0;
                read_sensors_interrupt.detach();
            }
        }
        //DEBUG
        pc.printf("HELD, hold_count = %d\r\n", hold_count);
    }
}










//TODO Remove "delete_this_exit_interrupt"
//*****************************************
// delete_this_exit_interrupt
//*****************************************

Ticker delete_interrupt; //TODO remove

int delete_button_count = 0;

void delete_this_exit_interrupt() {
    if (button) {
    	pc.printf("%d DELETE DELETE DELETE DELETE DELETE DELETE DELETE DELETE DELETE",delete_button_count);
        delete_button_count++;
        if (delete_button_count == 3){
            exit(0);
        }
    }
    else {
        delete_button_count = 0;
    }    
}










//*****************************************
// main
//*****************************************
//TODO remove this var  
int for_count = 0;

int main() {
    fp = fopen(filename, "w");
    timer.start();
    
    get_held_interrupt.attach(&get_held, 1);
    delete_interrupt.attach(&delete_this_exit_interrupt, 1);
    
    pc.printf("Begin While");
    
    while(1) {
        //pc.printf("w");
    }
    
}
