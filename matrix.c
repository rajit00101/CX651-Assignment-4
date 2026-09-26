#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void generate_random_matrix(int rows, int cols, int *matrix) {
    // generate n row x n col matrix w rando vals using rand()
    for (int i = 0; i < rows * cols; i++) {
        matrix[i] = rand();
    }
}

void multiply_matrices(int rows1, int cols1, int *matrix1,
                       int rows2, int cols2, int *matrix2,
                       int *result) {
    // multiply matrix A and B and generate new output matrix
    for (int i = 0; i < rows1; i++) {
        for (int x = 0; x < cols2; x++) {
            int sum = 0;
            for (int y = 0; y < cols1; y++) {
                sum += matrix1[i * cols1 + y] * matrix2[y * cols2 + x];
            }
            result[i * cols2 + x] = sum;
        }
    }
    
}

void display_matrix(int rows, int cols, int *matrix) {
    // print matrix to STDOUT to debug
    for (int i = 0; i < rows * cols; i += cols) {
        for (int x = i; x < i + cols; x++) {
            printf("%d ", matrix[x]);
        }
        printf("\n");
    }
}

float do_job(int rows1, int cols1, int cols2, int forever) {
    // generate matrix of specific size, multiply them
    // one time if forever = 0, otherwise loop forever
    int *matrix1 = malloc(rows1*cols1*sizeof(int));
    int *matrix2 = malloc(cols1*cols2*sizeof(int));
    int *result = malloc(rows1*cols2*sizeof(int));

    generate_random_matrix(rows1, cols1, matrix1);
    generate_random_matrix(cols1, cols2, matrix2);
    
    // timing multiplication
    struct timespec t0, t1;

    timespec_get(&t0, TIME_UTC);  // C11 feature

    // WHAT YOU WANT TO TIME
    do {
        multiply_matrices(rows1, cols1, matrix1, cols1, cols2, matrix2, result);
    } while (forever == 1);

    timespec_get(&t1, TIME_UTC);  // C11 feature

    // nano seconds elapsed converted to fractional seconds
    float dns = (float)(t1.tv_nsec - t0.tv_nsec) / 1000000000;
    // seconds elapsed
    float ds = (float)(t1.tv_sec - t0.tv_sec);

    float total_time = dns+ds;
    
    free(matrix1);
    free(matrix2);
    free(result);
    
    return total_time;

}

