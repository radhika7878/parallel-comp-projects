#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define NEWSITEM_NUMBER 5

//structure for a news piece
typedef struct news
{
  int topic;
  int timestamp;
}news;

main(int argc, char **argv)
{
  int my_id, editor_process, num_procs, ierr, i;
  //array of newsitems;
  news newsItem[NEWSITEM_NUMBER];
  char fileName[100];
  FILE *fp;

  /* Let process 0 be the editor process. */
  editor_process=0;
  my_id=1;

  //array of topics mapped to topic field of news srtuct
  char *headlines[] = {"JNU Sedition","Union Budget","Vijay Mallya", "Cultural Day","Donald Trump","2016 Presidential",
    "Apple Inc.","North Korea","ADBlock","Sunny Leone","Rahul Gandhi","PoK","Intolerance","Hezbollah Lebanon","Syrian War"};

  //derived datatypes: mpi_news-struct news, rowbool:array of integers, news_arr:array of news
  MPI_Datatype mpi_news;
  MPI_Datatype rowbool;
  MPI_Datatype news_arr;

  ierr = MPI_Init(&argc, &argv);

  /* Find out MY process ID, and how many processes were started. */

  ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
  ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

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

  //reporter process reading news from corresponding textfile
  if(my_id > editor_process)  
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
  }

  //news read,stored in structure set barrier till all processes have stored data in newsItem
  MPI_Barrier(MPI_COMM_WORLD);  

  //stores boolean values for latest news
  int boolean[NEWSITEM_NUMBER];
  int sum1=0;
  
  //reporters communicating with each other and making boolean array
  if(my_id>editor_process){
    int k;
    int c;
    for(c=0;c<NEWSITEM_NUMBER;c++)
      boolean[c] =1;
    for(k=1;k<num_procs-1;k++){
      if(my_id == k){
        int l;
        for(l=k+1;l<num_procs;l++){             
          MPI_Send(newsItem,1,news_arr,l,10,MPI_COMM_WORLD);
        }
        int no_send = num_procs-my_id-1;
        MPI_Status st; 
        int boo[NEWSITEM_NUMBER];       
        for(l=0;l<no_send;l++){
           MPI_Recv(boo,NEWSITEM_NUMBER,MPI_INT,MPI_ANY_SOURCE,10,MPI_COMM_WORLD,&st);
           int d;
           for(d=0;d<NEWSITEM_NUMBER;d++)
            boolean[d] = boolean[d] && boo[d];
        }
        int a;
      }
      else if(my_id>k){
        news n[NEWSITEM_NUMBER];
        MPI_Status st;
        MPI_Recv(n, NEWSITEM_NUMBER,mpi_news,k,10,MPI_COMM_WORLD,&st);
        int a;
        int boo[NEWSITEM_NUMBER];
        int b;
        for(a=0;a<NEWSITEM_NUMBER;a++)
          boo[a]=1;
        a=b=0;
        while(a<NEWSITEM_NUMBER && b<NEWSITEM_NUMBER){
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
      }
      else
        break;
    }

    //counting number of recent valid news
    for(c=0;c<NEWSITEM_NUMBER;c++)
      sum1 = sum1 + boolean[c];
    //printf("local sum : %d\n", sum1);

    //sending recent valid news to the editor
    for(c=0;c<NEWSITEM_NUMBER;c++){
      if(boolean[c]==1){
        MPI_Send(&newsItem[c],1,mpi_news,0,10,MPI_COMM_WORLD);
      }
    }
  }

  //calculating total number of recent valid news
  int global_sum;
  MPI_Reduce(&sum1,&global_sum,1, MPI_INT, MPI_SUM, 0 ,MPI_COMM_WORLD);

  //editor recieving all recent valid news and printing it
  if(my_id==0){
    int a;
    for(a=0;a<global_sum;a++){
      news n;
      MPI_Status st;
      int id;
      MPI_Recv(&n,1,mpi_news,MPI_ANY_SOURCE,10, MPI_COMM_WORLD,&st);
      id = st.MPI_SOURCE;
      printf("Topic: %s (Timestamp: %d) from reporter %d.\n",headlines[n.topic-1],n.timestamp,id);
    }
    
  }

  //freeing all derived datatype
  MPI_Type_free(&news_arr);
  MPI_Type_free(&mpi_news);
  MPI_Type_free(&rowbool);

  MPI_Finalize();
}
