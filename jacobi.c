#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define SIZE 4
#define NOTH 1
#define DELTA 0.001

typedef struct arg_st{
    double (* mtxLEFT) [SIZE];
    double (* mtxRIGHT) [SIZE];
    double (* maxArr) [NOTH];
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

void* jacobi(void* ptr);
void readInValues(double (*mtx)[SIZE]);
barrier* barrier_new (int noth);
void barrier_enter(barrier* b);

pthread_barrier_t threadBarrier;
//barrier threadBarrier;

//Lab computers have 8 threads total
int main (int argc, const char* argv[]){
    /*
    *Read in array, might need to move this to another function
    */
    double (* mtxL) [SIZE] = malloc (SIZE* SIZE* sizeof (double));
    double (* mtxR) [SIZE] = malloc (SIZE* SIZE* sizeof (double));
    double (* maxes) [NOTH] = malloc (SIZE * sizeof(double));
    //memset(maxes, 0, sizeof (maxes));
    pthread_t thd[NOTH];
    arg_t args[NOTH];
    int j;

    readInValues(mtxL);
    readInValues(mtxR);

    //threadBarrier = *barrier_new(NOTH);
    pthread_barrier_init(&threadBarrier, NULL, NOTH);

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
        //printf("Joining\n");
        pthread_join(thd[j], &unused);
    }

    printf("mtxR[1][1]: %lf out of thread\n", mtxR[1][1]);
    printf("mtxL[1][1]: %lf out of thread\n", mtxL[1][1]);

    free(mtxL);
    free(mtxR);
    return 0;

}

/*
Ideas:
Calc by row
1.) start each thread with an offset equal to their number
2.) start each thread at their row number and increment by number of threads

*/

void* jacobi(void* ptr){
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

    printf("from: %d thread: %d\n" , from, args->j);
    printf("to: %d thread: %d\n" , to - 1, args->j);

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
                if (test > *(args->maxArr[args->j]))
                    *(args->maxArr[j]) = test;
                printf("*(args->maxArr[j])): %lf\n" , *(args->maxArr[j]));

            }
        }
        pthread_barrier_wait(&threadBarrier);
        //mutex lock
        for(int i = 0; i < NOTH; i++){
            if (highestMax < *(args->maxArr[i])){
                highestMax = *(args->maxArr[i]);
                printf("highestMax: %lf\n" , highestMax);
            }
        }
        if (highestMax < DELTA)
             done = 1;



        //barrier_enter(&threadBarrier);
        highestMax = 0.0;
        for(int i = from; i < to; i++){
            for(int j = 1; j < SIZE-1; j++){
                double prev = args->mtxLEFT[i][j];
                args->mtxRIGHT[i][j] = (args->mtxLEFT[i-1][j] +
                            args->mtxLEFT[i+1][j] +
                            args->mtxLEFT[i][j-1] +
                            args->mtxLEFT[i][j+1]) / 4.0;
                //printf("mtxRIGHT[%d][%d]: %.2lf\n", i, j , args->mtxRIGHT[i][j]);
                test = args->mtxRIGHT[i][j] - prev;
                if (test > *(args->maxArr[args->j]))
                    *(args->maxArr[args->j]) = test;
                //printf("test: %lf\n" , test);
                //printf("*(args->maxArr[j])): %lf\n" , *(args->maxArr[j]));
                //printf("max: %lf\n", max);
            }
        }
        pthread_barrier_wait(&threadBarrier);
        for(int i = 0; i < NOTH; i++){
            if (highestMax < *(args->maxArr[i])){
                highestMax = *(args->maxArr[i]);
                printf("highestMax: %lf\n" , highestMax);
            }
        }
        if (highestMax < DELTA)
             done = 1;


        //barrier_enter(&threadBarrier);

    }
    //barrier_enter(&threadBarrier);

    //printf("mtxRIGHT[10][10]: %lf in thread\n", args->mtxRIGHT[10][10]);
    //printf("mtxLEFT[10][10]: %lf in thread\n", args->mtxLEFT[10][10]);
    return NULL;
}

void readInValues(double (*mtx)[SIZE]){
        FILE* inputFile = fopen("./testInput", "r");

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
    // printf("Waiting: %d\n", b->waiting);
    // printf("Leaving: %d\n\n", b->leaving);
    pthread_mutex_lock(&(b->mtx));

    while (b->leaving > 0){
        pthread_cond_wait(&(b->ready), &(b->mtx));
    }
    b->waiting++;

    if(b->waiting != b->noth){
        // printf("Start Waiting\n");
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




































//
