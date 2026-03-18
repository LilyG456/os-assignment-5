#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "sobel.h"
#include "rtclock.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Sobel kernels
int Kx[3][3] = {
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1}
};

int Ky[3][3] = {
    {-1, -2, -1},
    { 0,  0,  0},
    { 1,  2,  1}
};

// Globals: Image and threading data
unsigned char **input_image;
unsigned char **output_image;
unsigned char threshold = 127;
int width, height;
int num_threads;

/**
 * Main method
 */
int main(int argc, char *argv[]) {

    if (argc < 4) {
        printf("Usage: ...\n");
        return -1;
    }

    // TODO - Handle command line inputs

    char *filename = argv[1];
    num_threads = atoi(argv[2]);
    threshold = atoi(argv[3]);

    if(num_threads < 1) {
        printf("Number of threads must be at least 1.\n");
        return -1;
    }

    // TODO - Read image file into array a 1D array (see assignment write-up)

    unsigned char *data = stbi_load(filename, &width, &height, NULL, 1);

    if (data == NULL) {
        printf("Error loading image\n");
        return -1;
    }

    input_image = malloc(height * sizeof(unsigned char*));

    for(int i = 0; i < height; i++) {
        input_image[i] = &data[i * width]; // set row pointers
    }

    output_image = malloc(height * sizeof(unsigned char*));

    for(int i = 0; i < height; i++) {
        output_image[i] = malloc(sizeof(unsigned char) * width); // allocate memory for output image
    }

    printf("Loaded %s. Height=%d, Width=%d\n", filename, height, width);

    // Start clocking!
    double startTime, endTime;
    startTime = rtclock();

    // TODO - Prepare and create threads

    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

    int thread_rows = height / num_threads;

    for(int i = 0; i < num_threads; i++) {
        thread_data[i].start = i * thread_rows;
        if(i == num_threads - 1) { // to make sure all rows are assigned, give the last thread the rest
            thread_data[i].end = height;
        } else {
            thread_data[i].end = (i + 1) * thread_rows; // assign rows to thread
        }
        pthread_create(&threads[i], NULL, sobel_thread, &thread_data[i]); //create threads
    }

    // TODO - Wait for threads to finish

    for(int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL); // wait for threads to finish
    }
    
    // End clocking!
    endTime = rtclock();
    printf("Time taken (thread count = %d): %.6f sec\n", num_threads, (endTime - startTime));

    // TODO - Save the file!

    unsigned char *output_data = malloc(width * height);

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            output_data[i * width + j] = output_image[i][j]; // convert 2D output image to 1D array 
        }
    }

    char output_filename[256];
    sprintf(output_filename, "%s-sobel.jpg", filename);

    stbi_write_jpg(output_filename, width, height, 1, output_data, 80); // save 

    // TODO - Free allocated memory

    free(output_data);
    for(int i = 0; i < height; i++) {
        free(output_image[i]);
    }
    free(output_image);
    free(input_image);
    stbi_image_free(data);

    return 0;
}
