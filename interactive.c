#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdint.h>
#include "scheduler.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <FIFO/SJF> <job_sizes_comma_separated>\n", argv[0]);
        return 1;
    }

    int use_sjf;
    if (strcmp(argv[1], "FIFO") == 0 || strcmp(argv[1], "fifo") == 0) {
        use_sjf = 0;
    } else if (strcmp(argv[1], "SJF") == 0 || strcmp(argv[1], "sjf") == 0) {
        use_sjf = 1;
    } else {
        fprintf(stderr, "Unknown policy: use FIFO or SJF.\n");
        return 1;
    }

    // One more job than commas gives enough space for the entire list.
    size_t capacity = 1;
    for (const char *p = argv[2]; *p != '\0'; p++) {
        if (*p == ',') {
            capacity++;
        }
    }
    if (capacity > INT_MAX || capacity > SIZE_MAX / sizeof(int)) {
        fprintf(stderr, "Too many jobs.\n");
        return 1;
    }

    int *jobs = malloc(capacity * sizeof(int));
    if (jobs == NULL) {
        fprintf(stderr, "Could not allocate the job list.\n");
        return 1;
    }

    // Parse every entry before running any jobs; strtol detects invalid numbers.
    int count = 0;
    const char *cursor = argv[2];
    for (;;) {
        char *end;
        errno = 0;
        long dimension = strtol(cursor, &end, 10);
        if (end == cursor || errno == ERANGE || dimension <= 0 || dimension > INT_MAX) {
            fprintf(stderr, "Each job size must be a positive integer within the int range.\n");
            free(jobs);
            return 1;
        }

        // The matrix code uses int indices and needs dimension * dimension cells.
        if (dimension > INT_MAX / dimension ||
            (size_t)dimension > SIZE_MAX / sizeof(int) / (size_t)dimension) {
            fprintf(stderr, "Job size is too large for matrix indexing or storage.\n");
            free(jobs);
            return 1;
        }

        while (isspace((unsigned char)*end)) {
            end++;
        }
        if (*end != ',' && *end != '\0') {
            fprintf(stderr, "Job sizes must be integers separated by commas.\n");
            free(jobs);
            return 1;
        }

        jobs[count++] = (int)dimension;
        if (*end == '\0') {
            break;
        }
        cursor = end + 1;
    }

    // Time the entire batch, including matrix generation and scheduling work.
    struct timespec start, finish;
    if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
        perror("Could not read the start time");
        free(jobs);
        return 1;
    }

    float average_response;
    if (use_sjf) {
        average_response = SJF(jobs, count);
    } else {
        average_response = FIFO(jobs, count);
    }

    if (clock_gettime(CLOCK_MONOTONIC, &finish) != 0) {
        perror("Could not read the finish time");
        free(jobs);
        return 1;
    }
    free(jobs);

    if (!isfinite(average_response) || average_response < 0.0f) {
        fprintf(stderr, "The scheduler could not complete the job list.\n");
        return 1;
    }

    double elapsed = (double)(finish.tv_sec - start.tv_sec) +
                     (double)(finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    if (!isfinite(elapsed) || elapsed <= 0.0) {
        fprintf(stderr, "Could not measure a positive elapsed time.\n");
        return 1;
    }

    // The scheduler returns average response time; throughput is jobs per second.
    printf("Average response time: %.6f seconds\n", average_response);
    printf("Throughput: %.6f jobs/second\n", count / elapsed);
    return 0;
}
