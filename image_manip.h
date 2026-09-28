#ifndef IMAGE_MANIP_H
#define IMAGE_MANIP_H

#include "ppm_io.h"

// store PI as a constant
#define PI 3.14159265358979323846

// macro to square a number
#define sq(X) ((X) * (X))

#define PERCENT_PIXELS 0.03  // 3% of the pixels
#define MIN_RADIUS 1         // Minimum circle radius
#define MAX_RADIUS 5         // Maximum circle radius

///////////////////////////////
// Provided helper functions //
///////////////////////////////

/* Convert a RGB pixel to a single grayscale intensity;
* uses NTSC standard conversion
*/
unsigned char pixel_to_gray( Pixel p );

//////////////////////////////////
// Image manipulation functions //
//////////////////////////////////

//______invert______
/* invert the intensity of each channel 
*/
Image invert( const Image in );

//______crop______
/* takes an image and points defining a bounding box,
* and crops that box out of the image, returning a newly
* created image containing just the cropped region
*/
Image crop( const Image in , Point top_left , Point bot_right );

//______zoom_out______
/* "zoom out" an image, by taking a 2x2 square of pixels and averaging
* each of the three color channels to make a single pixel. If an odd
* number of rows in original image, we lose info about the bottom row.
* If an odd number of columns in original image, we lose info about the
* rightmost column.
*/
Image zoom_out( const Image in );

//_______pointilism________
/* apply a painting like effect, i.e. pointilism technique.
*/
Image pointilism( const Image in , unsigned int seed );

// helper for pointilism
int is_within_circle(int x, int y, int cx, int cy, int radius);

//______blur______
/* apply a blurring filter to the image
*/
Image blur( const Image in , double sigma );

// helper blur functions

double** generate_gaussian_matrix(double sigma, int *size_out);
Pixel apply_gaussian_blur(const Image in, int x, int y, double **gaussian_matrix, int kernel_size);

//______binarize______
/* convert to a black and white image based on a threshold
*/
Image binarize( const Image in , const int threshold );

#endif
