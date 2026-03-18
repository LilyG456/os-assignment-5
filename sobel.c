#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include "sobel.h"

// TODO - write your pthread-ready sobel filter function here for a range of rows

// Note: You have access to all the global variables here.

void process(int start, int end) {
    for (int i = start; i < end; i++) {
        if(i < 1 || i >= height - 1) { // set all pixels to zero of first and last row
            for(int j = 0; j < width; j++) {
                output_image[i][j] = 0; 
            }
            continue;
        }

        for (int j = 1; j < width - 1; j++) {
            int Gx = 0;
            int Gy = 0;

            for(int k = -1; k <= 1; k++) {
                for(int l = -1; l <= 1; l++) {// loop through the 3x3 grid of neighbors of the pixel
                    Gx += input_image[i + k][j + l] * Kx[k + 1][l + 1]; 
                    Gy += input_image[i + k][j + l] * Ky[k + 1][l + 1]; 
                }
            }
            int magnitude = (int) sqrt((Gx * Gx) + (Gy * Gy)); // compute magnitude of gradient
            if(magnitude > 255) { // cap the magnitude at 255
                magnitude = 255;
            }
            if(magnitude < threshold) { // check magnitude with threshold
                output_image[i][j] = 0;
            }else{
                output_image[i][j] = magnitude;
            }
        }
        output_image[i][0] = 0; // set first and last column to zero
        output_image[i][width - 1] = 0;
    }
}

void* sobel_thread(void* arg){
    ThreadData* data = (ThreadData*) arg;
    process(data->start, data->end);// call the process function for a range of rows for the thread
    return NULL;
}