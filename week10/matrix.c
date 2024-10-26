//Program using thread to perform matrix multiplication and modify local var
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define MIN_REQ_SSIZE 81920

typedef int** matrix_t;

//Function to allocate memory for the matrix (2D int Array)
matrix_t allocate_matrix(int size){
  matrix_t matrix = (int**)malloc(size * sizeof(int*)); //allocate rows
  for(int i = 0; i < size; i++) {
    matrix[i] = (int*)malloc(size * sizeof(int)); //allocate columns for each row
  }
  return matrix;
} 

//Function to free memory of the matrix 
void free_matrix(matrix_t matrix, int size){
  for (int i =0; i < size; i++){
    free(matrix[i]); //free each rows
  }
  free(matrix); //Free the matrix pointer
}

typedef struct {
  int       id;
  int       size;
  int       start_col; //col where a thread starts 
  int       end_col; //where a thread ends 
  matrix_t  MA, MB, MC;
  int       num_threads;
} package_t; 

//Innitialize Matrix A B C
matrix_t MA,MB,MC;

//Global diagonal sum
int diag_sum = 0;

//mutex variable
pthread_mutex_t matrix_lock;
pthread_mutex_t diag_sum_lock;

/*
* Routine to multiply a row by a column and place element in 
* resulting matrix.
*/
void multiply(int id, int size, int row, int column, matrix_t MA, matrix_t MB, matrix_t MC)
{
  int sum = 0;

  for(int position = 0; position < size; position++) {
    sum = sum + ( MA[row][position] * MB[position][column] ) ;
  }


  pthread_mutex_lock(&matrix_lock); //thread lock before modify MC 

  MC[row][column] = sum;
  printf("Matrix THREAD %d: Updating C | row: %d, col: %d. Value = %d\n", id, row, column, sum);

  pthread_mutex_unlock(&matrix_lock); //unlock thread 


}


/*
 * Routine to start off a worker thread. 
 */
void *mult_worker(void *arg)
{
  //Innitialize a package p with arg values 
  package_t *p=(package_t *)arg;
  
  int col_per_thread = p->size/p->num_threads;
  int size = p->size;
  int start_col = p->start_col;
  int end_col = p->end_col;

  printf("+++++ THREAD %d created, processing columns %d to %d +++++\n", p->id , p->start_col, p->end_col);

  for(int j = start_col; j <= end_col; j++){
    for(int i = 0; i < size; i++){
      //multiply
      printf("Matrix THREAD %d: Processing A row %d, B col %d\n", p->id, i, j);
      multiply(p->id, size, i, j, p->MA, p->MB, p->MC);

      // Check if this element is on the diagonal
        if (i == j) {
            pthread_mutex_lock(&diag_sum_lock);
            diag_sum += (p->MC)[i][j];  // Safely add to the diagonal sum
            printf("Matrix THREAD %d: Adding diagonal sum. Diagonal sum = %d\n", p->id, diag_sum);
            pthread_mutex_unlock(&diag_sum_lock);
        }
    }
  }

  printf("Matrix THREAD %d: ***** COMPLETE *****\n", p->id);

  free(p);
  
  return(NULL); 
}

/*
* Main(): allocates matrix, assigns values, then 
* creates threads to process rows and columns.
*/

int main(int argc, char **argv)
{
  //Innitialize mutex variables
  pthread_mutex_init(&matrix_lock, NULL);
  pthread_mutex_init(&diag_sum_lock, NULL);


  //process input 
  if (argc != 3) {
    printf("Usage: %s <size-of-matrix-N> <number-of-thread-M>\n", argv[0]);
    exit(1);
  }

  int size, num_threads;
  size = atoi(argv[1]);
  num_threads = atoi(argv[2]);

  if(size % num_threads != 0){
    printf("Error: N must be divisible by M\n");
    exit(1);
  }

  //Allocate memory for global MA MB MC
  MA = allocate_matrix(size);
  MB = allocate_matrix(size);
  MC = allocate_matrix(size);

  /// Fill in matrix A values (all 1)
  for (int row = 0; row < size; row++) {
    for (int column = 0; column < size; column++) {
      MA[row][column] = 1;
    }
  }
  // Fill in matrix B value (row + col + 1)
  for (int row = 0; row < size; row++) {
    for (int column = 0; column < size; column++) {
      MB[row][column] = row + column + 1;
    }
  }
  //Print the 2 matrix MA MB
  printf("MAIN THREAD: The Matrix A is:\n");
  for(int row = 0; row < size; row ++) {
    for (int column = 0; column < size; column++) {
      printf("%3d ",MA[row][column]);
    }
    printf("\n");
  }

  printf("MAIN THREAD: The Matrix B is:\n");
  for(int row = 0; row < size; row ++) {
    for (int column = 0; column < size; column++) {
      printf("%3d ",MB[row][column]);
    }
    printf("\n");
  }
  
  
  pthread_t *threads;  /* threads holds the thread ids of all threads created, so that the main thread can join with them. */
  package_t *p;      /* argument list to pass to each thread. */
  //Number of threads created based on user input
  threads = (pthread_t *)malloc(num_threads*sizeof(pthread_t));
  
  /* This is not required for the program as the peers stack 
    will not be too big. It is just here to show how to check
    and set pthread attributes.
  */
  unsigned long thread_stack_size;
  pthread_attr_t *pthread_attr_p, pthread_custom_attr;

  pthread_attr_init(&pthread_custom_attr);
  // If a system adhere to POSIX Standad, this block will compile, and not otherwise
  #ifdef _POSIX_THREAD_ATTR_STACKSIZE 
    pthread_attr_getstacksize(&pthread_custom_attr, 
                              &thread_stack_size);

    if (thread_stack_size < MIN_REQ_SSIZE) {
      pthread_attr_setstacksize(&pthread_custom_attr, 
                                (long)MIN_REQ_SSIZE);
    }
  #endif

  /* Process Matrix, by row, column, Create a thread 
     to process each element in the resulting matrix*/

  int col_per_thread = size/num_threads;

  for(int k = 0; k < num_threads; k++) {
    p = (package_t *)malloc(sizeof(package_t));
    p->id = k;
    p->size = size;
    p->start_col = k*col_per_thread; //each thread processes a part of columns in B starts here
    p->end_col = p->start_col + col_per_thread - 1;// and then end here
    p->MA = MA;
    p->MB = MB;
    p->MC = MC;
    p->num_threads = num_threads;
    
    pthread_create(&threads[k], &pthread_custom_attr, mult_worker, (void *) p);

  }

  printf("=============================================\n");
  /* Synchronize on the completion of the element in each thread */
  for (int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
    printf("MAIN THREAD: child %d has joined\n", i);
  }
 
  /*Free the thread*/
  free(threads);

  printf("=============================================\n");
  /* Print results */
  printf("MAIN THREAD: The resulting matrix C is:\n");
  for(int row = 0; row < size; row ++) {
    for (int column = 0; column < size; column++) {
      printf("%3d ",MC[row][column]);
    }
    printf("\n");
  }
  /* Free matrices memory */
  free_matrix(MA, size);
  free_matrix(MB, size);
  free_matrix(MC, size);

  /* Destroy mutex */ 
  pthread_mutex_destroy(&matrix_lock);
  pthread_mutex_destroy(&diag_sum_lock);

  /* Diagonal sum */ 
  printf("=============================================\n");
  printf("MAIN THREAD: Diagonal sum is: %d\n", diag_sum);
  printf("\n -------------------------- \n"); 
  system("date; hostname; who | grep txn200004 ; ps -eaf; ls -l");
  system("who > week10who.txt");
  return 0;
} 	

