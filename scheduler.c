#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include "scheduler.h"


float SJF(int* jobs, int size) {
    if (jobs == NULL || size <= 0) {
        return -1.0f;
    }

    int *sorted_jobs = malloc((size_t)size * sizeof(int));
    if (sorted_jobs == NULL) {
        return -1.0f;
    }
    memcpy(sorted_jobs, jobs, (size_t)size * sizeof(int));

    for (int i = 0; i < size - 1; i++) {
        int smallest = i;
        for (int j = i + 1; j < size; j++) {
            if (sorted_jobs[j] < sorted_jobs[smallest]) {
                smallest = j;
            }
        }

        int temp = sorted_jobs[i];
        sorted_jobs[i] = sorted_jobs[smallest];
        sorted_jobs[smallest] = temp;
    }

    float average_response = FIFO(sorted_jobs, size);
    free(sorted_jobs);
    return average_response;
}

float FIFO(int* jobs, int size) {
    if (size <= 0) {
        return -1.0f;
    }

    float elapsed = 0.0f;
    float total_response = 0.0f;

    for (int i = 0; i < size; i++) {
        float job_time = do_job(jobs[i], jobs[i], jobs[i], 0);

        elapsed += job_time;
        total_response += elapsed;
    }

    return total_response / size;
}
