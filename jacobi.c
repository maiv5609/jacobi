#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void* helloThread();
void readInValues(double (*mtx)[1024]);

//Lab computers have 8 threads total
int main (int argc, const char* argv[]){


    /*
    *Read in array, might need to move this to another function
    */

    //Make space for the arrays
    double (* mtx) [1024] = malloc (1024* 1024* sizeof (double));
    double (* tmp) [1024] = malloc (1024* 1024* sizeof (double));

    FILE* inputFile = fopen("./input.mtx", "r");

    if(inputFile == NULL){
        fprintf(stderr, "Opening input file failed\n");
        exit(1);
    }

    for (int i = 0; i < 1024; i++){
        for (int j = 0; j < 1024; j++){
            fscanf(inputFile, "%lf", &mtx[i][j]);
        }
    }
    fclose(inputFile);


    //readInValues(mtx);


    pthread_t thread1, thread2;
    const char *message1 = "Thread 1";
    const char *message2 = "Thread 2";
    int  iret1, iret2;

    iret1 = pthread_create(&thread1, NULL, helloThread, &num1);
    if(iret1){
        fprintf(stderr, "Error - pthread_create() return code: %d\n", iret1);
        exit(EXIT_FAILURE);
    }

    iret2 = pthread_create( &thread2, NULL, helloThread, &num2);
    if(iret2)
    {
        fprintf(stderr,"Error - pthread_create() return code: %d\n",iret2);
        exit(EXIT_FAILURE);
    }

    dprintf(2, "pthread_create() for thread 1 returns: %d\n",iret1);
    dprintf(2, "pthread_create() for thread 2 returns: %d\n",iret2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    exit(EXIT_SUCCESS);
    //read in the input matrix
    //Will have to decide on how to handle threads
    //Use monitors or sem
    //I assume in either case we would have to use a barrier?
}


/*
Ideas:
Calc by row
1.) start each thread at their row number and increment by number of threads
2.) 

jacobi algorithm (sub new and old for input and output arrays)
for (int i = 1; i < 1023; i++){
    for (int j = 1; j < 1023; j++){
        new[i][j] = (old[i-1][j] +
        old[i+1][j] +
        old[i][j-1] +
        old{i][j+1]) / 4.0;
    }
}
*/

void* helloThread(int* num){
    printf("Hello other thread %d\n", *num);
}

void readInValues(double (*mtx)[1024]){


}







































//
