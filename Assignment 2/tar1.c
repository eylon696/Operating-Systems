//Eylon Ben Simon- 312162951
//Saar Victor - 312392822
#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<semaphore.h> 
#include<string.h>
#include<unistd.h>
#include<fcntl.h>



void resourcesBuilder(char*);
void repairsBuilder(char*);
void requestsBuilder(char*);
void* clockHandler();
void* carFixer(void* carNumber);
int getIndexOfRepairNeeded(int repairNeeded);
int getIndexOfResourceNeeded(int resourceNeeded);
void freeAll();


typedef struct resources{
    int resourceId;
    char* resourceName;
    int resourceAmount;
}resources;

typedef struct repairs{
    int repairId;
    char* repairName;
    int repairTime;
    int numOfResourcesNeeded;
    int* resourcesNumbers;
}repairs;

typedef struct requests{
    int CarId;
    int arrivalTime;
    int numOfRepairs;
    int* repairsNumbers;
}requests;

resources *resourcesArr;
repairs *repairsArr;
requests *requestsArr;
int myClock=0;
int numOfResourcesInTheGarage;
int numOfCars;
int numOfTotalRepairs;
sem_t *semaphoresForTheResources;

   //argv[1]=resources, argv[2]=repairs,  argv[3]=requests 
int main(int argc, char * argv[])
{
    int i,answer;
    
    resourcesBuilder(argv[1]);
    repairsBuilder(argv[2]);
    requestsBuilder(argv[3]);
    int *num=(int*)malloc(sizeof(int)*numOfCars);
    if(num==NULL)
    {
        printf("Error allocating memory");
        exit(1);
    }
    pthread_t threadsArr[numOfCars+1];
    semaphoresForTheResources=(sem_t*)malloc(sizeof(sem_t)*numOfResourcesInTheGarage);//allocating numOfResourcesInTheGarage semaphores
    if(semaphoresForTheResources==NULL)
    {
        printf("Error allocating memory");
        exit(1);
    }
    //Initializind each semaphore has the amount it has 
    for(i=0;i<numOfResourcesInTheGarage;i++)
    {
        sem_init(&semaphoresForTheResources[i],0,resourcesArr[i].resourceAmount);
    }
    //Creating a thread for each Car
    for(i=0;i<numOfCars;i++)
    {
        num[i]=i;
        answer=pthread_create(&threadsArr[i],NULL,carFixer,(void*)&num[i]);
        if(answer!=0)
            {
            printf("can't create threads ");
            exit(0);
            }
    }
    answer=pthread_create(&threadsArr[i],NULL,clockHandler,NULL);//creating a thread that will manage the clock
    if(answer!=0)
        {
        printf("can't create threads ");
        exit(0);
        }
     for(i = 0; i < numOfCars; i++){
        pthread_join(threadsArr[i],NULL);
    }

   freeAll();
   return 0;
}

/**********************************Building the resources structure from resources.txt*****************************************/
void resourcesBuilder(char* resourcesTxt)
{
    int fd,rd,temp,j=0,k=0;
    char buff[2];

       fd=open(resourcesTxt,O_RDONLY);//Opening resources.txt
       if(fd<0)
            exit(1);
       rd=read(fd,buff,1);
       if(rd!=0)
       {
           resourcesArr=(resources*)malloc(sizeof(resources));
           if(resourcesArr==NULL)
           {
               printf("Error allocating memory");
               exit(1);
           }
       }
       while(rd!=0)
	   {
            resourcesArr[j].resourceId=0;
            while(buff[0]!='\t')//means we are in the resourceId
            {
                temp=atoi(&buff[0]);
                resourcesArr[j].resourceId=resourcesArr[j].resourceId*10+temp;
                rd=read(fd,buff,1); 
            }

            rd=read(fd,buff,1);
            resourcesArr[j].resourceName=(char*)malloc(sizeof(char));
            if(resourcesArr[j].resourceName==NULL)
               {
                printf("Error allocating memory");
                exit(1);
               }
            while(buff[0]!='\t')//means we are in the resourceName
               {
                resourcesArr[j].resourceName[k]=buff[0];
                k++;
                rd=read(fd,buff,1);
                if(buff[0]!='\t')
                 {
                    resourcesArr[j].resourceName=(char*)realloc(resourcesArr[j].resourceName,sizeof(char)*(k+1));
                    if(resourcesArr[j].resourceName==NULL)
                     {
                       printf("Error allocating memory");
                       exit(1);
                     }
                 }
               }
            resourcesArr[j].resourceName=(char*)realloc(resourcesArr[j].resourceName,sizeof(char)*(k+1));
            if(resourcesArr[j].resourceName==NULL)
            {
                printf("Error allocating memory");
                exit(1);
            }
            resourcesArr[j].resourceName[k]='\0';

            rd=read(fd,buff,1);
            resourcesArr[j].resourceAmount=0;
            while(buff[0]!='\n')//means we are in the resourceAmount
                {
                temp=atoi(&buff[0]);
                resourcesArr[j].resourceAmount=resourcesArr[j].resourceAmount*10+temp;
                rd=read(fd,buff,1); 
                }
            rd=read(fd,buff,1);
            j++;
            if(rd!=0)
            {
                resourcesArr=(resources*)realloc(resourcesArr,sizeof(resources)*(j+1));
                if(resourcesArr==NULL)
                    {
                    printf("Error allocating memory");
                    exit(1);
                    }
            }
            k=0;
         }
         close(fd);
         numOfResourcesInTheGarage=j;
}
/**********************************Finishing the resources structure from resources.txt*****************************************/
/**********************************Building the repairs structure from resources.txt*****************************************/
void repairsBuilder(char* repairsTxt)
{ 
   int fd,rd,temp,j=0,k=0,index;
   char buff[2];

       fd=open(repairsTxt,O_RDONLY);//Opening repairs.txt
       if(fd<0)
            exit(1);
       rd=read(fd,buff,1);
       if(rd!=0)
       {
          repairsArr=(repairs*)malloc(sizeof(repairs));
           if(repairsArr==NULL)
           {
               printf("Error allocating memory");
               exit(1);
           }
       }
       while(rd!=0)
	   {
            repairsArr[j].repairId=0;
            while(buff[0]!='\t')//means we are in the repairId
            {
                temp=atoi(&buff[0]);
                repairsArr[j].repairId=repairsArr[j].repairId*10+temp;
                rd=read(fd,buff,1); 
            }

            rd=read(fd,buff,1);
            repairsArr[j].repairName=(char*)malloc(sizeof(char));
            if( repairsArr[j].repairName==NULL)
               {
                printf("Error allocating memory");
                exit(1);
               }
            while(buff[0]!='\t')//means we are in the repairName
               {
                repairsArr[j].repairName[k]=buff[0];
                k++;
                rd=read(fd,buff,1);
                if(buff[0]!='\t')
                 {
                    repairsArr[j].repairName=(char*)realloc(repairsArr[j].repairName,sizeof(char)*(k+1));
                    if(repairsArr[j].repairName==NULL)
                     {
                       printf("Error allocating memory");
                       exit(1);
                     }
                 }
               }
            repairsArr[j].repairName=(char*)realloc(repairsArr[j].repairName,sizeof(char)*(k+1));
            if(repairsArr[j].repairName==NULL)
                {
                    printf("Error allocating memory");
                    exit(1);
                }
            repairsArr[j].repairName[k]='\0';

           //Scan repairTime
            rd=read(fd,buff,1);
            repairsArr[j].repairTime=atoi(&buff[0]);
            //stop scan repairTime

            rd=read(fd,buff,1);//scan tab
            rd=read(fd,buff,1);
            repairsArr[j].numOfResourcesNeeded=0;
            while(buff[0]!='\t')//means we are in the numOfResourcesNeeded
            {
                temp=atoi(&buff[0]);
                repairsArr[j].numOfResourcesNeeded=repairsArr[j].numOfResourcesNeeded*10+temp;
                rd=read(fd,buff,1); 
            }
            rd=read(fd,buff,1); 
            if(repairsArr[j].numOfResourcesNeeded>0)//has more than 0 resources
            {
                repairsArr[j].resourcesNumbers=(int*)malloc(sizeof(int)*repairsArr[j].numOfResourcesNeeded);
                if(repairsArr[j].resourcesNumbers==NULL)
                {
                  printf("Error allocating memory");
                  exit(1);
                }

                for(index=0;index<repairsArr[j].numOfResourcesNeeded;index++)
                {
                    repairsArr[j].resourcesNumbers[index]=0;
                    while(buff[0]!='\t'&&buff[0]!='\n')
                    {
                        temp=atoi(&buff[0]);
                        repairsArr[j].resourcesNumbers[index]=repairsArr[j].resourcesNumbers[index]*10+temp;
                        rd=read(fd,buff,1); 
                    }

                    rd=read(fd,buff,1);
                }    
            }
            if(repairsArr[j].numOfResourcesNeeded==0)
                rd=read(fd,buff,1);
            j++;
            if(rd!=0)
            {
               repairsArr=(repairs*)realloc(repairsArr,sizeof(repairs)*(j+1));
                if(repairsArr==NULL)
                    {
                    printf("Error allocating memory");
                    exit(1);
                    }
            }
            k=0;
         }
         close(fd);
         numOfTotalRepairs=j;
}
/**********************************Finishing the Repairs structure from Repairs.txt*****************************************/
/**********************************Building the Requests structure from Requests.txt*****************************************/
void requestsBuilder(char* requestsTxt)
{
   int fd,rd,temp,j=0,index;
   char buff[2];

       fd=open(requestsTxt,O_RDONLY);//Opening Requests.txt
       if(fd<0)
            exit(1);
       rd=read(fd,buff,1);
       if(rd!=0)
       {
          requestsArr=(requests*)malloc(sizeof(requests));
           if(requestsArr==NULL)
           {
               printf("Error allocating memory");
               exit(1);
           }
       }
       while(rd!=0)
	   {
            requestsArr[j].CarId=0;
            while(buff[0]!='\t')//means we are in the CarId
            {
                temp=atoi(&buff[0]);
                requestsArr[j].CarId=requestsArr[j].CarId*10+temp;
                rd=read(fd,buff,1); 
            }
           
            rd=read(fd,buff,1);
            requestsArr[j].arrivalTime=0;
            while(buff[0]!='\t')//means we are in the arrivalTime
            {
                temp=atoi(&buff[0]);
                requestsArr[j].arrivalTime=requestsArr[j].arrivalTime*10+temp;
                rd=read(fd,buff,1); 
            }
            rd=read(fd,buff,1);
            requestsArr[j].numOfRepairs=0;
            while(buff[0]!='\t')//means we are in numOfRepairs
            {
                temp=atoi(&buff[0]);
                requestsArr[j].numOfRepairs=requestsArr[j].numOfRepairs*10+temp;
                rd=read(fd,buff,1); 
            }
            rd=read(fd,buff,1);

            if(requestsArr[j].numOfRepairs>0)//has more than 0 Repairs
            {
                requestsArr[j].repairsNumbers=(int*)malloc(sizeof(int)*requestsArr[j].numOfRepairs);
                if(requestsArr[j].repairsNumbers==NULL)
                {
                  printf("Error allocating memory");
                  exit(1);
                }

                for(index=0;index<requestsArr[j].numOfRepairs;index++)
                {
                    requestsArr[j].repairsNumbers[index]=0;
                    while(buff[0]!='\t'&&buff[0]!='\n')
                    {
                        temp=atoi(&buff[0]);
                        requestsArr[j].repairsNumbers[index]=requestsArr[j].repairsNumbers[index]*10+temp;
                        rd=read(fd,buff,1); 
                    }

                    rd=read(fd,buff,1);
                }    
            }
            j++;
            if(rd!=0)
            {
               requestsArr=(requests*)realloc(requestsArr,sizeof(requests)*(j+1));
                if(requestsArr==NULL)
                    {
                    printf("Error allocating memory");
                    exit(1);
                    }
            }
         }
         close(fd);
         numOfCars=j;   
}
/**********************************Finishing the Requests structure from Requests.txt*****************************************/

void* clockHandler()
{
    while(1)
    {
        if(myClock==23)
            myClock=0;
        else
            myClock++;
        sleep(1);
    }
}
void* carFixer(void* carNumber)
{
    int i,j,indexOfRepairNeeded,indexOfResourceNeeded;
    int carIndex=*((int*)carNumber);//index shows the number of car the thread represents
    while(1)
    {
        if(myClock>=requestsArr[carIndex].arrivalTime)//starting to fix the car when it arrives
        {
            for(i=0;i<requestsArr[carIndex].numOfRepairs;i++)//loop to go through all the repairs this car needs
            {
                indexOfRepairNeeded=getIndexOfRepairNeeded(requestsArr[carIndex].repairsNumbers[i]);
                for(j=0;j<repairsArr[indexOfRepairNeeded].numOfResourcesNeeded;j++)//loop to go through all the resources this repair needs 
                {
                    indexOfResourceNeeded=getIndexOfResourceNeeded(repairsArr[indexOfRepairNeeded].resourcesNumbers[j]);
                    sem_wait(&semaphoresForTheResources[indexOfResourceNeeded]);
                }
                printf("car: %d , time : %d , started %s.\n",requestsArr[carIndex].CarId,myClock,repairsArr[indexOfRepairNeeded].repairName);
                sleep(repairsArr[indexOfRepairNeeded].repairTime);
                printf("car: %d , time : %d , completed %s.\n",requestsArr[carIndex].CarId,myClock,repairsArr[indexOfRepairNeeded].repairName);
                for(j=0;j<repairsArr[indexOfRepairNeeded].numOfResourcesNeeded;j++)//loop to "free" all the resources this repair needs
                {
                    indexOfResourceNeeded=getIndexOfResourceNeeded(repairsArr[indexOfRepairNeeded].resourcesNumbers[j]);
                    sem_post(&semaphoresForTheResources[indexOfResourceNeeded]);
                }
            }
          printf("car: %d , time : %d , service complete\n",requestsArr[carIndex].CarId,myClock);
          break;
        }
    }
    return NULL;
}
int getIndexOfRepairNeeded(int repairNeeded)
{
    int i;
    for(i=0;i<numOfTotalRepairs;i++)
    {
        if(repairsArr[i].repairId==repairNeeded)
            return i;
    }
    return -1;
}
int getIndexOfResourceNeeded(int resourceNeeded)
{
    int i;
    for(i=0;i<numOfResourcesInTheGarage;i++)
    {
        if(resourcesArr[i].resourceId==resourceNeeded)
            return i;
    }
    return -1;
}
void freeAll()
{
    int i;
    for(i=0;i<numOfResourcesInTheGarage;i++)
        free(resourcesArr[i].resourceName);
    free(resourcesArr);
    for(i=0;i<numOfTotalRepairs;i++)
    {
        free(repairsArr[i].repairName);
        free(repairsArr[i].resourcesNumbers);
    }
    free(repairsArr);
    for(i=0;i<numOfCars;i++)
        free(requestsArr[i].repairsNumbers);
    free(requestsArr); 
}
