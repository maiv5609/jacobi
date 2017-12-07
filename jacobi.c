#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define SIZE 4
#define NOTH 2
#define DELTA .1

void* jacobi(void* ptr);
void readInValues(double (*mtx)[SIZE]);

typedef
struct arg_st{
    double (* mtxLEFT) [SIZE];
    double (* mtxRIGHT) [SIZE];
    int n;
    int t;
    int j;
} arg_t;


//Lab computers have 8 threads total
int main (int argc, const char* argv[]){

/* WORKNOTE:

    /*
    *Read in array, might need to move this to another function
    */
    double (* mtxL) [SIZE] = malloc (SIZE* SIZE* sizeof (double));
    double (* mtxR) [SIZE] = malloc (SIZE* SIZE* sizeof (double));
    pthread_t thd[NOTH];
    arg_t args[NOTH];
    int j;

    readInValues(mtxL);
    readInValues(mtxR);


    for(j = 0; j < NOTH; j++){
        args[j].mtxLEFT = mtxL;
        args[j].mtxRIGHT = mtxR;
        args[j].n = SIZE;
        args[j].t = NOTH;
        args[j].j = j;
        if(pthread_create(&thd[j], NULL, &jacobi, &args[j])){
            perror("pthread_create");
            return -1;
        }
    }

    void* unused;

    for(j = 0; j < NOTH; j++){
        pthread_join(thd[j], &unused);
    }

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
    int to = ((args->j+1)*args->n)/args->t - args->j;
    if (from == 0)
        from = 1;
    if (to == args->n)
        to = args->n - 1;
    double max;
    double test;
    int done = 0;

    printf("from: %d thread: %d\n" , from, args->j);
    printf("to: %d thread: %d\n" , to - 1, args->j);

    while (!done){
        max = 0;

        for(int i = from; i < to; i++){
            for(int j = 1; j < SIZE-1; j++){
                double prev = args->mtxLEFT[i][j];
                args->mtxLEFT[i][j] = (args->mtxRIGHT[i-1][j] +
                            args->mtxRIGHT[i+1][j] +
                            args->mtxRIGHT[i][j-1] +
                            args->mtxRIGHT[i][j+1]) / 4.0;
                printf("mtxLEFT[%d][%d]: %.2lf\n", i, j , args->mtxLEFT[i][j]);
                test = args->mtxLEFT[i][j] - prev;
                if (test > max)
                    max = test;

                //printf("max: %lf\n", max);
            }
        }

        if (max < DELTA)
            done = 1;
        else {
            double (* temp)[SIZE] = args->mtxLEFT;
            args->mtxLEFT = args->mtxRIGHT;
            args->mtxRIGHT = temp;
        }


    }

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

        for (int i = 0; i <= SIZE-1; i++){
            printf("row %d: ", i);
            for (int j = 0; j <= SIZE-1; j++){
                printf("%0.lf ", mtx[i][j]);
            }
            printf("\n");
        }


        fclose(inputFile);
}

typedef struct barrier_st{
    int noth;
    pthread_cond_t ready;
    pthread_cond_t full;
    pthread_mutex_t mtx;
    int waiting;
    int leaving;
} barrier;

barrier* barrier_new (int noth){
    barrier* b = malloc(sizeof(barrier));
    b->noth = noth;

    return b;
}

void barrier_enter(barrier* b){
    assert(b != NULL);
    pthread_mutex_lock(&(b->mtx));
    while (b->leaving > 0)
        pthread_cond_wait(&(b->ready), &(b->mtx));
        b->waiting++;

    if(b->waiting != b->noth){
        pthread_cond_wait(&(b->full), &(b->mtx));
        b->leaving--;;
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
