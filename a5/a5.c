//
//  main.c
//  a5
//
//  Created by Nathan Sherwood on 2020-07-12.
//  Copyright © 2020 Nathan Sherwood. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_REF_STRING 10000
#define MAX_FRAME 2500
#define MAX_PAGE_REF 5000

int refString[MAX_REF_STRING]; //input from stdin
int *frame;
int *tmp;
int i, j, k;
int length;
bool loaded;
int num_frames;
int optimal_faults = -1;
int fifo_faults = -1;
int lru_faults = -1;
int lfu_faults = -1;
int mfu_faults = -1;
int second_chance_faults = -1;

/* Used to initialize storage array for each iteration */
void initialize()
{
    for(i = 0; i < num_frames; i++)
    {
        frame[i] = -1;
        tmp[i] = - 1;
    }
};

/* Used to check if reference string data is loaded into memory */
bool isLoaded(int data)
{
    loaded = false;
    for(j = 0; j < num_frames; j++)
    {
        if(frame[j] == data)
        {
            loaded = true;
            break;
        }
    }
    return loaded;
}

int getIndex(int data)
{
    int index = -1;
    for(k = 0; k < num_frames; k++)
    {
        if(frame[k] == data)
        {
            index = k;
            break;
        }
    }
    return index;
}

void fifo()
{
    initialize();
    fifo_faults = 0;
    for(i = 0; i < length ; i++)
    {
        if(!isLoaded(refString[i]))
        {
            for(k = 0; k < num_frames - 1; k++)
            {
                frame[k] = frame[k + 1];
            }
            frame[k] = refString[i];
            fifo_faults++;
        }
    }
}

void optimal()
{
    initialize();
    optimal_faults = 0;
    int max, page;
    int index = -1;
    bool found;
    
    for(i = 0; i < length; i++)
    {
        if(!isLoaded(refString[i]))
        {
            for(j = 0; j < num_frames; j++)
            {
                page = frame[j];
                found = false;
                for(k = i; k < length; k++)
                {
                    if(page == refString[k])
                    {
                        tmp[j] = k;
                        found = true;
                        break;
                    }
                    else
                        found = false;
                }
                if(!found) tmp[j] = MAX_PAGE_REF;
            }
            max = -1;
            for(j = 0; j < num_frames; j++)
            {
                if(tmp[j] > max)
                {
                    max = tmp[j];
                    index = j;
                }
            }
            frame[index] = refString[i];
            optimal_faults++;
        }
    }
   
}

void lfu()
{
    int min;
    int index = -1;
    int hitIndex = -1;
    int runningCnt = 0;
    int tmpIndex = 0;
    initialize();
    lfu_faults = 0;
    
    for(i = 0; i < length; i++)
    {
        if(isLoaded(refString[i]))
        {
            hitIndex = getIndex(refString[i]);
            tmp[hitIndex]++;
        }
        else
        {
            lfu_faults++;
            if(tmpIndex < num_frames)
            {
                frame[tmpIndex] = refString[i];
                tmp[tmpIndex]++;
                tmpIndex++;
            }
            else
            {
                min = MAX_PAGE_REF;
                for(k = 0; k < num_frames; k++)
                    if(tmp[k] < min)
                    {
                        min = tmp[k];
                        index = k;
                    }
                frame[index] = refString[i];
                runningCnt = 0;
                for(k = 0; k <= i; k++)
                    if(refString[i] == refString[k])
                        runningCnt++;
                tmp[index] = runningCnt;
            }
        }
    }
}

void ReadData()
{
    i = 0;
    length = 0;
    while(scanf("%d", &refString[i])!= EOF)
    {
        i++;
        length++; /* Page reference string length */
    }
};


void PrintData()
{
    printf("\n");
    for(i = 0; i < length; i++)
    {
        printf("%d ", refString[i]);
    }
    printf("\n");
    printf("For a system with %d frames:\n", num_frames);
    printf("Optimal page faults: %d\n", optimal_faults);
    printf("FIFO page faults: %d\n", fifo_faults);
    printf("LRU page faults: %d\n", lru_faults);
    printf("LFU page faults: %d\n", lfu_faults);
    printf("MFU page faults: %d\n", mfu_faults);
    printf("Second chance page faults: %d\n", second_chance_faults);
};

int main(int argc, char * argv[])
{
    char c;
    char *progname = argv[0];
    
    switch(argc)
    {
        case 1:
            fprintf(stderr, "Please enter 2 inputs\n");
            break;
            
        case 2:
            if (sscanf(argv[1], "%d%c", &num_frames, &c) == 1)
            {
                if (num_frames < 1)
                {
                    fprintf(stderr, "%s: invalid argument /%s/\n",
                            progname, argv[1]);
                    return EXIT_FAILURE;
                }
                else
                {
                    frame = (int *)malloc(num_frames * sizeof(int));
                    tmp = (int *)malloc(num_frames * sizeof(int));
                }
            }
            
            ReadData();
            fifo();
            optimal();
            lfu();
            PrintData();
            break;
            
    }
    
    
    return EXIT_SUCCESS;
}
