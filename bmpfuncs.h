#ifndef __BMPFUNCS__
#define __BMPFUNCS__
#pragma warning (disable : 4996)
typedef unsigned char uchar;

float* readImageDataf(const char *filename, int* width, int* height, int* channels);
uchar* readImageData(const char *filename, int* width, int* height, int* channels);
void storeImage(float *imageOut, const char *filename, int rows, int cols, 
                const char* refFilename);

#endif
