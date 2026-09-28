//project.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ppm_io.h"
#include "image_manip.h"

// Return (exit) codes
#define RC_SUCCESS            0
#define RC_MISSING_FILENAME   1
#define RC_OPEN_FAILED        2
#define RC_INVALID_PPM        3
#define RC_INVALID_OPERATION  4
#define RC_INVALID_OP_ARGS    5
#define RC_OP_ARGS_RANGE_ERR  6
#define RC_WRITE_FAILED       7
#define RC_UNSPECIFIED_ERR    8

void print_usage(void);

 int main(int argc, char* argv[]) {
    // make sure the right number of arguments is provided
    if (argc < 4) {
        fprintf(stderr, "Incorrect number of arguments for the specified operation\n");
        return 1;
    }

    // Read command-line arguments
    const char *input_filename = argv[1];   // First argument: Input image file
    FILE* file = fopen(input_filename, "rb");
    const char *output_filename = argv[2];  // Second argument: Output image file
    const char *operation = argv[3];        // Third argument: Operation to perform

    if(!file) {
        fprintf(stderr, "Input file I/O error from file %s\n", input_filename);
        return 2;
    }

    // Read the input image
    Image in = read_ppm(file);
    if (!in.data) {
        fprintf(stderr, "Error: Could not read input image from file %s\n", input_filename);
        return 3;
    }

    // Declare the output image
    Image out;

    // Check the operation type and call the corresponding function
    if (strcmp(operation, "zoom_out") == 0) {
        // Perform zoom out 
        out = zoom_out(in);
    }
    else if (strcmp(operation, "invert") == 0) {
        // Perform invert
        out = invert(in);
    }
    else if (strcmp(operation, "crop") == 0) {
        // Ensure that crop operation has enough arguments
        if (argc != 8) {
            fprintf(stderr, "Error: Crop operation requires 4 additional arguments (top_left_x, top_left_y, bot_right_x, bot_right_y)\n");
            free_image(&in);
            return 5;
        }

        // Read the top-left and bottom-right coordinates from the command line
        Point top_left = {atoi(argv[4]), atoi(argv[5])};
        Point bot_right = {atoi(argv[6]), atoi(argv[7])};

        // Perform crop
        out = crop(in, top_left, bot_right);
    }
      else if (strcmp(operation, "binarize") == 0) {
        // Ensure the binarize operation has exactly one threshold argument
        if (argc != 5) {
            fprintf(stderr, "Error: Binarize operation requires 1 additional argument (threshold)\n");
            free_image(&in);
            return 5;
        }

        // Read the threshold value and validate it
        int threshold = atoi(argv[4]);
        if (threshold < 0 || threshold > 255) {
            fprintf(stderr, "Error: Threshold value must be between 0 and 255\n");
            free_image(&in);
            return 6;
        }

        // Perform binarize 
        out = binarize(in, threshold);
    }

    else if (strcmp(operation, "blur") == 0) {
    if (argc != 5) {
        fprintf(stderr, "Error: Blur operation requires 1 additional parameter (sigma)\n");
        free_image(&in);
        return 5;
    }

    double sigma = atof(argv[4]);  // Convert sigma from string to double
    out = blur(in, sigma);
    }

    else if (strcmp(operation, "pointilism") == 0) {
    // Apply pointilism 
    out = pointilism(in, 1);  // Pass a seed value of 1 
    }

    else {
        fprintf(stderr, "Error: Unknown operation %s\n", operation);
        free_image(&in); // Free input image
        return 4;
    }

    // Open the output file for writing
    FILE *fp = fopen(output_filename, "wb");
    if (!fp) {
        fprintf(stderr, "Error: Could not open output file %s\n", output_filename);
        free_image(&in);
        free_image(&out);
        return 7;
    }

    // Write the processed image to the output file
    if (!write_ppm(fp, out)) {
        fprintf(stderr, "Error: Failed to write output image to file %s\n", output_filename);
        fclose(fp);
        free_image(&in);
        free_image(&out);
        return 7;
    }

    // Close the output file
    fclose(fp);

    // Free allocated memory for the images
    free_image(&in);
    free_image(&out);

    // Return success
    return 0;
}



void print_usage(void) {
  printf("USAGE: ./project <input-image> <output-image> <command-name> <command-args>\n");
  printf("SUPPORTED COMMANDS:\n");
  printf("   invert\n" );
  printf("   zoom-out\n" );
  printf("   binarize <threshold>\n" );
  printf("   pointilism\n" );
  printf("   blur <sigma>\n" );
  printf("   crop <top left (x,y)> <bottom right (x,y)>\n" );
}


