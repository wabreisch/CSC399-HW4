/***
 * Threading the Sum
 * Author: Christian Duncan
 * For: CSC222: Systems Programming Example
 *
 * This program uses multiple threads to add up several values -
 *  This is more of a warning about resource sharing again.
 *  So the sum might not be correct!!!  (But the error might be hard to replicate!)
 ***/
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void fillArray(int* a, int n, int min, int max);
void printArray(int* a, int n);
void* add(void* val);

int size = 10;
int *a;
int sumB = 0;

int main(int argc, char **argv) {
  if (argc > 1) {
    size = atoi(argv[1]);
  }

  // Create array
  a = (int*) malloc(size * sizeof(int));
  srandom(time(NULL));

  // Fill array with random values
  fillArray(a, size, -10, 10);
  if (size <= 100) printArray(a, size);

  // Compute sum the old-fashioned way (for comparison)
  int sumA = 0;
  int i;
  for (i = 0; i < size; i++) {
    sumA += a[i];
  }
  printf("Sum (via the boring method) is: %d\n", sumA);

  // Now we shall sum it up using a thread per entry!  (Overkill!)
  //  But imagine that each entry was the result of another complex calculation!
  pthread_t thread[size];
  sumB = 0;
  for (i = 0; i < size; i++) {
    int error = pthread_create(thread+i, NULL, add, (void*) i);
    if (error != 0) {
      // An error occurred
      printf("Error occurred creating thread %d: %s\n", i, strerror(error));
      // Keep trying for the other threads anyway!
    }
  }

  // Now sit back and wait for all the child threads to finish their job
  for (i = 0; i < size; i++) {
    // Wait for thread i to finish (ignore the value returned)
    int error = pthread_join(thread[i], NULL);
    if (error != 0) {
      // An error occurred
      printf("Error occurred creating thread %d: %s\n", i, strerror(error));
      // Keep trying for the other threads anyway!
    }
  }

  printf("Sum (via the threaded method) is: %d\n", sumB);
  return 0;
}

/***
 * Add the current value to the sumB
 ***/
void* add(void* val) {
  // Cast the val back to an int
  int v = (int) val;
  int tempSum = sumB;
  tempSum = tempSum + a[v];
  int i, j = 0;
  for (i = 0; i < 10000; i++) j=j*3;
  sumB = tempSum;
  // sumB += a[v];
  pthread_exit(NULL);
  return NULL;
}

/***
 * Fill the array of N integers with random values between min (inclusive) and max (exclusive)
 ***/
void fillArray(int* a, int n, int min, int max) {
  int i;
  int range = max - min;
  for (i = 0; i < n; i++) {
    // Not the best way to get uniform distribution but close enough for us
    a[i] = random() % range + min;
  }
}

void printArray(int* a, int n) {
  int i;
  for (i = 0; i < n; i++) {
    printf("%5d ", a[i]);
    if (i % 10 == 9) printf("\n");  // Print a return after every 10th entry (count starts at 0)
  }
  if (i % 10 != 0) printf("\n"); // Print a final return if not done before
}
