#define _GNU_SOURCE //this allowed me to use syscall idk had to google it
#include <stdio.h>
#include <string.h>
#include <stdlib.h> //for malloc since array is so big
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <pthread.h> // for pthreads
#include <time.h>
#include <sys/mman.h> //needed for the provided function
#include <ctype.h> //needed for the provided function
#include <sys/syscall.h> //needed for the provided function
//Some of the libraries needed for the print progress function were not listed
//in the allowed libraries so I had to add them or else it wouldnt work

#define ANSI_COLOR_GRAY    "\x1b[30m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_WHITE   "\x1b[37m"

#define ANSI_COLOR_RESET   "\x1b[0m"

#define TERM_CLEAR() printf("\033[H\033[J")
#define TERM_GOTOXY(x,y) printf("\033[%d;%dH", (y), (x))


#define MAX_SIZE 2000000

typedef struct _thread_data_t {
    int localTid;
    const int *data;
    int numVals;
    pthread_mutex_t *lock;
    long long int *totalSum;
    } thread_data_t;

void print_progress(pid_t localTid, size_t value);

void* arraySum(void* input);

int main(int argc, char* argv[]){
    if(argc != 2){
        printf("There are not enough parameters please try again");
        return -1;
    }
    int threadcount = atoi(argv[1]);
    if(threadcount <= 0){
        printf("Error fix");
        return -1;
    }
    //https://www.geeksforgeeks.org/dynamic-memory-allocation-in-c-using-malloc-calloc-free-and-realloc/#malloc 
    //use for malloc since array is too big
    int *data = (int *)malloc(sizeof(int) * MAX_SIZE); //has to be called data oops
    for(int i = 0; i< MAX_SIZE; i++){
        data[i] = rand() % 101;
    }
    long long int totalSum = 0;
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);
    pthread_t threads[threadcount];
    thread_data_t thread_data[threadcount];

    //check the last project I feel like we made an array
    for(int i = 0; i < threadcount; i++){
        thread_data[i].localTid = i;
        thread_data[i].data = data;
        thread_data[i].numVals = MAX_SIZE;
        thread_data[i].lock = &lock;
        thread_data[i].totalSum = &totalSum;
        pthread_create(&threads[i], NULL, arraySum, &thread_data[i]);
    }

    
    for(int i = 0; i < threadcount; i++){ // this portion was also taken from my homework 2
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock); //??
    free(data); // ??

    printf("Total Sum: %lld\n", totalSum);

    return 0;

    
}
//a lot of this project is similar to homework two especially the array sum funciton
void* arraySum(void* input){
    thread_data_t* thread_data = (thread_data_t*)input; //name must be the same from main oops
    //long long int totalSum = 0;
    struct timespec startIndex;
    struct timespec endIndex;
    long latency;
    long latency_max = 0;
    while(1){
        for(int temp = 0; temp < 10; temp++){
        clock_gettime(CLOCK_MONOTONIC, &startIndex);

        for(int i = 0; i < thread_data-> numVals; i++){
            pthread_mutex_lock(thread_data->lock);
            *(thread_data->totalSum)+= thread_data->data[i];
            pthread_mutex_unlock(thread_data->lock);
        }
        clock_gettime(CLOCK_MONOTONIC, &endIndex);
        

        latency= (endIndex.tv_sec - startIndex.tv_sec) *(long)1e9 + (endIndex.tv_nsec - startIndex.tv_nsec);
        //totalSum += threadcount;
        //*(thread_data->totalSum) += totalSum;
        

        if(latency > latency_max){
            latency_max = latency; // need to actually use latency
        }

        print_progress(thread_data->localTid, latency_max);
        
    }
    }

    return NULL;
}

void print_progress(pid_t localTid, size_t value) {
    pid_t tid = syscall(__NR_gettid);

    TERM_GOTOXY(0,localTid+1);

char prefix[256];
    size_t bound = 100;
    sprintf(prefix, "%d: %ld (ns) \t[", tid, value);
const char suffix[] = "]";
const size_t prefix_length = strlen(prefix);
const size_t suffix_length = sizeof(suffix) - 1;
char *buffer = calloc(bound + prefix_length + suffix_length + 1, 1);
size_t i = 0;

strcpy(buffer, prefix);
for (; i < bound; ++i)
{
    buffer[prefix_length + i] = i < value/10000 ? '#' : ' ';
}
strcpy(&buffer[prefix_length + i], suffix);
    
    if (!(localTid % 7)) 
        printf(ANSI_COLOR_WHITE "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
    else if (!(localTid % 6)) 
        printf(ANSI_COLOR_BLUE "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
    else if (!(localTid % 5)) 
        printf(ANSI_COLOR_RED "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
    else if (!(localTid % 4)) 
        printf(ANSI_COLOR_GREEN "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
    else if (!(localTid % 3)) 
        printf(ANSI_COLOR_CYAN "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
    else if (!(localTid % 2)) 
        printf(ANSI_COLOR_YELLOW "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
    else if (!(localTid % 1)) 
        printf(ANSI_COLOR_MAGENTA "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
    else
        printf("\b%c[2K\r%s\n", 27, buffer);

fflush(stdout);
free(buffer);
}


