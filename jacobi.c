#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void* jacobi(void* ptr);
void readInValues(double (*mtx)[4]);

#define NOTH 1

typedef
struct arg_st{
    double (* mtxLEFT) [4];
    double (* mtxRIGHT) [4];
    int n;
    int t;
    int j;
} arg_t;


//Lab computers have 8 threads total
int main (int argc, const char* argv[]){

/* WORKNOTE:
 * try to parse arguments from void*
 * figure out how to stop and swap matrix that we are looking at
 * increment for loops based on thread number
 */

    /*
    *Read in array, might need to move this to another function
    */
    double (* mtxL) [4] = malloc (4* 4* sizeof (double));
    double (* mtxR) [4] = malloc (4* 4* sizeof (double));
    pthread_t thd[NOTH];
    arg_t args[NOTH];
    int j;

    readInValues(mtxL);
    readInValues(mtxR);

    for(j = 0; j < NOTH; j++){
        args[j].mtxLEFT = mtxL;
        args[j].mtxRIGHT = mtxR;
        args[j].n = 4;
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
    int to = ((args->j+1)*args->n)/args->t;

    for(i = from; i < to; i++){
        for(j = 0; j < 3; j++){
            args->mtxLEFT[i][j] = (args->mtxRIGHT[i-1][j] +
                        args->mtxRIGHT[i+1][j] +
                        args->mtxRIGHT[i][j-1] +
                        args->mtxRIGHT[i][j+1]) / 4.0;
            printf("mtxLEFT[%d][%d]: %lf\n", i, j , args->mtxLEFT[i][j]);
        }
    }


}

void readInValues(double (*mtx)[4]){
        FILE* inputFile = fopen("./testInput", "r");

        if(inputFile == NULL){
            fprintf(stderr, "Opening input file failed\n");
            exit(1);
        }

        for (int i = 0; i < 3; i++){
            for (int j = 0; j < 3; j++){
                fscanf(inputFile, "%lf", &mtx[i][j]);
            }
        }
        printf("first: %lf\n", mtx[0][0]);
        printf("last: %lf\n", mtx[3][3]);
        fclose(inputFile);
}







































//
