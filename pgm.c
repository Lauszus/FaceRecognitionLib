// Source: https://sun.iwu.edu/~shelley/sie/zoo/journal/pgm.c.html

#include <stdio.h>
#include <stdlib.h>
#include "pgm.h"

/**********************File I/O functions*******************************/
/***********************************************************************/

/*Gets an ascii pgm image file, store as a color pgm.*/
void getPGMfile(const char *filename, PGMImage *img) {
    FILE *in_file;
    char ch;
    int row, col, type;
    int ch_int;

    in_file = fopen(filename, "r");
    if (in_file == NULL) {
        fprintf(stderr, "Error: Unable to open file %s\n\n", filename);
        exit(8);
    }

    //printf("\nReading image file: %s\n", filename);

    /*determine pgm image type (only type three can be used)*/
    ch = getc(in_file);
    if(ch != 'P') {
        printf("ERROR(1): Not valid pgm/ppm file type\n");
        exit(1);
    }
    ch = getc(in_file);
    /*convert the one digit integer currently represented as a character to an integer(48 == '0')*/
    type = ch - 48;
    if((type != 2) && (type != 3) && (type != 5) && (type != 6)) {
        printf("ERROR(2): Not valid pgm/ppm file type\n");
        exit(1);
    }

    while(getc(in_file) != '\n');             /* skip to end of line*/

    while (getc(in_file) == '#') {             /* skip comment lines */
        while (getc(in_file) != '\n');          /* skip to end of comment line */
    }

    /*there seems to be a difference between color and b/w.  This line is needed by b/w but doesn't effect color reading...*/
    fseek(in_file, -1, SEEK_CUR);             /* backup one character*/

    fscanf(in_file,"%d", &((*img).width));
    fscanf(in_file,"%d", &((*img).height));
    fscanf(in_file,"%d", &((*img).maxVal));

    // printf("\n width  = %d",(*img).width);
    // printf("\n height = %d",(*img).height);
    // printf("\n maxVal = %d",(*img).maxVal);
    // printf("\n");

    if (((*img).width  > MAX) || ((*img).height  > MAX)) {
        printf("\n\n***ERROR - image too big for current image structure***\n\n");
        exit(1);
    }

    if(type == 2) { /*uncompressed ascii file (B/W)*/
        for (row=(*img).height-1; row >=0; row--) {
            for (col=0; col< (*img).width; col++) {
                fscanf(in_file,"%d", &ch_int);
                (*img).data[row][col].red = ch_int;
                (*img).data[row][col].green = ch_int;
                (*img).data[row][col].blue = ch_int;
            }
        }
    } else if(type == 3) { /*uncompressed ascii file (color)*/
         for (row=(*img).height-1; row >= 0; row--) {
            for (col=0; col< (*img).width; col++) {
                fscanf(in_file,"%d", &ch_int);
                ((*img).data[row][col].red) = (unsigned char)ch_int;

                fscanf(in_file,"%d", &ch_int);
                ((*img).data[row][col].green) = (unsigned char)ch_int;

                fscanf(in_file,"%d", &ch_int);
                ((*img).data[row][col].blue) = (unsigned char)ch_int;
            }
        }
    } else if(type == 5) { /*compressed file (B/W) - note: this type remains untested at this time...*/
        while(getc(in_file) != '\n'); /*skip to end of line*/
        for (row=(*img).height-1; row >= 0; row--) {
            for (col=0; col < (*img).width; col++) {
                ch = getc(in_file);
                (*img).data[row][col].red = ch;
                (*img).data[row][col].green = ch;
                (*img).data[row][col].blue = ch;
            }
        }
    } else if(type == 6) { /*compressed file (color)*/
        while(getc(in_file) != '\n'); /*skip to end of line*/
        for (row=(*img).height-1; row >=0; row--) {
            for (col=0; col < (*img).width; col++) {
                (*img).data[row][col].red = getc(in_file);
                (*img).data[row][col].green = getc(in_file);
                (*img).data[row][col].blue = getc(in_file);
            }
        }
    }
    fclose(in_file);
    // printf("\nDone reading file.\n");
}

// Modified to save image as P5 type, take filename as argument and read maxVal
void savePGMfile(const char *filename, PGMImage *img) {
    int i, j, nr, nc;//, k;
    int red;//, green, blue;
    FILE *iop;

    nr = img->height;
    nc = img->width;

    iop = fopen(filename, "w");
    //fprintf(iop, "P6\n");
    fprintf(iop, "P5\n");
    fprintf(iop, "%d %d\n", nc, nr);
    fprintf(iop, "%d\n", img->maxVal);

     //k = 1;
     for(i = nr - 1; i  >= 0; i--) {
        for(j = 0; j <  nc; j++) {
           red = img->data[i][j].red;
           //green = img->data[i][j].green;
           //blue = img->data[i][j].blue;
           if(red <  0) {
                printf("IMG_WRITE: Found value %d at row %d col %d\n", red, i, j);
                printf("           Setting red to zero\n");
                red = 0;
            }
            // if(green <  0) {
            //     printf("IMG_WRITE: Found value %d at row %d col %d\n", green,i, j);
            //     printf("           Setting green to zero\n");
            //     green = 0;
            // }
            // if(blue <  0) {
            //     printf("IMG_WRITE: Found value %d at row %d col %d\n", blue, i, j);
            //     printf("           Setting green to zero\n");
            //     blue = 0;
            // }
            if(red  > img->maxVal) {
                printf("IMG_WRITE: Found value %d at row %d col %d\n", red, i, j);
                printf("           Setting red to %d\n", img->maxVal);
                red = img->maxVal;
            }
            // if(green  > 255) {
            //     printf("IMG_WRITE: Found value %d at row %d col %d\n", green,i, j);
            //     printf("           Setting green to 255\n");
            //     green = 255;
            // }
            // if(blue  > 255) {
            //     printf("IMG_WRITE: Found value %d at row %d col %d\n", blue, i, j);
            //     printf("           Setting blue to 255\n");
            //     blue = 255;
            // }

            putc(red, iop);
            /*putc(green, iop);
            putc(blue, iop);*/
        }
    }
    fprintf(iop, "\n");
    fclose(iop);
}
