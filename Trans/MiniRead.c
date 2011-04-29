#include "mbed.h"

LocalFileSystem local("local");
Serial pc(USBTX, USBRX);
DigitalOut buzzer(p5);

int main() {
	buzzer = 1;

	FILE * fp = fopen("/local/DATA0.csv", "r");
	if (fp == NULL) {
		pc.printf("ERROR, could not open file");
		error("Could not open file");
	}
	
	int t = 0;
	float ax = 0;
	float ay = 0;
	float az = 0;
	float gx = 0;
	float gy = 0;
	
	while (!feof(fp)) {
		int tm = fscanf(fp, "%d,%f,%f,%f,%f,%f\r\n", &t,&ax,&ay,&az,&gx,&gy);
		
		if (tm != 6) {
			pc.printf("Could not read 12 elements");
			error("Could not read 12 elements");
		}
		
		pc.printf("%d,%f,%f,%f,%f,%f\r\n", t,ax,ay,az,gx,gy);
	}
	fclose(fp);
	buzzer = 0;
}
