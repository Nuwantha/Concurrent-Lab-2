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
 
int thread_count;
pthread_mutex_t mutex;
struct list_node_s* head = NULL;    

int sample_size=100;
int n=1000;
int m=10000;
int m_member;
int m_insert;
int m_delete;
int count_member=0;
int count_insert=0;
int count_delete=0;
double mean_temp=0;
double standard_deviation_temp=0;
 
int member( int value, struct  list_node_s* head_p );
int insert(int value, struct list_node_s** head_pp);
int delete (int value, struct list_node_s** head_pp);
int printList( struct  list_node_s* head_p ); 
void* linklist_oparation(void* rank);
void calculate_standard_deviation(double elapsed_time_array[sample_size],double mean);
float random_generator();

//main function
int main(int argc, char **argv)
{
    m_member=atoi(argv[1]);
    m_insert=atoi(argv[2]);
    m_delete=atoi(argv[3]);
    thread_count=atoi(argv[4]);
    double elapsed_time_array[sample_size];
    
    //run program sample size times
    for(int j=0;j<sample_size;j++)
    {
        int i=0;
        long thread_id; 
        pthread_t* thread_handles;
        double start;
        double finish;
        double elapsed;

        //initializing linklist with n nodes        
        for(;i<n;i++)
        {   
            int r = rand()%65536;
            if(!insert(r,&head))
            {
                i--;
            }
        }

        //allocating memory
        thread_handles = (pthread_t*) malloc (thread_count*sizeof(pthread_t));  

        start = clock();
        //initilizing mutex
        pthread_mutex_init(&mutex, NULL);

        // creating new thread and ask to run linklist list operation
        for (thread_id = 0; thread_id < thread_count; thread_id++)  
        {
            pthread_create(&thread_handles[thread_id], NULL,linklist_oparation , (void*)thread_id);  
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
        head=NULL;
        count_member=0;
        count_insert=0;
        count_delete=0;
    }
    printf("thread count=%d\n", thread_count);
    printf("mean=%f\n", mean_temp/sample_size);
    calculate_standard_deviation(elapsed_time_array,mean_temp/sample_size);
    return 0;
}

//this function shedule order of linklist operation. until we satisfy required operations amount (m_member=member_count,insert_count=m_insert and delete_count=m_delete) 
void* linklist_oparation(void* rank) 
{
    
    while(count_member<m_member ||count_insert<m_insert || count_delete<m_delete )
    {
        //generate random number to decide function
        float prob =random_generator();
        //printf("random number=%f\n", prob);
        int value = rand()%65536;
        if(prob<m_member)
        {
            pthread_mutex_lock(&mutex);    
            if(count_member<m_member)
            {
                member(value,head);
                count_member++;
            }
            pthread_mutex_unlock(&mutex);
        }
        else if(prob < m_member + m_insert )
        {
            pthread_mutex_lock(&mutex);
            if(count_insert<m_insert)
            {
                insert(value,&head);
                count_insert++;
            }
            pthread_mutex_unlock(&mutex);
        }
        else 
        {           
            pthread_mutex_lock(&mutex);
            if(count_delete<m_delete)
            {
                delete(value,&head);
                count_delete++;
            }
            pthread_mutex_unlock(&mutex);
        }   
    }  
 
   return NULL;
} 

//find members in linklist
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
}

//insert value to link list
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
}  

//delete value from linklist
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
 
}
// print linklist
int printList( struct  list_node_s* head_p ) 
{
    struct list_node_s* curr_p = head_p;
     
    while(curr_p != NULL)
    {
        printf("%d ",curr_p->data);
        curr_p = curr_p->next;
    }
    printf("\n");
}  
//calculate standard deviation
void calculate_standard_deviation(double elapsed_time_array[sample_size],double mean)
{
    for(int j=0;j<sample_size;j++)
    {
        standard_deviation_temp+=pow((elapsed_time_array[j]-mean),2);
    }
    
    printf("sd=%f\n", sqrt(standard_deviation_temp/10));
    
}


// generate random suitable number
float random_generator(){
    float prob = (rand()%m);        
    //value for operation
    if(count_member<m_member && count_insert<m_insert && count_delete<m_delete ){
        prob = (rand()%m);
    }else if(count_member > m_member && count_insert<m_insert && count_delete<m_delete ){
        prob = (rand()%(m_insert+m_delete))+m_member;
    }else if(count_member>m_member && count_insert>m_insert && count_delete<m_delete ){
        prob = (rand()%(m_delete))+m_member+m_insert;
    }else if(count_member>m_member && count_insert<m_insert && count_delete>m_delete ){
        prob = (rand()%(m_insert))+m_member;
    }else if(count_member<m_member && count_insert>m_insert && count_delete>m_delete ){
        prob = (rand()%(m_member));
    }else if(count_member<m_member && count_insert<m_insert && count_delete>m_delete ){
        prob = (rand()%(m_insert+m_member));
    }else if(count_member<m_member && count_insert>m_insert && count_delete<m_delete ){
        prob = (rand()%(m_member));
    }
    return prob;
}