#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
 
struct link_list_node
{
    int data;
    struct link_list_node* next;
};
 

struct link_list_node* head = NULL;    

int sample_size=80;
int n=1000;
int m=10000;
float m_member=5000;
float m_insert=2500;
float m_delete=2500;
int count_member=0;
int count_insert=0;
int count_delete=0;
double mean_temp=0;
double standard_deviation_temp=0;

int member( int value, struct  link_list_node* head_p );
int insert(int value, struct link_list_node** head_pp);
int delete (int value, struct link_list_node** head_pp);
int printList( struct  link_list_node* head_p ); 
void* oparation();
void calculate_standard_deviation(double elapsed_time_array[sample_size],double mean);

 
 
/* Only executed by main thread */
double Serial_pi(long long n);
 
/* Main function */
int main(int argc, char* argv[])
{
    
    double elapsed_time_array[sample_size];
    for(int j=0;j<sample_size;j++)
    {
        double start;
        double finish;
        double elapsed;

        /* initially populating the link list */
        for(int i=0;i<n;i++)
        {   
            int value = rand()%65536;
            if(!insert(value,&head))
            {
                i--;
            }
        }

        start = clock();
        oparation();
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
    
    printf("mean=%f\n", mean_temp/sample_size);
    calculate_standard_deviation(elapsed_time_array,mean_temp/sample_size);
  
    return 0;
}  
 
/*------------------------------------------------------------------
 * Function:       oparation 
 * Purpose:        Compleetea the link list oparations by the thread running this 
 * In arg:         rank
 * Ret val:        ignored
 * Globals in:     n, thread_count, m_member, m_insert, m_delete
 * Global in/out:  count_member, count_insert, count_delete 
 */

void* oparation() 
{
    while(count_member<m_member ||count_insert<m_insert || count_delete<m_delete )
    {
 
        float prob = (rand()%m);
        //printf("%f\n",prob);
        
        int r = rand()%65536;
        if(prob < m_member)
        {
            if(count_member<m_member){
                member(r,head);
                count_member++;
            }
            
        }
        else if(prob < m_member + m_insert)
        {
            if(count_insert<m_insert){
                insert(r,&head);
                count_insert++;
            }
            
        }
        else if(count_delete<m_delete) 
        {           
            if(count_delete<m_delete){
                delete(r,&head);
                count_delete++;
            }
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
int member( int value, struct  link_list_node* head_p )
{
    struct link_list_node* curr_p = head_p;
     
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
int insert(int value, struct link_list_node** head_pp)
{
    struct link_list_node* curr_p = *head_pp;          
    struct link_list_node* pred_p = NULL;
    struct link_list_node* temp_p = NULL;
 
    while(curr_p !=NULL && curr_p->data < value)
    {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }
     
    if(curr_p == NULL || curr_p->data > value)
    {
        temp_p = malloc(sizeof(struct link_list_node));        
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
int delete (int value, struct link_list_node** head_pp)
{
    struct link_list_node* curr_p = *head_pp;
    struct link_list_node* pred_p = NULL;
     
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
int printList( struct  link_list_node* head_p ) 
{
    struct link_list_node* curr_p = head_p;
     
    while(curr_p != NULL)
    {
        printf("%d ",curr_p->data);
        curr_p = curr_p->next;
    }
    printf("\n");
} 

void calculate_standard_deviation(double elapsed_time_array[sample_size],double mean)
{
    for(int j=0;j<sample_size;j++)
    {
        standard_deviation_temp+=pow((elapsed_time_array[j]-mean),2);
    }
    
    printf("sd=%f\n", sqrt(standard_deviation_temp/10));
    
}


