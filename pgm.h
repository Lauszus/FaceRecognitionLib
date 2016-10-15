// Source: https://sun.iwu.edu/~shelley/sie/zoo/journal/pgm.h.html

#ifndef PGM_H
#define PGM_H

/*max size of an image*/
#define MAX 800

#ifdef __cplusplus
extern "C" {
#endif

/*
#define LOW_VALUE 0
#define HIGH_VALUE 255
*/

/*RGB color struct with integral types*/
typedef struct { unsigned char red;
                 unsigned char green;
                 unsigned char blue;
                } RGB_INT;

struct PGMstructure {
    int maxVal;
    int width;
    int height;
    RGB_INT data[MAX][MAX];
};

typedef struct PGMstructure PGMImage;

/***prototypes**********************************************************/
/***********************************************************************/

void getPGMfile(const char *filename, PGMImage *img);
void savePGMfile(const char *filename, PGMImage *img);

#ifdef __cplusplus
}
#endif

#endif
