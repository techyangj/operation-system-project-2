
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>   // sleep, usleep
#include <time.h>

#define MAX_PROCESSES 100


// Process struct using .txt file
typedef struct {
    int pid;
    int arrival_time;
    int burst_time;
    int priority;
} Process;

typedef struct {
    int pid;
    int burst_time;
} ProcessThreadArg;

// read  processes.txt
int read_processes(const char *filename, Process processes[], int max_n) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("fopen");
        return -1;
    }

    // skip
    char header[256];
    if (!fgets(header, sizeof(header), fp)) {
        fclose(fp);
        return -1;
    }

    int count = 0;
    while (count < max_n &&
           fscanf(fp, "%d %d %d %d",
                  &processes[count].pid,
                  &processes[count].arrival_time,
                  &processes[count].burst_time,
                  &processes[count].priority) == 4) {
        count++;
    }

    fclose(fp);
    return count;
}

// simulate CPU burst
void *process_run(void *arg) {
    ProcessThreadArg *targ = (ProcessThreadArg *)arg;

    printf("[Process %d] Started. Burst time = %d seconds.\n",
           targ->pid, targ->burst_time);

    sleep(targ->burst_time);

    printf("[Process %d] Finished.\n", targ->pid);

    free(targ);
    return NULL;
}

// part 1 simulation
void run_process_simulation() {
    printf("========== Step 1: Process Threads ==========\n");

    Process processes[MAX_PROCESSES];
    int n = read_processes("processes.txt", processes, MAX_PROCESSES);
    if (n <= 0) {
        fprintf(stderr, "Failed to read processes.txt\n");
        return;
    }

    printf("Read %d processes from file.\n", n);

    pthread_t threads[MAX_PROCESSES];

    for (int i = 0; i < n; i++) {
        ProcessThreadArg *arg = malloc(sizeof(ProcessThreadArg));
        if (!arg) {
            perror("malloc");
            exit(1);
        }
        arg->pid = processes[i].pid;
        arg->burst_time = processes[i].burst_time;

        if (pthread_create(&threads[i], NULL, process_run, arg) != 0) {
            perror("pthread_create");
            exit(1);
        }
    }

    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("All process threads finished.\n\n");
}

// Part 2.1: Dining Philosophers Question

#define NUM_PHILOSOPHERS 5
#define MEALS_PER_PHILOSOPHER 3

pthread_mutex_t forks_mutex[NUM_PHILOSOPHERS];

typedef struct {
    int id;   // number：1..5
} PhilosopherArg;

void *philosopher_run(void *arg) {
    PhilosopherArg *parg = (PhilosopherArg *)arg;
    int id = parg->id;

    // index（0..4）
    int left  = id - 1;
    int right = id % NUM_PHILOSOPHERS;

    // Id  always pick up small fork then larger fork.
    int first  = (left < right) ? left : right;
    int second = (left < right) ? right : left;

    printf("[Philosopher %d] Started.\n", id);

    for (int m = 0; m < MEALS_PER_PHILOSOPHER; m++) {
        printf("[Philosopher %d] Thinking...\n", id);
        usleep(100000 + (rand() % 200000));   // simulate thought

        printf("[Philosopher %d] Waiting for forks...\n", id);

        // wait forks
        pthread_mutex_lock(&forks_mutex[first]);
        pthread_mutex_lock(&forks_mutex[second]);

        printf("[Philosopher %d] Picked up fork %d and %d\n",
               id, first + 1, second + 1);

        printf("[Philosopher %d] Eating...\n", id);
        usleep(150000 + (rand() % 200000));   // simulate eat 

        pthread_mutex_unlock(&forks_mutex[second]);
        pthread_mutex_unlock(&forks_mutex[first]);

        printf("[Philosopher %d] Released forks\n", id);
    }

    printf("[Philosopher %d] Finished.\n", id);

    free(parg);
    return NULL;
}

void run_dining_philosophers() {
    printf("========== Step 2.1: Dining Philosophers ==========\n");

    // init 
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_init(&forks_mutex[i], NULL);
    }

    pthread_t philosophers[NUM_PHILOSOPHERS];

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        PhilosopherArg *arg = malloc(sizeof(PhilosopherArg));
        if (!arg) {
            perror("malloc");
            exit(1);
        }
        arg->id = i + 1;   

        if (pthread_create(&philosophers[i], NULL, philosopher_run, arg) != 0) {
            perror("pthread_create philosopher");
            exit(1);
        }
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&forks_mutex[i]);
    }

    printf("All philosophers finished.\n");
}


int main(void) {
    srand((unsigned int)time(NULL));

    // first question anwser
    run_process_simulation();

    // Second question anwser
    run_dining_philosophers();

    return 0;
}
