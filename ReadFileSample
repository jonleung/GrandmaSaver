#include "mbed.h"

LocalFileSystem local("local");   //File System
int main() {

    FILE *tmp = fopen("/local/tmp.txt", "w");  // Open "temp.txt" on the local file system for writing

    float max[3],min[3],offset[3], range[3];

    max[0]=250;
    max[1]=230;
    max[2]=210;

    min[0]=-234;
    min[1]=-154;
    min[2]=-238;

    range[0]=max[0]-min[0];
    range[1]=max[1]-min[1];
    range[2]=max[2]-min[2];

    offset[0]=range[0]/2-max[0];
    offset[1]=range[1]/2-max[1];
    offset[2]=range[0]/2-max[2];

    if ( tmp == NULL)
        error("Could not create file");


    fprintf(tmp, "%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n",
            max[0], min[0], offset[0], range[0],max[1], min[1], offset[1], range[1],max[2], min[2], offset[2], range[2] );

    fclose(tmp);

    max[0]=0;
    min[0]=0;
    offset[0]=0;
    range[0]=0;
    max[1]=0;
    min[1]=0;
    offset[1]=0;
    range[1]=0;
    max[2]=0;
    min[2]=0;
    offset[2]=0;
    range[2]=0;

    // update config file
    FILE * fp = fopen("/local/config.txt", "w");  // Open "config.txt" on the local file system for writing
    tmp = fopen("/local/tmp.txt", "r");  // Open "temp.txt" on the local file system for read

    if (tmp == NULL) {
        error("Could not open file");
    }
    
    while (!feof(tmp)) {
        int n = fscanf(tmp, "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",
            &max[0], &min[0], &offset[0], &range[0],&max[1], &min[1], &offset[1], &range[1],&max[2], &min[2], &offset[2], &range[2]);

        if (n != 12) {
            error("Could not read 12 elements");
        }

        fprintf(fp, "%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n",
                max[0], min[0], offset[0], range[0],max[1], min[1], offset[1], range[1],max[2], min[2], offset[2], range[2]);
    }

    fclose(fp);
    fclose(tmp);
}
