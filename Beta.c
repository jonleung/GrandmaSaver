#include "mbed.h"
#include <stdio.h>
#include <math.h>


#define DATA_READ_RATE 600 //in microseconds
#define FALL_WAIT_MULTIPLE 30 //x*DATA_READ_RATE = total wait time
#define CHUNK_SIZE 20000
#define MAX_NUM_FILES 7

int state = 0; // 0 is rest, 1 is walk, 2 is falling, 3 is fell
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
DigitalIn button(p12);

Serial sms(p28, p27); // transmit & receive pins, respectively
// default baud rate is 9600bps, which works for the gsm modem

int t = 0;
float ax = 0;
float ay = 0;
float az = 0;
float gx = 0;
float gy = 0;

FILE *fp;
LocalFileSystem local("local");
char chared_file_count = '0';
char filename[17] = "/local/data0.csv";
int line_count = 0;
int file_count = 1;

FILE *ffp;
char fall_file_count = '0';
char fall_filename[] = "/local/FALL0.csv";

int f = 0;
int is_held = 0;
int hold_count = 0;

int fall_count = 0;
int is_horiz = 0;








//*****************************************
// reset_mbed()
//*****************************************
void reset_mbed() {
	if (button) {
    	state = 0; //STATE Rest
        buzzer = 0;
        
        fclose(fp);
        wait(2);
        remove("/local/DATA0.csv");
        remove("/local/DATA1.csv");
        remove("/local/DATA2.csv");
        remove("/local/DATA3.csv");
        remove("/local/DATA4.csv");
        remove("/local/DATA5.csv");
        remove("/local/DATA6.csv");
        remove("/local/DATA7.csv");
        
        exit(0);
	}
}









//*****************************************
// ana_row
//*****************************************
void ana_row(int lt, float lax, float lay, float laz, float lgx, float lgy) {
	pc.printf("%d,%f,%f,%f,%f,%f,%f",lt,lax,lay,laz,lgx,lgy);
}









//*****************************************
// change_file
//*****************************************
void change_file() {
    pc.printf("MOVING TO NEW FILE---------------------------------------------");
    
    //Close Old
    fclose(fp);
    
    //Update Count
    if (file_count == MAX_NUM_FILES) {
        file_count = 0;}
    else {
        file_count++;}
    
    //Open New
    chared_file_count = '0' + file_count;
    filename[11] = chared_file_count;
    fp = fopen(filename, "w");
}










//*****************************************
// read_lines
//*****************************************
void read_lines(int time) {
	pc.printf("Begin Read Lines");
	int local_time = t;
	int time_in_us = time * 1000000;
	int local_file_count = file_count;
	change_file();
	
	int num_lines = (time*1000000) / 2130;
	
	
	int num_files_needed;
    double double_file_count;
    int starting_file_number;
    char file_num_array[MAX_NUM_FILES];
    
    for(int i = 0; i < MAX_NUM_FILES; i++) {
    	file_num_array[i] = 'e';
    }
    	
	double_file_count = ((double)num_lines - (double)line_count) / (double)CHUNK_SIZE;
	num_files_needed = double_file_count + .5;
	
	int array_count = 0;
	int cur_file_num;
	
	for(int i = num_files_needed; i != 0; i--) {
		
		cur_file_num = local_file_count - num_files_needed;
		
		if (cur_file_num < 0) {
			cur_file_num += MAX_NUM_FILES + 1;
		}
			
		if (cur_file_num == file_count){
			printf("WARNING, CALCULATED THAT YOU SHOULD OPEN FILE TO BE WRITTEN");
			exit(0);
		}
		
		file_num_array[array_count] = '0' + cur_file_num;
	}
	
	int rt, rax, ray, raz, rgx, rgy;
	buzzer = 1;
	char name_of_file_to_read[] = "/local/DATA0.CSV";
	for(int i = 0; file_num_array[i] != 'e'; i++) {
		name_of_file_to_read[11] = file_num_array[i];
		FILE *file_to_read = fopen(file_num_array, "r");
		if (file_to_read == NULL) {
			pc.printf("ERROR, could not open file");
			error("Could not open file");
		}
		
		while (!feof(file_to_read)) {
			int tm = fscanf(fp, "%d,%f,%f,%f,%f,%f\r\n", &rt,&rax,&ray,&raz,&rgx,&rgy);
		
			if (tm != 6) {
				pc.printf("Could not read 6 elements");
				error("Could not read 6 elements");
			}
			
			if (local_time - rt <= time_in_us) {
				ana_row(rt,rax,ray,raz,rgx,rgy);
			} 
		}
		
		fclose(file_to_read);
		
	}
	buzzer = 0;
		
	
	
	
}













//*****************************************
// activate_fall_analysis
//*****************************************
void activate_fall_analysis() {
    
    read_lines(10);
    
    wait(4);
    
    //Only activate this if you think that the person fell.
    while(1) {
    	state = 3; //STATE Fell
        pc.printf("I fell!!! FUCK\r\n");
        buzzer = 1;
        reset_mbed();
    }
}










//*****************************************
// analyze_z
//*****************************************

void analyze_z() {

    if (is_horiz == 0) {   
        if(az >= .66) {
        	state = 2; //STATE Falling
            //DEBUG
            pc.printf("falling... fall_count = %d, ax%f,ay%f,az%f gx%f,gy%f\r\n", fall_count, ax,ay,az,gx,gy);    
            
            fall_count++;
            if (fall_count >= FALL_WAIT_MULTIPLE) { // 3 seconds if 
            	state = 3; // STATE fell
                is_horiz = 1;
                fall_count = 0;
                activate_fall_analysis();
            }
        }
        else if (az <.66 && fall_count > 0) {
            state = 1;
            fall_count--;fprintf(fp, "%d,%f,%f,%f,%f,%f\r\n",t,ax,ay,az,gx,gy);
        }
    }
}









//*****************************************
// store_data
//*****************************************

void store_data(){
    
    //DEBUG
    //pc.printf("STORE:l%d,f%d,%d,%f,%f,%f,%f,%f\r\n",line_count,file_count,t,ax,ay,az,gx,gy);
    fprintf(fp, "%d,%f,%f,%f,%f,%f\r\n",t,ax,ay,az,gx,gy);
    
    if (line_count == (CHUNK_SIZE - 1)) { //Reached file limit
        //DEBUG
		change_file();
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

void get_held(){

    f = flexi.read();
	if (state == 0 || state == 1) {// falling
		if (is_held == 0) {  
		    if(f > .35) {
		        hold_count++;    
		        if (hold_count == 2) {
		            is_held = 1;
		            hold_count = 0;
		            state = 1;
		            read_sensors_interrupt.attach_us(&read_sensors, DATA_READ_RATE);
		        }
		    }
		    else if (hold_count > 0) {
		    	hold_count--;
		    }
		    //DEBUG
		    pc.printf("NOT_pressed, hold_count = %d, state = %d\r\n", hold_count, state);
		}
		else if (is_held == 1) { //TODO figure out if it even makes sense to think that you stop walking once you are not holding it
		    if(f <= .35) {
		        hold_count--;    
		        if (hold_count == -8) {
		            is_held = 0;
		            hold_count = 0;
		            state = 0;
		            read_sensors_interrupt.detach();
		        }
		    }
		    else {
		    	hold_count = 0;
		    }
		    //DEBUG
		    pc.printf("PRESSED, hold_count = %d, state = %d\r\n", hold_count);
		}
	}
}










//TODO Remove "delete_this_exit_interrupt"
//*****************************************
// delete_this_exit_interrupt
//*****************************************

Ticker delete_interrupt; //TODO remove

int delete_button_count = 0;
char index;
int index_string;

void delete_this_exit_interrupt() {
    if (button) {
    	pc.printf("%d DELETE DELETE DELETE DELETE DELETE DELETE DELETE DELETE DELETE",delete_button_count);
        delete_button_count++;
        
		
		sms.printf("AT+CMGS=\"+14085059891\"/n");  // SEND message to jonathan
		wait_ms(1000);
		sms.printf("HELLO! I AM MBED! ROARRRRR! ^Z"); // write SMS body text
        
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
    buzzer = 0;
    fp = fopen(filename, "w");
    timer.start();
    
    get_held_interrupt.attach(&get_held, 1);
    delete_interrupt.attach(&delete_this_exit_interrupt, 1);
    
    pc.printf("Begin While");
    
    while(1) {
        //pc.printf("w");
    }
    
}
