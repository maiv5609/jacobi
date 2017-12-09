/* Preston Carroll
 * Victor Mai
 * CSCI 347, Assignment 5
 * Multi threaded application of jacobi algorithm on variable sized matrix
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define SIZE 1024
#define NOTH 7
#define DELTA 0.00001

typedef struct arg_st{
    double (* mtxLEFT) [SIZE];
    double (* mtxRIGHT) [SIZE];
    double (* maxArr);
    int n;
    int t;
    int j;
} arg_t;

typedef struct barrier_st{
    int noth;
    pthread_cond_t ready;
    pthread_cond_t full;
    pthread_mutex_t mtx;
    int waiting;
    int leaving;
} barrier;

/* jacobi
 * thread body that applies the jacobi algorithm to our left and right matrix, alternating between the two.
 * will skip the first and last row when calculating the values since the border is already known
 */
void* jacobi(void* ptr);
void readInValues(double (*mtx)[SIZE]);
/* barrier_new
 * initalizes our barrier after being given a number of threads that is defined at the top of our program
 */
barrier* barrier_new (int noth);
/* barrier_enter
 * implements our barrier logic
 */
void barrier_enter(barrier* b);

//single barrier used across all threads
barrier threadBarrier;

int main (int argc, const char* argv[]){
    double (* mtxL) [SIZE] = malloc (SIZE* SIZE* sizeof (double));
    double (* mtxR) [SIZE] = malloc (SIZE* SIZE* sizeof (double));
    double (* maxes) = malloc (SIZE * sizeof(double));

    pthread_t thd[NOTH];
    arg_t args[NOTH];
    int j;

    readInValues(mtxL);
    readInValues(mtxR);

    threadBarrier = *barrier_new(NOTH);

    for(j = 0; j < NOTH; j++){
        args[j].mtxLEFT = mtxL;
        args[j].mtxRIGHT = mtxR;
        args[j].maxArr = maxes;
        args[j].n = SIZE;
        args[j].t = NOTH;
        args[j].j = j;
        pthread_create(&thd[j], NULL, &jacobi, &args[j]);
    }

    void* unused;

    for(j = 0; j < NOTH; j++){
        pthread_join(thd[j], &unused);
    }

    printf("Done\n");
    free(mtxL);
    free(mtxR);
    return 0;
}


void* jacobi(void* ptr){barrier_enter(&threadBarrier);
    arg_t* args = (arg_t*)(ptr);
    int i;
    int j;
    int from = (args->j*args->n)/args->t;
    int to = ((args->j+1)*args->n)/args->t;
    if (from == 0)
        from = 1;
    if (to == args->n)
        to = args->n - 1;
    double highestMax;
    double test;
    int done = 0;

    while (!done){
        highestMax = 0.0;
        for(int i = from; i < to; i++){
            for(int j = 1; j < SIZE-1; j++){
                double prev = args->mtxRIGHT[i][j];
                args->mtxLEFT[i][j] = (args->mtxRIGHT[i-1][j] +
                            args->mtxRIGHT[i+1][j] +
                            args->mtxRIGHT[i][j-1] +
                            args->mtxRIGHT[i][j+1]) / 4.0;
                test = args->mtxLEFT[i][j] - prev;
                if (test > args->maxArr[args->j])
                    args->maxArr[args->j] = test;
            }
        }
        barrier_enter(&threadBarrier);
        for(int i = 0; i < NOTH; i++){
            if (highestMax < args->maxArr[i])
                highestMax = args->maxArr[i];
        }
        if (highestMax < DELTA)
            done = 1;
        barrier_enter(&threadBarrier);


        for(int i = 0; i < NOTH; i++)
            args->maxArr[i] = 0.0;
        highestMax = 0.0;
        for(int i = from; i < to; i++){
            for(int j = 1; j < SIZE-1; j++){
                double prev = args->mtxLEFT[i][j];
                args->mtxRIGHT[i][j] = (args->mtxLEFT[i-1][j] +
                            args->mtxLEFT[i+1][j] +
                            args->mtxLEFT[i][j-1] +
                            args->mtxLEFT[i][j+1]) / 4.0;
                test = args->mtxRIGHT[i][j] - prev;
                if (test > args->maxArr[args->j])
                    args->maxArr[args->j] = test;
            }
        }
        barrier_enter(&threadBarrier);
        for(int i = 0; i < NOTH; i++){
            if (highestMax < args->maxArr[i])
                highestMax = args->maxArr[i];
        }
        if (highestMax < DELTA)
            done = 1;
        barrier_enter(&threadBarrier);

        for(int i = 0; i < NOTH; i++)
            args->maxArr[i] = 0.0;
    }
    barrier_enter(&threadBarrier);
    return NULL;
}

void readInValues(double (*mtx)[SIZE]){
        FILE* inputFile = fopen("./input.mtx", "r");

        if(inputFile == NULL){
            fprintf(stderr, "Opening input file failed\n");
            exit(1);
        }

        for (int i = 0; i <= SIZE-1; i++){
            for (int j = 0; j <= SIZE-1; j++){
                fscanf(inputFile, "%lf", &mtx[i][j]);
            }
        }
        fclose(inputFile);
}

barrier* barrier_new (int noth){
    barrier* b = malloc(sizeof(barrier));
    b->noth = noth;
    b->ready = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
    b->full = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
    b->mtx = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    b->waiting = 0;
    b->leaving = 0;
    return b;
}

void barrier_enter(barrier* b){
    assert(b != NULL);
    pthread_mutex_lock(&(b->mtx));

    while (b->leaving > 0){
        pthread_cond_wait(&(b->ready), &(b->mtx));
    }
    b->waiting++;

    if(b->waiting != b->noth){
        pthread_cond_wait(&(b->full), &(b->mtx));
        b->leaving--;
    }
    else{
        b->leaving = b->waiting;
        b->waiting = 0;
        b->leaving--;
        pthread_cond_broadcast(&(b->full));
    }
    if(b->leaving == 0)
        pthread_cond_broadcast(&(b->ready));

    pthread_mutex_unlock(&(b->mtx));

}
