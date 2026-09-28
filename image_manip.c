#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "image_manip.h"
#include "ppm_io.h"
#include <stdbool.h>
//more accuracy for pixel calculations
#define PI 3.14159265358979323846

////////////////////////////////////////
// Definitions of the functions       //
// declared in image_manip.h go here! //
////////////////////////////////////////

/* Convert a RGB pixel to a single grayscale intensity;
* uses NTSC standard conversion
*/
unsigned char pixel_to_gray( Pixel p ) {
	return (unsigned char)( 0.3 * p.r + 0.59 * p.g + 0.11 * p.b );
}

/* Invert the colors of the input image and return a new image */
Image invert(const Image in) {
  Image inverted = { NULL , 0 , 0 };

  // Validate the input image
  if (in.data == NULL || in.rows <= 0 || in.cols <= 0) {
    fprintf(stderr, "Error: invert - invalid input image\n");
    return inverted;
  }

  // Create a new image of the same size
  inverted = make_image(in.rows, in.cols);
  if (inverted.data == NULL) {
    fprintf(stderr, "Error: invert - could not allocate memory for inverted image\n");
    return inverted;
  }

  // Invert RGB values for each pixel
  for (int i = 0; i < in.rows; i++) {
        for (int j = 0; j < in.cols; j++) {
            // Get the current pixel from the input image
            Pixel p = in.data[i * in.cols + j];
            // Invert the pixel for r,g,b
            inverted.data[i * in.cols + j].r = 255-p.r;
            inverted.data[i * in.cols + j].g = 255-p.g;
            inverted.data[i * in.cols + j].b = 255-p.b;
        }
    }

  /*for (int i = 0; i < in.rows * in.cols; i++) {
    inverted.data[i].r = 255 - in.data[i].r;
    inverted.data[i].g = 255 - in.data[i].g;
    inverted.data[i].b = 255 - in.data[i].b;
  }*/
  // Return the new inverted image
  return inverted;
}



/* Crop the input image given top-left and bottom-right points */
Image crop(const Image in, Point top_left, Point bot_right) {
  Image cropped = { NULL, 0, 0 };

  // Validate the input image
  if (!in.data || in.rows <= 0 || in.cols <= 0) {
    fprintf(stderr, "Error: crop - invalid input image\n");
    return cropped;
  }

  // Validate the crop points
  if (top_left.x < 0 || top_left.y < 0 || bot_right.x > in.cols || bot_right.y > in.rows) {
    fprintf(stderr, "Error: crop - crop coordinates out of bounds\n");
    return cropped;
  }

  // Ensure the bottom-right point is below and to the right of the top-left point
  if (top_left.x >= bot_right.x || top_left.y >= bot_right.y) {
    fprintf(stderr, "Error: crop - invalid crop region\n");
    return cropped;
  }

  // Calculate the dimensions of the new cropped image
  int new_cols = bot_right.x - top_left.x;
  int new_rows = bot_right.y - top_left.y;

  // Allocate memory for the new cropped image
  cropped = make_image(new_rows, new_cols);
  if (!cropped.data) {
    fprintf(stderr, "Error: crop - could not allocate memory for cropped image\n");
    return cropped;
  }

  // Copy the pixel data from the input image to the new cropped image
  for (int row = 0; row < new_rows; row++) {
    for (int col = 0; col < new_cols; col++) {
      cropped.data[row * new_cols + col] = in.data[(top_left.y + row) * in.cols + (top_left.x + col)];
    }
  }

  // Return the new cropped image
  return cropped;
}

Image zoom_out(const Image in) {
    // Determine dimensions of the zoomed-out image
    int new_rows = in.rows / 2;
    int new_cols = in.cols / 2;

    // Create zoomed-out image with new dimensions
    Image zoom_out = make_image(new_rows, new_cols);

    // Loop through input image, processing 2x2 pixel blocks
    for (int row = 0; row < new_rows; row++) {
        for (int col = 0; col < new_cols; col++) {
            // Get four pixels in the 2x2 block from the input image
            Pixel p1 = in.data[(2 * row) * in.cols + (2 * col)];
            Pixel p2 = in.data[(2 * row) * in.cols + (2 * col + 1)];
            Pixel p3 = in.data[(2 * row + 1) * in.cols + (2 * col)];
            Pixel p4 = in.data[(2 * row + 1) * in.cols + (2 * col + 1)];

            // Average the red, green, and blue 
            Pixel avg_pixel;
            avg_pixel.r = (p1.r + p2.r + p3.r + p4.r) / 4;
            avg_pixel.g = (p1.g + p2.g + p3.g + p4.g) / 4;
            avg_pixel.b = (p1.b + p2.b + p3.b + p4.b) / 4;

            // Assign the averaged pixel to the new image
            zoom_out.data[row * new_cols + col] = avg_pixel;
        }
    }

    // Return zoomed-out image
    return zoom_out;
}

Image binarize(const Image in, const int threshold) {
    // Create a new image of the same size
    Image out = make_image(in.rows, in.cols);

    // Loop through each pixel in the image
    for (int i = 0; i < in.rows; i++) {
        for (int j = 0; j < in.cols; j++) {
            // Get the current pixel from the input image
            Pixel p = in.data[i * in.cols + j];

            // Convert the pixel to grayscale
            int gray_value = pixel_to_gray(p);

            // Determine the binary value based on the threshold
            int binary_value = (gray_value < threshold) ? 0 : 255;

            // Set the binary value for all three channels in the output image
            out.data[i * in.cols + j].r = binary_value;
            out.data[i * in.cols + j].g = binary_value;
            out.data[i * in.cols + j].b = binary_value;
        }
    }

    // Return the binarized image
    return out;
}

// Helper function to check if a pixel (x, y) is within a circle centered at (cx, cy) with radius r
int is_within_circle(int x, int y, int cx, int cy, int radius) {
    return (sq(x - cx) + sq(y - cy)) <= sq(radius);
}

Image pointilism(const Image in, unsigned int seed) {
    // Seed the random number generator with the provided seed
    srand(seed);

    // Calculate the total number of pixels and the number of pixels to modify (3% of total)
    int total_pixels = in.rows * in.cols;
    int num_selected_pixels = (int)(PERCENT_PIXELS * total_pixels);

    // Create a new output image and initialize it with black pixels
    Image out = make_image(in.rows, in.cols);
    if (!out.data) {
        fprintf(stderr, "Error: Could not allocate memory for output image in pointilism.\n");
        return out;
    }

    // Initialize the output image with black pixels (set each pixel to black)
    Pixel black = {0, 0, 0};  // RGB values for black
    for (int i = 0; i < total_pixels; i++) {
        out.data[i] = black;
    }

    // Apply the pointilism effect by selecting random pixels and creating circles of color
    for (int i = 0; i < num_selected_pixels; i++) {
        // Randomly select a pixel (x, y)
        int x = rand() % in.rows;
        int y = rand() % in.cols;

        // Get the color of the selected pixel from the input image
        Pixel center_pixel = in.data[x * in.cols + y];

        // Generate a random radius between 1 and 5
        int radius = MIN_RADIUS + rand() % (MAX_RADIUS - MIN_RADIUS + 1);

        // Apply the center pixel's color to all pixels within the circle defined by the radius
        for (int dx = -radius; dx <= radius; dx++) {
            for (int dy = -radius; dy <= radius; dy++) {
                int new_x = x + dx;
                int new_y = y + dy;

                // Check if the new pixel (new_x, new_y) is within bounds and within the circle
                if (new_x >= 0 && new_x < in.rows && new_y >= 0 && new_y < in.cols) {
                    if (is_within_circle(new_x, new_y, x, y, radius)) {
                        out.data[new_x * in.cols + new_y] = center_pixel;
                    }
                }
            }
        }
    }

    return out;
}

// Function to generate a Gaussian kernel
double** generate_gaussian_matrix(double sigma, int *size_out) {
    // Use ceil to ensure proper kernel size, common choice is ceil(6 * sigma)
    // adjusted kernel size based on this formula found on internet: Kernel size=2×⌈3×σ⌉+1
    int size = (int)ceil(sigma * 10)+1;
    if (size % 2 == 0) size++;  // Ensure it's odd
    *size_out = size;

    int half_size = size / 2;
    double **matrix = (double**) malloc(size * sizeof(double*));
    for (int i = 0; i < size; i++) {
        matrix[i] = (double*) malloc(size * sizeof(double));
    }

    double sum = 0.0;
    double sigma_sq = sq(sigma);
    double normalization_factor = 1.0 / (2.0 * PI * sigma_sq);

    // Fill the matrix with Gaussian values
    for (int x = -half_size; x <= half_size; x++) {
        for (int y = -half_size; y <= half_size; y++) {
            double g = normalization_factor * exp(-(sq(x) + sq(y)) / (2 * sigma_sq));
            matrix[x + half_size][y + half_size] = g;
            //printf("%f",g);
            sum += g;
        }
    }

    // Normalize the matrix (so that the sum of all elements is 1)
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            matrix[x][y] /= sum;
        }
    }

    return matrix;
}

// Function to apply Gaussian blur to one pixel with more precision
Pixel apply_gaussian_blur(const Image in, int x, int y, double **gaussian_matrix, int kernel_size) {
    double weight_sum=0.0;
    int half_size = kernel_size / 2;
    double red = 0.0, green = 0.0, blue = 0.0;

    // Apply the Gaussian matrix to the pixel's neighborhood
    for (int i = -half_size; i <= half_size; i++) {
        for (int j = -half_size; j <= half_size; j++) {
            int new_x = x + i;
            int new_y = y + j;

            // Edge handling: Clamp new_x and new_y to stay within bounds
            if(new_x<0 || new_x >=in.rows || new_y<0 ||  new_y>=in.cols){
                continue;
            }
            //new_x = fmin(fmax(new_x, 0), in.rows - 1);
            //new_y = fmin(fmax(new_y, 0), in.cols - 1);

            Pixel p = in.data[new_x * in.cols + new_y];
            double weight = gaussian_matrix[i + half_size][j + half_size];

            red += (double)p.r * weight;
            green += (double)p.g * weight;
            blue += (double)p.b * weight;
            weight_sum+=weight;
        }
    }
//normalizing the final pixel by the sum making sure not under/over normalizing when kernel is on edge
  red /= weight_sum;
  green /= weight_sum;
  blue /= weight_sum;


    // Use precise rounding and clamping for final pixel values
    Pixel blurred_pixel;
    blurred_pixel.r = (unsigned char) fmin(fmax((red + 0.0001), 0), 255);
    blurred_pixel.g = (unsigned char) fmin(fmax((green + 0.0001), 0), 255);
    blurred_pixel.b = (unsigned char) fmin(fmax((blue + 0.0001), 0), 255);

    return blurred_pixel;
}

// Function to apply Gaussian blur to the entire image
Image blur(const Image in, double sigma) {
    // Generate the Gaussian matrix
    int kernel_size;
    double **gaussian_matrix = generate_gaussian_matrix(sigma, &kernel_size);

    // Create the output image
    Image out = make_image(in.rows, in.cols);
    if (!out.data) {
        fprintf(stderr, "Error: Could not allocate memory for output image in blur.\n");
        return out;
    }

    // Apply the Gaussian blur to each pixel
    for (int x = 0; x < in.rows; x++) {
        for (int y = 0; y < in.cols; y++) {
            out.data[x * in.cols + y] = apply_gaussian_blur(in, x, y, gaussian_matrix, kernel_size);
        }
    }

    // Free the Gaussian matrix
    for (int i = 0; i < kernel_size; i++) {
        free(gaussian_matrix[i]);
    }
    free(gaussian_matrix);

    return out;
}