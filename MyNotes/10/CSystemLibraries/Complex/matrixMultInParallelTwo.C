#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <assert.h>

// This code illustrates how to do matrix multiplication in parallel
// In this case, we are chaining several mults together and using
// Just two processors to see if there is a significant difference
// We can time it just to see if there is a speed-up or not!

class Matrix {
private:
  double** a;
  int nC;
  int nR;

public:
  Matrix(int _nC, int _nR) : nC(_nC), nR(_nR) {
    a = new double*[nR];
    for (int r = 0; r < nR; r++) a[r] = new double[nC];
  }

  ~Matrix() { 
    for (int r = 0; r < nR; r++) delete[] a[r];
    delete[] a; 
  }
  
  int getNumRows() { return nR; }
  int getNumCols() { return nC; }

  // Note neither of these performs proper bounds checking
  // which would be essential in a well-written C++ program!!!
  double getValue(int r, int c) { return a[r][c]; }
  void setValue(int r, int c, double value) { a[r][c] = value; }

  /***
   * fill the matrix with random values from min to max
   ***/
  void fillMatrix(double min, double max);

  /***
   * Print out the matrix
   ***/
  void printMatrix();

  /***
   * Multiply the current matrix by the matrix other
   *    Returns a new allocated matrix (REFERENCE IS GIVEN to caller)
   ***/
  Matrix* multMatrix(Matrix& other) { return multMatrix(&other); } // Wrapper for next method
  Matrix* multMatrix(Matrix* other);

  // Let this function have full access to the class
  friend Matrix* multInParallel(Matrix** arr, int numCopies);
};

/***
 * fill the matrix with random values from min to max
 ***/
void Matrix::fillMatrix(double min, double max) {
  int r, c;
  for (r = 0; r < nR; r++) {
    for (c = 0; c < nC; c++) {
      double zeroToOne = random() / (double) RAND_MAX;   // A value from [0,1)
      double value = zeroToOne * (max - min) + min;
      a[r][c] = value;
    }
  }
}

void Matrix::printMatrix() {
  // The following two IO Manipulators (for C++) apply to all output (until changed again)
  std::fixed(std::cout);  // Make the floating points print out in fixed value (with decimals)
  std::cout << std::setprecision(4);  // The precision level
  int r, c;
  for (r = 0; r < nR; r++) {
    for (c = 0; c < nC; c++) {
      std::cout << std::setw(10) << a[r][c] << " ";  // setw manipulator is only for next value!
    } 
    std::cout << std::endl;
  }
}

/***
 * Multiply the current matrix by the matrix other
 *    Returns a new allocated matrix (REFERENCE IS GIVEN to caller)
 ***/
Matrix* Matrix::multMatrix(Matrix* other) {
  assert(this->nC == other->nR);  // Number of cols in this must match number of rows in other!
  Matrix* answer = new Matrix(this->nR, other->nC);

  int r, c, k;
  for (r = 0; r < answer->nR; r++) {
    for (c = 0; c < answer->nC; c++) {
      double sum = this->a[r][0] * other->a[0][c];
      for (k = 1; k < this->nC; k++) {
	sum += this->a[r][k] * other->a[k][c];
      }
      answer->a[r][c] = sum;
    }
  }
  return answer;
}

Matrix* multRange(Matrix** arr, int start, int end);
Matrix* multInParallel(Matrix** arr, int numCopies);

int main() {
  srandom(time(NULL));

  // First let us create the matrix
  int dimension = 100;
  int numCopies = 2000;
  int m;

  Matrix** arr = new Matrix*[numCopies];
  for (m = 0; m < numCopies; m++) {
    arr[m] = new Matrix(dimension, dimension);
    arr[m]->fillMatrix(-1.0, 1.0);
  }

  time_t start, stop;

  start = time(NULL);  // Start the clock
  Matrix* c = multRange(arr, 0, numCopies);
  stop = time(NULL);   // Stop it
  std::cout << "Normal Multiplication took " << (stop-start) << " seconds." << std::endl;

  if (dimension <= 10) {
    std::cout << "Here is the matrix multiplied normally:" << std::endl;
    c->printMatrix();
  }

  // And now in parallel
  start = time(NULL);
  Matrix* d = multInParallel(arr, numCopies);
  stop = time(NULL);
  std::cout << "Parallel Multiplication took " << (stop-start) << " seconds." << std::endl;
  if (dimension <= 10) {
    std::cout << "Here is the matrix multiplied in parallel:" << std::endl;
    d->printMatrix();
  }
}


/***
 * Multiply the matrices in the given arr from start (inclusive) to end (exclusive)
 ***/
Matrix* multRange(Matrix** arr, int start, int end) {
  if (start+1 >= end) {
    // Just return the matrix itself
    return arr[start];
  }

  Matrix* c;
  c = arr[start]->multMatrix(arr[start+1]);  // Multiply the first two together
  for (int m = start+2; m < end; m++) {
    Matrix* d = c->multMatrix(arr[m]);
    delete c;
    c = d;
  }
  return c;
}

/***
 * Multiply the matrices in parallel
 * We will split the task into two parts
 * Parent multiplies 0-numCopies/2-1 Yielding A
 *   Child multiplies numCopies/2 - numCopies-1 Yielding B
 *   Parent then multiplies A and B
 ***/
Matrix* multInParallel(Matrix** arr, int numCopies) {
  // Create a pipe for communication
  int comm[2];
  if (pipe(comm) == -1) {
    char* errorMessage = strerror(errno);
    std::cerr << "Error creating pipe: " << errorMessage << std::endl;
    exit(1);
  }

  // Compute the range (the mid value)
  int mid = numCopies/2;

  pid_t cid = fork();  // Create two processes!
  if (cid == -1) {
    // Error
    char* errorMessage = strerror(errno);
    std::cerr << "Error creating new process: " << errorMessage << std::endl;
    exit(1);
  }

  if (cid == 0) {
    // We are the child
    Matrix* b = multRange(arr, mid, numCopies);

    // Send matrix to parent, we'll do it one row at a time
    // Which should be faster than each entry
    size_t count = b->nC * sizeof(double);  // Amount of bytes per row
    for (int r = 0; r < b->nR; r++) {
      write(comm[1], b->a[r], count);
    }
    
    // Don't forget to exit after
    exit(1);
  } else {
    // We are the parent
    Matrix* a = multRange(arr, 0, mid);

    // Now read in the matrix from child 
    //   The dimensions of this matrix must be the following
    //   In our example though they are all square matrices so it is actually easier
    //   Just the same as the dimensions for any of the matrices
    Matrix* b = new Matrix(arr[mid]->nR, arr[numCopies-1]->nC);

    // Read in one row at a time
    size_t count = b->nC * sizeof(double);
    for (int r = 0; r < b->nR; r++) {
      read(comm[0], b->a[r], count);
    }

    // Now multiply the two together
    Matrix* c = a->multMatrix(b);
    delete a;
    delete b;
    return c;
  }
}
