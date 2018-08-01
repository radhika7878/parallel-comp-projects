#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#define NEWSITEM_NUMBER 5
#define EDITOR_SIZE 4

typedef struct news
{
  int topic;
  int timestamp;
}news;

 main(int argc, char **argv)
   {
    
    int my_id, editor_process, num_procs, ierr, i;
    //MPI_Status status;
    news newsItem[NEWSITEM_NUMBER];
    char fileName[100];
    FILE *fp;

    /* Let process 0 be the editor process. */
    MPI_Datatype mpi_news;
    MPI_Datatype rowbool;
    MPI_Datatype news_arr;

    char *headlines[] = {"JNU Sedition","Union Budget","Vijay Mallya", "Cultural Day","Donald Trump","2016 Presidential",
    "Apple Inc.","North Korea","ADBlock","Sunny Leone","Rahul Gandhi","PoK","Intolerance","Hezbollah Lebanon","Syrian War"};

   ierr = MPI_Init(&argc, &argv);

    /* Find out MY process ID, and how many processes were started. */

    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    editor_process=my_id-(my_id%EDITOR_SIZE);

    //derived datatype for struct news

    int blocklengths[2] = {1, 1};
    MPI_Datatype types[2] = {MPI_INT, MPI_INT};
    MPI_Aint offsets[2],extent;
    MPI_Type_extent(MPI_INT,&extent);
    offsets[0] = 0;
    offsets[1] = 1*extent;
    MPI_Type_create_struct(2, blocklengths, offsets, types, &mpi_news);
    MPI_Type_commit(&mpi_news);

    //derived datatype for array of news
    MPI_Type_contiguous(NEWSITEM_NUMBER, mpi_news, &news_arr);
    MPI_Type_commit(&news_arr);

    //derived datatype for row of 0's and 1's
    MPI_Type_contiguous(NEWSITEM_NUMBER, MPI_INT, &rowbool);
    MPI_Type_commit(&rowbool);

    if(my_id %EDITOR_SIZE!=0)  //reporter processes
    {
        sprintf(fileName, "%d", my_id);
        strcat(fileName,".txt");   //filename
       /* read from file,fill up struct. */
       if((fp=fopen(fileName,"r"))==NULL)
       {
           printf("NO SUCH FILE\n");
           return 0;
       }
       i=0;
       while(fscanf(fp,"%d %d",&newsItem[i].topic,&newsItem[i].timestamp)==2)
       {
           //printf("%d,%d\n",newsItem[i].topic,newsItem[i].timestamp);
           i++;
       }
       if(feof(fp))
       {
           //printf("ALL NEWS READ\n");
           fclose(fp);
       }
       else
       {
           printf("file not read completely\n");
       }

       //news read,stored in structure set barrier
       //all processes have stored data in newsItem
    }
    MPI_Barrier(MPI_COMM_WORLD);  

    int boolean[NEWSITEM_NUMBER];
    int cl = num_procs/EDITOR_SIZE;
    int sum1[cl];
    int a1;
    for(a1=0;a1<cl;a1++)
      sum1[a1]=0;

    int index;
    index=editor_process/EDITOR_SIZE;

    if(my_id%EDITOR_SIZE!=0)	//reporter process
    {
      int k;
      int c;
      for(c=0;c<NEWSITEM_NUMBER;c++)
        boolean[c] =1;
      //printf("editor process is %d for %d",editor_process,my_id);
      for(k=editor_process+1;k<editor_process+EDITOR_SIZE-1;){
        //printf("k is %d for %d\n",k , my_id);
        if(my_id == k){
          int l;
          for(l=k+1;l<editor_process+EDITOR_SIZE;l++){             
            MPI_Send(newsItem,1,news_arr,l,10,MPI_COMM_WORLD);
          }
          int no_send = EDITOR_SIZE-(my_id%EDITOR_SIZE)-1;
          MPI_Status st; 
          int boo[NEWSITEM_NUMBER];       
          for(l=0;l<no_send;l++){
             MPI_Recv(boo,NEWSITEM_NUMBER,MPI_INT,MPI_ANY_SOURCE,10,MPI_COMM_WORLD,&st);
             //printf("%d recieved from %d\n",my_id,st.MPI_SOURCE);
             int d;
             for(d=0;d<NEWSITEM_NUMBER;d++)
              boolean[d] = boolean[d] && boo[d];
          }
          int a;
          // for(a=0;a<NEWSITEM_NUMBER;a++){
          //   printf(" %d : %d ",my_id, boolean[a]);
          // }
          // printf("\n");
        }
        else if(my_id>k){
          news n[NEWSITEM_NUMBER];
          MPI_Status st;
          MPI_Recv(n, NEWSITEM_NUMBER,mpi_news,k,10,MPI_COMM_WORLD,&st);
          int a;
          // for(a=0;a<NEWSITEM_NUMBER;a++)
          //   printf("%d,%d\n",n[a].topic,n[a].timestamp);
          int boo[NEWSITEM_NUMBER];
          int b;
          for(a=0;a<NEWSITEM_NUMBER;a++)
            boo[a]=1;
          a=b=0;
          while(a<NEWSITEM_NUMBER && b<NEWSITEM_NUMBER){
            //printf("%d in while\n", my_id);
            if(n[a].topic<newsItem[b].topic)
              a++;
            else if(n[a].topic>newsItem[b].topic)
              b++;
            else{
              if(n[a].timestamp<=newsItem[b].timestamp)
                boo[a]=0;
              else
                boolean[b]=0;                
              a++;
              b++;
            }
          }
          MPI_Send(boo,1,rowbool,k,10,MPI_COMM_WORLD);
          //printf("%d send %d\n",my_id,k);
        }
        // else if(my_id<k){
        //   printf("%d is leaving",my_id);
        //   break;
        // }
        //printf("%d is here with %d\n",my_id,k);
        k++;
        //printf("for %d now k is %d\n",my_id, k);
        //printf("for %d other value is: %d\n",my_id,editor_process+EDITOR_SIZE-1);
        int x = k;
        int y = editor_process+EDITOR_SIZE-1;
        if(x==y){
          //printf("hello");
          break;
        }
          
      }
      

      //printf("%d out of for loop", my_id);

      
      
      for(c=0;c<NEWSITEM_NUMBER;c++){
        sum1[index] = sum1[index] + boolean[c];
      }
        
      //printf("local sum : %d\n", sum1);
      for(c=0;c<NEWSITEM_NUMBER;c++){
        if(boolean[c]==1){
          MPI_Send(&newsItem[c],1,mpi_news,editor_process,10,MPI_COMM_WORLD);
        }
      }
    }
    //MPI_Barrier(MPI_COMM_WORLD);
    int global_sum[cl];
    MPI_Allreduce(sum1,global_sum,cl, MPI_INT, MPI_SUM ,MPI_COMM_WORLD);

    //changes from here

    news edit_news[global_sum[index]]; 
    
    if(my_id%EDITOR_SIZE==0)	//editor process
    {
      int a;
      for(a=0;a<global_sum[index];a++){
        news n;
        MPI_Status st;
        int id;
        MPI_Recv(&n,1,mpi_news,MPI_ANY_SOURCE,10, MPI_COMM_WORLD,&st);
        edit_news[a] = n;
        //id = st.MPI_SOURCE;
        //printf("Topic: %d Timestamp: %d, from reporter %d\n",n.topic,n.timestamp,id);
      }
      // for(a=0;a<global_sum[index];a++)
      //   printf("from editor: %d topic: %d timestamp: %d\n",my_id,edit_news[a].topic,edit_news[a].timestamp);
      
    }
   
   MPI_Barrier(MPI_COMM_WORLD);

   if(my_id%EDITOR_SIZE==0){
    int edit_boolean[global_sum[index]];
    int k;
    int c;
    for(c=0;c<global_sum[index];c++)
      edit_boolean[c] =1;
    for(k=0;k<num_procs- EDITOR_SIZE ;k+=EDITOR_SIZE){
      if(my_id == k){
        int l;
        for(l=k+EDITOR_SIZE;l<=num_procs-EDITOR_SIZE;l+=EDITOR_SIZE){             
          MPI_Send(edit_news,1,news_arr,l,10,MPI_COMM_WORLD);
        }
        int no_send = num_procs/EDITOR_SIZE - my_id/EDITOR_SIZE - 1;
        MPI_Status st; 
        int boo[global_sum[index]];       
        for(l=0;l<no_send;l++){
           MPI_Recv(boo,global_sum[index],MPI_INT,MPI_ANY_SOURCE,10,MPI_COMM_WORLD,&st);
           int d;
           for(d=0;d<global_sum[index];d++)
            edit_boolean[d] = edit_boolean[d] && boo[d];
        }
        // int a;
        // for(a=0;a<NEWSITEM_NUMBER;a++){
        //   printf(" %d : %d ",my_id, boolean[a]);
        // }
        // printf("\n");
      }
      else if(my_id>k){
        int kindex = global_sum[k/EDITOR_SIZE];
        news n[kindex];
        MPI_Status st;
        MPI_Recv(n, kindex,mpi_news,k,10,MPI_COMM_WORLD,&st);
        int a;
        // for(a=0;a<NEWSITEM_NUMBER;a++)
        //   printf("%d,%d\n",n[a].topic,n[a].timestamp);
        int boo[kindex];
        int b;
        for(a=0;a<kindex;a++)
          boo[a]=1;
        a=b=0;
        while(a<kindex && b<global_sum[index]){
          if(n[a].topic<edit_news[b].topic)
            a++;
          else if(n[a].topic>edit_news[b].topic)
            b++;
          else{
            if(n[a].timestamp<=edit_news[b].timestamp)
              boo[a]=0;
            else
              edit_boolean[b]=0;                
            a++;
            b++;
          }
        }
        MPI_Send(boo,1,rowbool,k,10,MPI_COMM_WORLD);
        

      }
      else
        break;
    }
    // for(c=0;c<NEWSITEM_NUMBER;c++)
    //   sum1 = sum1 + boolean[c];
    // //printf("local sum : %d\n", sum1);
    for(c=0;c<global_sum[index];c++){
      if(edit_boolean[c]==1){
        //MPI_Send(&newsItem[c],1,mpi_news,editor_process,10,MPI_COMM_WORLD);
        printf("Topic: %s (Timestamp: %d) by editor %d.\n",headlines[edit_news[c].topic-1],edit_news[c].timestamp,my_id);
      }
    }
   }

    MPI_Type_free(&news_arr);
    MPI_Type_free(&mpi_news);
    MPI_Type_free(&rowbool);
    MPI_Finalize();
    

}
   
