#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>
 
struct list_node_s
{
    int data;
    struct list_node_s* next;
};
 
const int MAX_THREADS = 1024;
 
long thread_count=8;
pthread_mutex_t mutex;
struct list_node_s* head = NULL;    

int sample_size=80;
int n=1000;
int m=10000;
float m_member=9900;
float m_insert=50;
float m_delete=50;
int count_member=0;
int count_insert=0;
int count_delete=0;
double mean_temp=0;
double standard_deviation_temp=0;
 
int member( int value, struct  list_node_s* head_p );
int insert(int value, struct list_node_s** head_pp);
int delete (int value, struct list_node_s** head_pp);
int printList( struct  list_node_s* head_p ); 
void* thread_oparation(void* rank);
void calculate_standard_deviation(double elapsed_time_array[sample_size],double mean);
 
 
/* Only executed by main thread */
double Serial_pi(long long n);
 
/* Main function */
int main(int argc, char* argv[])
{
    double elapsed_time_array[sample_size];
    for(int j=0;j<sample_size;j++)
    {
        int i=0;
        long thread_id;  /* Use long in case of a 64-bit system */
        pthread_t* thread_handles;
        double start;
        double finish;
        double elapsed;

        /* read command line arguments */
        //Get_args(argc, argv); 

        /* initially populating the link list */
        for(;i<n;i++)
        {   
            int r = rand()%65536;
            if(!insert(r,&head))
            {
                i--;
            }
        }


        //printf("%f\n",mMember);
        //printf("%f\n",mInsert);


        thread_handles = (pthread_t*) malloc (thread_count*sizeof(pthread_t));  

        start = clock();
        pthread_mutex_init(&mutex, NULL);

        for (thread_id = 0; thread_id < thread_count; thread_id++)  
        {
            pthread_create(&thread_handles[thread_id], NULL,thread_oparation , (void*)thread_id);  
        }

        for (thread_id = 0; thread_id < thread_count; thread_id++) 
        {
            pthread_join(thread_handles[thread_id], NULL); 
        }

        pthread_mutex_destroy(&mutex);
        finish = clock();
        elapsed = (finish - start)/CLOCKS_PER_SEC;
        mean_temp+=elapsed;
        elapsed_time_array[j]=elapsed;
            
        printf("Elapsed time = %e seconds\n", elapsed); 
        printf("m_member= %d\n", count_member); 
        printf("m_insert=%d\n", count_insert); 
        printf("m_delete=%d\n", count_delete);
        printf("thread count=%ld\n", thread_count);
        head=NULL;
        count_member=0;
        count_insert=0;
        count_delete=0;
    }
    
    printf("mean=%f\n", mean_temp/sample_size);
    calculate_standard_deviation(elapsed_time_array,mean_temp/sample_size);
    return 0;
}/*main*/  
 
/*------------------------------------------------------------------
 * Function:       thread_oparation 
 * Purpose:        Compleetea the link list oparations by the thread running this 
 * In arg:         rank
 * Ret val:        ignored
 * Globals in:     n, thread_count, mMember, mInsert, mDelete
 * Global in/out:  count_member, count_insert, count_delete 
 */
void* thread_oparation(void* rank) 
{
    long my_rank = (long) rank;
    double factor, my_sum = 0.0;
    long long i;
    long long my_m = m/thread_count;
    while(count_member<m_member ||count_insert<m_insert || count_delete<m_delete )
    {
 
    
        float prob = (rand()%10000);
        //printf("%f\n",prob);
     
     
        int r = rand()%65536;
        if(prob<m_member && count_member<m_member)
        {
            pthread_mutex_lock(&mutex);
            
            member(r,head);
            count_member++;
            pthread_mutex_unlock(&mutex);
        }
        else if(prob < m_member + m_insert  && count_insert<m_insert)
        {
            pthread_mutex_lock(&mutex);
            insert(r,&head);
            count_insert++;
            pthread_mutex_unlock(&mutex);
        }
        else if (count_delete<m_delete)
        {           
            pthread_mutex_lock(&mutex);
            delete(r,&head);
            count_delete++;
            pthread_mutex_unlock(&mutex);
        }   
    }  
 
   return NULL;
}  /* Thread_sum */
 
/*------------------------------------------------------------------
 * Function:       member
 * Purpose:        Check if the given values is in the link list
 * In arg:         value, head_p
 * Globals in:     
 * Global in/out:   
 * Return val: Return 1 if value exist otherwise 0
 */
int member( int value, struct  list_node_s* head_p )
{
    struct list_node_s* curr_p = head_p;
     
    while( curr_p != NULL && curr_p->data < value )
    {
        curr_p = curr_p->next;
    }
 
    if(curr_p == NULL || curr_p->data > value)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}/* member */
 
/*------------------------------------------------------------------
 * Function:       insert
 * Purpose:        Add new values in to link list
 * In arg:         value, head_p
 * Globals in:  
 * Global in/out:  
 * Return val: Return 1 if value successfully add to the list otherwise 0
 */
int insert(int value, struct list_node_s** head_pp)
{
    struct list_node_s* curr_p = *head_pp;          
    struct list_node_s* pred_p = NULL;
    struct list_node_s* temp_p = NULL;
 
    while(curr_p !=NULL && curr_p->data < value)
    {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }
     
    if(curr_p == NULL || curr_p->data > value)
    {
        temp_p = malloc(sizeof(struct list_node_s));        
        temp_p->data = value;
        temp_p->next = curr_p;
         
        if(pred_p == NULL)
        {
            *head_pp = temp_p;
        }
        else
        {
            pred_p->next = temp_p;
        }
        return 1;
  
    }
    else
    {
        return 0;
    }
}   /*insert*/
 
 
/*------------------------------------------------------------------
 * Function:       delete
 * Purpose:        remove values from the link list 
 * In arg:         value, head_p
 * Globals in:     
 * Global in/out:  
 * Return val: Return 1 if value successfully remove from the list otherwise 0
 */
int delete (int value, struct list_node_s** head_pp)
{
    struct list_node_s* curr_p = *head_pp;
    struct list_node_s* pred_p = NULL;
     
    while(curr_p != NULL && curr_p->data < value)
    {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }   
     
    if(curr_p != NULL && curr_p -> data < value)
    {
        if(pred_p == NULL){
            *head_pp = curr_p->next;
            free(curr_p);
        }
        else
        {
            pred_p->next = curr_p->next;
            free(curr_p);
        }
        return 1;
         
    }
    else
    {
        return 0;
    }
 
}   /*delete*/
 
 
/*------------------------------------------------------------------
 * Function:       printList
 * Purpose:        Add in the terms computed by the thread running this 
 * In arg:         value, head_p
 * Globals in:     
 * Global in/out:   
 * Return val: Estimate of pi using n terms of Maclaurin series
 */
int printList( struct  list_node_s* head_p ) 
{
    struct list_node_s* curr_p = head_p;
     
    while(curr_p != NULL)
    {
        printf("%d ",curr_p->data);
        curr_p = curr_p->next;
    }
    printf("\n");
}   /*printList */


void calculate_standard_deviation(double elapsed_time_array[sample_size],double mean)
{
    for(int j=0;j<sample_size;j++)
    {
        standard_deviation_temp+=pow((elapsed_time_array[j]-mean),2);
    }
    
    printf("sd=%f\n", sqrt(standard_deviation_temp/10));
    
}
