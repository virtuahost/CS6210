#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define DUMMY_SIZE 1
#define DUMMY_TAG 1


typedef struct treenode {
	int parent_sense;
	int parent_pointer;
  int child_pointers[2]; 
  int have_child[4]; 
  int child_not_ready[4] ; 
} treenode_t;

struct treenode **nodes = NULL;

int sense;
int nodeLen;

int ntbarrier_init(int num_processes)
{
  int i = 0, j =0;
  sense = 1;
  nodes = (struct treenode **)realloc(nodes, (num_processes) * sizeof(struct treenode *));
  nodeLen = num_processes;
  for(i = 0; i < nodeLen;i++)
  {
    nodes[i] = (struct treenode *)malloc(sizeof(struct treenode));
    nodes[i]->parent_sense = 0;
    while(j<4)
    {
      if((4*i+j+1)<nodeLen)
      {
        nodes[i]->have_child[j] =  1;
        nodes[i]->child_not_ready[j] = 1;
      }
      else
      {
        nodes[i]->have_child[j] =  0;
        nodes[i]->child_not_ready[j] = 0;
      }
      j++;
    }
    if(i!=0)
      nodes[i]->parent_pointer = i;
    else
      nodes[i]->parent_pointer = -1;
    if(2*i+1 >= nodeLen)
      nodes[i]->child_pointers[0] = -1;
    else
      nodes[i]->child_pointers[0] = 2*i+1;
    if(2*i+2 >= nodeLen)
      nodes[i]->child_pointers[1] = -1;
    else
      nodes[i]->child_pointers[1] = 2*i+2;
  }
	return 0;
}

int ntbarrier_barrier(int id)
{
  int allChildReady = 0;  int j =0;
  int index = 0;
  int child = 0;
  int waitKey = 0, readyKey = 1;
  if((id >= 0)&&(id <= nodeLen-1))
  {
    while(j<4)
    {
      // allChildReady = 0;
      // while(j<4)
      // {  
        if(nodes[id]->have_child[j] == 1)
        {          
          // printf("Child present: %i in %i for id: %i \n",nodes[id]->have_child[j], (4*id+j+1), id);
          // readyKey = nodes[id]->child_not_ready[j];
          while(readyKey != 0)
          {
            MPI_Recv(&readyKey, DUMMY_SIZE, MPI_INT, (4*id+j+1), (4*id+j+1) , MPI_COMM_WORLD, MPI_STATUS_IGNORE);   
          }
          nodes[id]->child_not_ready[j] = readyKey;
          // readyKey = 1;
          if(nodes[id]->child_not_ready[j] == 0)
          {
            j++;            
          }
        }
        else
          j++;
        // j++;
      // }
      // j = 0;
    }
    for(j = 0; j < 4; j++)
    {
      nodes[id]->child_not_ready[j] = nodes[id]->have_child[j]; 
    }
    if(id>0)
    {
      index = (int)floor((nodes[id]->parent_pointer - 1)/4);
      child = (nodes[id]->parent_pointer - 1)%4;
      // printf("Setting index: %i \n",index); 
      // printf("Child to set: %i \n",child); 
      nodes[index]->child_not_ready[child] = 0;
      readyKey = 0;
      MPI_Send(&readyKey, DUMMY_SIZE, MPI_INT, index, id, MPI_COMM_WORLD);
      index = (int)floor((nodes[id]->parent_pointer-1)/2);
      while(waitKey != sense)
      {  
        // waitKey = nodes[id]->parent_sense;  
        // printf("Setting index: %i %i \n",index,id);    
        MPI_Recv(&waitKey, DUMMY_SIZE, MPI_INT, index, id , MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }      
      nodes[id]->parent_sense = waitKey; 
    }
    // printf("Finishing off: %i",id); 
    // printf("Sending off: %i %i",nodes[id]->child_pointers[0],nodes[id]->child_pointers[1]); 
    if(nodes[id]->child_pointers[0] != -1)
    {
      nodes[nodes[id]->child_pointers[0]]->parent_sense = sense;
      waitKey = sense;
      MPI_Send(&waitKey, DUMMY_SIZE, MPI_INT, nodes[id]->child_pointers[0], nodes[id]->child_pointers[0] , MPI_COMM_WORLD);
    }
    if(nodes[id]->child_pointers[1] != -1)
    {
      nodes[nodes[id]->child_pointers[1]]->parent_sense = sense;
      waitKey = sense;
      MPI_Send(&waitKey, DUMMY_SIZE, MPI_INT, nodes[id]->child_pointers[1], nodes[id]->child_pointers[1] , MPI_COMM_WORLD);
    }
    sense = (sense == 0?1:0);
  } 
	return 0;
}

int ntbarrier_finalize()
{
  int i =0;
  for(i = 0; i < nodeLen; i++) {
    free(nodes[i]);
    i++;
  }
  free(nodes);
	return 0;
}

