#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mpi.h"

#define DUMMY_SIZE 1
#define DUMMY_TAG 1

typedef enum {
  winner,
  loser,
  bye,
  champion,
  dropout
} status;

typedef struct treenode {
  status role;  
  int opponent[2];
  int flag;
} treenode_t;

struct treenode **nodes = NULL;

int sense;
int nodeLen;
int colLen;

int tbarrier_init(int num_processes)
{
  int i = 0, j =0;
  int p, t, e, p1;
  sense = 1;
  nodes = (struct treenode **)realloc(nodes, (num_processes) * sizeof(struct treenode *));
  nodeLen = num_processes;
  colLen = (int)ceil(log2(num_processes))+1;
  // printf("colLen: %i %i \n",colLen, num_processes);
  for(i = 0; i < num_processes;i++)
  {
    nodes[i] = (struct treenode *)malloc(colLen * sizeof(struct treenode));
  }
  for(i = 0; i < nodeLen;i++)
  {
    for(j = 0; j < colLen;j++)
    {       
      p = (int)(pow(2,j));
      t = (int)(pow(2,(j-1)));
      nodes[i][j].flag = 0;
      nodes[i][j].role = 500;
      if(j > 0)
      { 
        if((i%p) == 0)
        {
          if((i == 0) && (p >= nodeLen))
          {
            nodes[i][j].role = champion;
          }
          else if(((i+t)<nodeLen) && (p<nodeLen))
          {
            // printf("Setting node data: %i : %i\n", i, t);
            nodes[i][j].role = winner;
          }
          else if((i+t)>=nodeLen)
          {
            nodes[i][j].role = bye;
          }
        }
        else if((i%p) == t)
        {
          nodes[i][j].role = loser;
        }
      }
      else if(j == 0)
      {
        nodes[i][j].role = dropout;
      }
      if(nodes[i][j].role == loser)
      {
        nodes[i][j].opponent[0] = i-t;
        nodes[i][j].opponent[1] = j;
      }
      else if((nodes[i][j].role == winner)||(nodes[i][j].role == champion))
      {
        // printf("Role: %i :%i : %i\n", nodes[i][j].role, i, t);
        nodes[i][j].opponent[0] = i+t;
        nodes[i][j].opponent[1] = j;
      }
      else
      {
        nodes[i][j].opponent[0] = -1;
        nodes[i][j].opponent[1] = -1;
      }
      // printf("Opponent: %i for id: %i round: %i \n", i,nodes[i][j].opponent[0],j);
    }
  }
  return 0;
}

int tbarrier_barrier(int id)
{
  int i = 0, j =0;
  int waitKey = 0;
  int found = 0;
  int temp = 0;

  //arrival
  // printf("Arrival: %i \n",colLen);
  for(j = 1; j < colLen;j++)
  {
    // if(id == 8)
    //   printf("Role: %i \n",nodes[id][j].role);
    waitKey = 0;
    temp = (int)pow(2,(j-1));
    // printf("Temp: %i %i \n",temp, j);
    switch(nodes[id][j].role)
    {
      case loser:
          nodes[nodes[id][j].opponent[0]][nodes[id][j].opponent[1]].flag = sense;
          waitKey = sense;
          // if(id == 8)
          //   printf("Loser: %i %i \n",nodes[id][j].opponent[0],id);
          MPI_Send(&waitKey, DUMMY_SIZE, MPI_INT, nodes[id][j].opponent[0], id, MPI_COMM_WORLD);
          waitKey = 0;
          while(waitKey != sense)
          {
            MPI_Recv(&waitKey, DUMMY_SIZE, MPI_INT, (id-temp), (id-temp) , MPI_COMM_WORLD, MPI_STATUS_IGNORE);   
          }
          waitKey = 0;
          found = 1;
          break;
      case winner:
          // if(id == 8)
          //   printf("Winner: %i \n",(id+temp));
          while(waitKey != sense)
          {
            MPI_Recv(&waitKey, DUMMY_SIZE, MPI_INT, (id+temp), (id+temp), MPI_COMM_WORLD, MPI_STATUS_IGNORE);   
          }
          waitKey = 0;
          break;
      case bye:
          break;
      case champion:
          // printf("Champion: %i \n",(id+temp));
          while(waitKey != sense)
          {
            MPI_Recv(&waitKey, DUMMY_SIZE, MPI_INT, (id+temp), (id+temp) , MPI_COMM_WORLD, MPI_STATUS_IGNORE);   
          }
          waitKey = 0;
          nodes[nodes[id][j].opponent[0]][nodes[id][j].opponent[1]].flag = sense;
          waitKey = sense;
          MPI_Send(&waitKey, DUMMY_SIZE, MPI_INT, nodes[id][j].opponent[0], id, MPI_COMM_WORLD);
          found = 1;
          break;
      default:
          break;
    }
    if(found == 1)
    {
      found = 0;
      break;
    }
  }

  //Wakeup
  // printf("Wakeup: %i \n",colLen);
  for(j = colLen-1; j >= 0;j--)
  {
    switch(nodes[id][j].role)
    {
      case loser:
          break;
      case winner:
          // printf("Role: %i \n",nodes[id][j].opponent[0]);
          // if((nodes[id][j].opponent[0] >= 0)&&(nodes[id][j].opponent[0]<nodeLen))
          // {            
            nodes[nodes[id][j].opponent[0]][nodes[id][j].opponent[1]].flag = sense;
            waitKey = sense;
            MPI_Send(&waitKey, DUMMY_SIZE, MPI_INT, nodes[id][j].opponent[0], id, MPI_COMM_WORLD);
          // } 
          break;
      case bye:
          break;
      case champion:
          break;
      case dropout:
          found = 1;
          break;
      default:
          break;
    }
    if(found == 1)
    {
      found = 0;
      break;
    }
  }
  sense = (sense == 0?1:0);
  return 0;
}

int tbarrier_finalize()
{
  int i =0;
  for(i = 0; i < nodeLen; i++) {
    free(nodes[i]);
    i++;
  }
  free(nodes);
  return 0;
}

int main(int argc, char** argv)
{
  int num_processes, id, i;
  double start, end, minStart = 999999999999, maxEnd = 0;
  double *allStart, *allEnd;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &num_processes);  
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  for (i = 0; i < 4; i++) {
    start = MPI_Wtime();
    if(id == 0)
    {
      printf("Started time for barrier %i for node count %i is %f \n",i, num_processes, start);
    }
    tbarrier_init(num_processes);
    printf("Started process id: %i for node count %i for barrier %i \n",id, num_processes, i);

    tbarrier_barrier(id);


    end = MPI_Wtime();
    if(id == 0)
    {
      printf("Finished time for barrier %i for node count %i is %f \n", i, num_processes, end);
      printf("Time taken by barrier: %i for node count %i is %f \n",i, num_processes,(end - start));
    }
  }
  allStart = (double *)malloc(num_processes*sizeof(double));
  allEnd = (double *)malloc(num_processes*sizeof(double));

  MPI_Gather(&start, 1, MPI_DOUBLE, allStart, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Gather(&end, 1, MPI_DOUBLE, allEnd, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  if(id == 0) {
    for(i = 0; i < num_processes; i++) {
      if (*(allStart + i) < minStart)
        minStart = *(allStart + i);
      if (*(allEnd + i) > maxEnd)
        maxEnd = *(allEnd + i);      
    }
    printf("Total time = %f\n", maxEnd - minStart);
  }

  MPI_Finalize();
  tbarrier_finalize(); 

  return(0);
}