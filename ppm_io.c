#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "ppm_io.h"



/* helper function for read_ppm, takes a filehandle
 * and reads a number, but detects and skips comment lines
 */
int read_num( FILE *fp ) {
  assert(fp);

  int ch;
  while((ch = fgetc(fp)) == '#') { // # marks a comment line
    while( ((ch = fgetc(fp)) != '\n') && ch != EOF ) {
      /* discard characters til end of line */
    }
  }
  ungetc(ch, fp); // put back the last thing we found

  int val;
  if (fscanf(fp, "%d", &val) == 1) { // try to get an int
    ch = fgetc(fp);
    while (ch == ' ') {
      ch = fgetc(fp);
      // drop trailing whitespace
    }
    ungetc(ch, fp);
    if ( (ch = fgetc(fp) ) != '\n') ungetc(ch, fp);
    return val; // we got a value, so return it
  } else {
    fprintf(stderr, "Error:ppm_io - failed to read number from file\n");
    return -1;
  }
}

Image read_ppm( FILE *fp ) {
  Image im = { NULL , 0 , 0 };
  
  /* confirm that we received a good file handle */
  if( !fp ){
	fprintf( stderr , "Error:ppm_io - bad file pointer\n" );
	return im;
  }

  int rows=-1 , cols=-1;

  /* read in tag; fail if not P6 */
  char tag[20];
  tag[19] = '\0';
  fscanf( fp , "%19s\n" , tag );
  if( strncmp( tag , "P6" , 20 ) ) {
	fprintf( stderr , "Error:ppm_io - not a PPM (bad tag)\n" );
	return im;
  }


  /* read image dimensions */

  //read in columns
  cols = read_num( fp ); // NOTE: cols, then rows (i.e. X size followed by Y size)
  //read in rows
  rows = read_num( fp );

  //read in colors; fail if not 255
  int colors = read_num( fp );
  if( colors!=255 ){
	fprintf( stderr , "Error:ppm_io - PPM file with colors different from 255\n" );
	return im;
  }

  //confirm that dimensions are positive
  if( cols<=0 || rows<=0 ){
	fprintf( stderr , "Error:ppm_io - PPM file with non-positive dimensions\n" );
	return im;
  }

  /* Allocate the new image */
  im = make_image( rows , cols );
  if( !im.data ){
	fprintf( stderr , "Error:ppm_io - Could not allocate new image\n" );
	return im;
  }
  /* finally, read in Pixels */

  /* read in the binary Pixel data */
  if( fread( im.data , sizeof(Pixel) , im.rows * im.cols , fp ) != (size_t)(im.rows * im.cols) ) {
    fprintf(stderr, "Error:ppm_io - failed to read data from file!\n");
	free_image( &im );
    return im;
  }

  //return the image struct pointer
  return im;
}

  /* Write given image to disk as a PPM; assumes fp is not null */
int write_ppm( FILE *fp , const Image im ) {
    // Check if the file pointer and image data are valid
  if( !fp || !im.data ) {
    fprintf( stderr , "Error:ppm_io - bad file pointer or image data\n" );
    return 0;
  }

  // Write the header ("P6" format)
  fprintf(fp, "P6\n");

  // Write the image dimensions: width (cols) x height (rows)
  fprintf(fp, "%d %d\n", im.cols, im.rows);

  // Write the maximum color value (255)
  fprintf(fp, "255\n");

  // Write the binary pixel data
  size_t num_pixels = im.rows * im.cols;
  if (fwrite(im.data, sizeof(Pixel), num_pixels, fp) != num_pixels) {
    fprintf(stderr, "Error:ppm_io - failed to write data to file!\n");
    return 0;
  }

  // Return 1
  return 1;
}

/* allocate a new image of the specified size;
 * doesn't initialize pixel values */
Image make_image( int rows , int cols ) {
  Image im = { NULL , 0 , 0 };

  // Check if rows and cols are positive
  if( rows <= 0 || cols <= 0 ) {
    fprintf( stderr , "Error: make_image - non-positive dimensions\n" );
    return im;
  }

  // Allocate memory for the image's pixel data
  im.data = (Pixel *)malloc( rows * cols * sizeof(Pixel) );
  if( !im.data ) {
    fprintf( stderr , "Error: make_image - memory allocation failed\n" );
    return im;
  }

  // Set the image dimensions
  im.rows = rows;
  im.cols = cols;

  // Return the image structure with allocated memory
  return im;
}

/* output dimensions of the image to stdout */
void output_dims( const Image im ) {
  printf( "cols = %d, rows = %d" , im.cols , im.rows );
}

/* free_image
 * utility function to free inner and outer pointers, 
 * and set to null 
 */
void free_image( Image *im ) {
  // Check if the image pointer is valid
  if (im == NULL || im->data == NULL) {
    return;
  }

  // Free the dynamically allocated pixel data
  free(im->data);

  // Set the pixel data pointer to NULL to avoid dangling pointers
  im->data = NULL;

  // Set image dimensions to 0
  im->rows = 0;
  im->cols = 0;
}