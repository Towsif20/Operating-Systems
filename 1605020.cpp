#include<iostream>
#include<pthread.h>
#include<cstdio>
#include<unistd.h>
#include<semaphore.h>
#include<cstring>
#include<time.h>
#include<random>

using namespace std;

#define number_of_cyclists 10
#define number_of_servicemen 3
#define number_of_cash_counters 2

pthread_mutex_t mutex[number_of_servicemen];
pthread_mutex_t departCountAccess;

sem_t cash_full, control;
int count = 0;
int departcount = 0;

bool flag = false;
bool flag2 = false;

void* takeService(void* arg)
{
    int rand;

    for (int i = 0; i < number_of_servicemen; i++)
    {
        /* code */
        if(i == 0)
        {
            while(true)
            {
                pthread_mutex_lock(&mutex[i]);

                pthread_mutex_lock(&departCountAccess);
                if(departcount == 0)
                {
                    //allow only when no one is departing
                    pthread_mutex_unlock(&departCountAccess);
                    break;
                }
                pthread_mutex_unlock(&departCountAccess);
                pthread_mutex_unlock(&mutex[i]);
            }
        }

        printf("%s started taking service from serviceman %d\n", (char*)arg, i+1);
        rand = random()%4;
        sleep(rand);
        printf("%s finished taking service from serviceman %d\n", (char*)arg, i+1);

        if(i != number_of_servicemen-1)
            pthread_mutex_lock(&mutex[i+1]);

        pthread_mutex_unlock(&mutex[i]);
    }

    sem_wait(&cash_full);

    rand = random()%4;
    sleep(rand);
    printf("%s started paying the service bill\n", (char*)arg);

    rand = random()%4;
    sleep(rand);

    printf("%s finished paying the service bill\n", (char*)arg);

    pthread_mutex_lock(&departCountAccess);
    departcount++;
    pthread_mutex_unlock(&departCountAccess);

    sem_post(&cash_full);

    // pthread_mutex_lock(&d1);

    for (int i = 0; i < number_of_servicemen; i++)
    {
        /* code */
        pthread_mutex_lock(&mutex[i]);
        //wait until existing cyclists complete taking service
    }
    
   
    pthread_mutex_lock(&departCountAccess);
    departcount--;
    pthread_mutex_unlock(&departCountAccess);

    rand = random()%4;
    sleep(rand);
    printf("%s has departed\n", (char*)arg);
   
    for (int i = 0; i < number_of_servicemen; i++)
    {
        /* code */
        pthread_mutex_unlock(&mutex[i]);
    }
    
    // pthread_mutex_unlock(&d1);

    return NULL;
    
}

int main()
{
    int res;

    for (int i = 0; i < number_of_servicemen; i++)
    {
        /* code */
        res = pthread_mutex_init(&mutex[i], NULL);

        if(res != 0)
        {
            printf("failed initializing mutex for servceman %d\n", i+1);
            return 0;
        }
    }


    res = pthread_mutex_init(&departCountAccess, NULL);

    if(res != 0)
    {
        printf("failed initializing mutex for depart count Access\n");
        return 0;
    }

    res = sem_init(&cash_full, 0, number_of_cash_counters);  

    if(res != 0)
    {
        printf("failed initializing semaphore for cash full\n");
        return 0;
    }

    pthread_t cyclists[number_of_cyclists];

    for(int i=0;i<number_of_cyclists;i++)
    {
        char* id = new char[3];
        strcpy(id, to_string(i+1).c_str());
        res = pthread_create(&cyclists[i], NULL, takeService, id);

        if(res != 0)
        {
            printf("Thread creation failed for cyclist %d\n", i+1);
            return 0;
        }
    }

    for(int i=0;i<number_of_cyclists;i++)
    {
        void* result;
        res = pthread_join(cyclists[i], &result);

        if(res != 0)
        {
            printf("Thread joining failed for cyclist %d\n", i+1);
            return 0;
        }
    }
    
    
    for (int i = 0; i < number_of_servicemen; i++)
    {
        /* code */
        res = pthread_mutex_destroy(&mutex[i]);

        if(res != 0)
        {
            printf("Destroying mutex failed for serviceman %d\n", i+1);
            return 0;
        }
    }



    res = pthread_mutex_destroy(&departCountAccess);
    if(res != 0)
    {
        printf("failed destroying mutex for depart count Access\n");
        return 0;
    }

    res = sem_destroy(&cash_full);

    if(res != 0)
    {
        printf("failed destroying semaphore for cash full\n");
        return 0;
    }
    

    return 0;
}