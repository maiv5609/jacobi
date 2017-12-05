//preston was here
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void* helloThread();
void readInValues(double (*mtx)[4]);

//Lab computers have 8 threads total
int main (int argc, const char* argv[]){


    /*
    *Read in array, might need to move this to another function
    */

    //Make space for the arrays
    double (* mtx) [4] = malloc (4* 4* sizeof (double));
    double (* tmp) [4] = malloc (4* 4* sizeof (double));


    readInValues(mtx);


    pthread_t thread1, thread2;
    const char *message1 = "Thread 1";
    const char *message2 = "Thread 2";
    //Array that contains arguments


    //Pass a number associated with each thread
    int num1 = 1;
    int num2 = 2;

    //Could make a flag that is changed when any of the threads find that the difference is too high
    //As each thread is filling in numbers they should be checking the difference between the current number they are adding and the same position on the old matrix


    num1 = pthread_create(&thread1, NULL, helloThread, &num1);
    if(iret1){
        fprintf(stderr, "Error - pthread_create() return code: %d\n", num1);
        exit(EXIT_FAILURE);
    }

    num2 = pthread_create( &thread2, NULL, helloThread, &num2);
    if(iret2)
    {
        fprintf(stderr,"Error - pthread_create() return code: %d\n", num2);
        exit(EXIT_FAILURE);
    }

    dprintf(2, "pthread_create() for thread 1 returns: %d\n", num1);
    dprintf(2, "pthread_create() for thread 2 returns: %d\n", num2);


    //Join threads together after work is done
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    //Check iteration flag

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


for (i = 1; i < 1023; i++){
    for (j = 1; j < 1023; j++){
        new[i][j] = (old[i-1][j] +
        old[i+1][j] +
        old[i][j-1] +
        old{i][j+1]) / 4.0;
    }
}
*/

void* jacobi(int* noth, int* numT){
    printf("Hello other thread %d\n", *num);
}

void readInValues(double (*mtx)[4]){
        FILE* inputFile = fopen("./testInput", "r");

        if(inputFile == NULL){
            fprintf(stderr, "Opening input file failed\n");
            exit(1);
        }

        for (int i = 0; i < 4; i++){
            for (int j = 0; j < 4; j++){
                fscanf(inputFile, "%lf", &mtx[i][j]);
            }
        }
        printf("first: %lf\n", mtx[0][0]);
        printf("last: %lf\n", mtx[3][3]);
        fclose(inputFile);
}







































//
