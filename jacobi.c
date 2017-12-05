//preston was here
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void* jacobi(void* arguments);
void readInValues(double (*mtx)[4]);

double (* mtx) [4];
double (* tmp) [4];
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

    //Make space for the arrays
    mtx = malloc (4* 4* sizeof (double));
    tmp = malloc (4* 4* sizeof (double));


    readInValues(mtx);
    readInValues(tmp);


    pthread_t thread1, thread2;
    const char *message1 = "Thread 1";
    const char *message2 = "Thread 2";
    //Array that contains arguments
    int threadArgs[2] = {1, 8};
    int done = 1;
    //Pass a number associated with each thread
    int num1 = 1;
    int num2 = 2;

    //Could make a flag that is changed when any of the threads find that the difference is too high
    //As each thread is filling in numbers they should be checking the difference between the current number they are adding and the same position on the old matrix

    //while loop checks for iteration
    while(done != 0){
        num1 = pthread_create(&thread1, NULL, jacobi, &threadArgs);
        if(num1){
            fprintf(stderr, "Error - pthread_create() return code: %d\n", num1);
            exit(EXIT_FAILURE);
        }

        num2 = pthread_create(&thread2, NULL, jacobi, &threadArgs);
        if(num2)
        {
            fprintf(stderr,"Error - pthread_create() return code: %d\n", num2);
            exit(EXIT_FAILURE);
        }

        dprintf(2, "pthread_create() for thread 1 returns: %d\n", num1);
        dprintf(2, "pthread_create() for thread 2 returns: %d\n", num2);


        //Join threads together after work is done
        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);
    }



    printf("Arr[1][1]: %lf\n", mtx[1][1]);

    exit(EXIT_SUCCESS);
    //read in the input matrix
    //Will have to decide on how to handle threads
    //Use monitors or sem
    //I assume in either case we would have to use a barrier?
}


/*
Ideas:
Calc by row
1.) start each thread with an offset equal to their number
2.) start each thread at their row number and increment by number of threads

*/

void* jacobi(void* arguments){
    int successNum = 1;
    void* successReturn = &successNum;

    int** args = (int**) arguments;

    int noth = (int)args[1];
    printf("noth: %d\n", noth);

    for (int i = 1; i < 3; i++){
        for (int j = 1; j < 3; j++){
            mtx[i][j] = (tmp[i-1][j] +
            tmp[i+1][j] +
            tmp[i][j-1] +
            tmp[i][j+1]) / 4.0;
            //Check for difference at every value
            printf("during change Arr[i][j]: %lf\n", mtx[i][j]);
        }
    }

    printf("in function Arr[0][0]: %lf\n", mtx[0][0]);
    printf("in function Arr[1][1]: %lf\n", mtx[1][1]);
    return successReturn;
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
