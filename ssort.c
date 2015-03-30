/* Parallel sample sort
 */
#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
#include <stdlib.h>


static int compare(const void *a, const void *b)
{
  int *da = (int *)a;
  int *db = (int *)b;

  if (*da > *db)
    return 1;
  else if (*da < *db)
    return -1;
  else
    return 0;
}

int main( int argc, char *argv[])
{
  int rank, mpisize;
  int i, j, N, s, tag=0;
  int *vec;
  MPI_Status status;
  double T1,T2;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &mpisize);


  


  /* Number of random numbers per processor (this should be increased
   * for actual tests or could be made a passed in through the command line */
  N = 1000;
  int *splitters;
  splitters = (int *)malloc((mpisize+1)*sizeof(int));
  vec = calloc(N, sizeof(int));
  /* seed random number generator differently on every core */
  srand48((unsigned int) rank);

  /* fill vector with random integers */
  
  for (i = 0; i < N; ++i) {
    vec[i] = lrand48();
  }

  /* sort locally */
  // qsort(vec, N, sizeof(int), compare);
  
  MPI_Barrier(MPI_COMM_WORLD);
  T1=MPI_Wtime();
  /* randomly sample s entries from vector or select local splitters,
   * i.e., every N/P-th entry of the sorted vector */
  s =(int)N/10;
  /* every processor communicates the selected entries
   * to the root processor */
  if (rank != 0){
    MPI_Send(vec, s, MPI_INT, 0, tag, MPI_COMM_WORLD);
  }
  if(rank ==0){
    int *temp;
    temp = (int *)malloc(mpisize*s*sizeof(int));
    for (i=0; i<s; i++){
      temp[i]=vec[i];
    }
    for (i=1; i<mpisize; i++){
      MPI_Recv(&temp[i*s], s, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
    }

    /* root processor does a sort, determinates splitters and broadcasts them */
    qsort(temp, mpisize*s, sizeof(int), compare);
    for (i=0; i<mpisize-1; i++){
      splitters[i+1]=temp[(i+1)*s];
    }
   
    free(temp);
  }
  MPI_Bcast(&splitters[1], mpisize-1, MPI_INT, 0, MPI_COMM_WORLD);
  /* every processor uses the obtained splitters to decide to send
   * which integers to whom */
  qsort(vec, N, sizeof(int), compare);
  splitters[0]=vec[0];
  splitters[mpisize]=vec[N-1];
  

  int *index=(int *)malloc((mpisize+1)*sizeof(int));
  int *chunkLength=(int *)malloc(mpisize*sizeof(int));
  int *recvLength=(int *)malloc(mpisize*sizeof(int));
  int newLength=0;//length of new vec
  int *newVec;
  index[0]=0;
  index[mpisize]=N;
  //index[i] is the starting address for chunk 
  //sending to rank i, chunk length is index[i+1]-index[i]
  for(i=0;i<mpisize;i++){
    chunkLength[i]=0;
  }

  for (i=0;i<N;i++){
    for (j=1;j<mpisize-1;j++){
      if (vec[i]>splitters[j] && vec[i]<=splitters[j+1]){
	chunkLength[j]++;
      }
    }
    if(vec[i]<=splitters[1]){
      chunkLength[0]++;
    }
    if(vec[i]>splitters[mpisize-1]){
      chunkLength[mpisize-1]++;
    }
  }
  for (i=1;i<mpisize;i++){
    index[i]=index[i-1]+chunkLength[i-1];
  }
  
  MPI_Alltoall(chunkLength,1,MPI_INT,recvLength,1,MPI_INT,MPI_COMM_WORLD);
  for(i=0;i<mpisize;i++){
    newLength = newLength + recvLength[i];
  }
  newVec=(int*)malloc(newLength*sizeof(int));
  //send chunk of data
  for (i=0;i<mpisize;i++){
    if(rank != i){
      MPI_Send(&vec[index[i]],chunkLength[i],MPI_INT,i,tag,MPI_COMM_WORLD);
    }
  }
  //receive chunk of data
  int temp=0;
  for (i=0;i<mpisize;i++){
    if(rank != i){
      MPI_Recv(&newVec[temp],recvLength[i],MPI_INT,i,tag,MPI_COMM_WORLD,&status);
      temp=temp+recvLength[i];
    }
  }
  //self chunk copy from vec to newVec
  for (i=0;i<chunkLength[rank];i++){
    newVec[temp+i]=vec[index[rank]+i];
  }

  /* send and receive: either you use MPI_AlltoallV, or
   * (and that might be easier), use an MPI_Alltoall to share
   * with every processor how many integers it should expect,
   * and then use MPI_Send and MPI_Recv to exchange the data */

  /* local sort */
  qsort(newVec,newLength,sizeof(int),compare);
  /* every processor writes its result to a file */
  MPI_Barrier(MPI_COMM_WORLD);
  T2=MPI_Wtime();
  FILE* fd = NULL;
  char filename[256];
  snprintf(filename, 256, "output%02d.txt", rank);
  fd = fopen(filename,"w+");

  if(NULL == fd){
    printf("Error opening file \n");
    return 1;
  }
  
  fprintf(fd,"rank %d, length of new vector: %d\n",rank,newLength);
  fprintf(fd,"Time spent:%lf seconds\n",T2-T1);
  for(i=0;i<newLength;i++){
    fprintf(fd,"%d\n",newVec[i]);
  }
  fclose(fd);

  free(vec);
  free(splitters);
  free(index);
  free(chunkLength);
  free(recvLength);
  free(newVec);
  MPI_Finalize();
  return 0;
}
