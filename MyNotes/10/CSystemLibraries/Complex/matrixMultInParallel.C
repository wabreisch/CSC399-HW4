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
// We can time it just to see if there is a speed-up or not!
// Highly doubtful.  Too many processes are spawned incurring MASSIVE OVERHEAD!

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

  /***
   * Multiply the current matrix but in parallel
   *    Just to illustrate how it could be done
   ***/
  Matrix* multMatrixParallel(Matrix& other) { return multMatrixParallel(&other); }
  Matrix* multMatrixParallel(Matrix* other);
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

/***
 * Multiply the current matrix by the matrix other
 *    Returns a new allocated matrix (REFERENCE IS GIVEN to caller)
 *    This version does the multiplication in parallel (using forks)
 *    Of course, with only a few processors this version has too much
 *    overhead to be truly effective (I would guess)  Test to see...
 ***/
Matrix* Matrix::multMatrixParallel(Matrix* other) {
  assert(this->nC == other->nR);  // Number of cols in this must match number of rows in other!
  Matrix* answer = new Matrix(this->nR, other->nC);

  int comm[this->nR][other->nC][2];  // Pipes for each entry to multiply
  int r, c, k;
  for (r = 0; r < answer->nR; r++) {
    for (c = 0; c < answer->nC; c++) {
      // Create a process to perform the task for this row/column
      // First, we need a pipe for communication (the costly part really)
      if (pipe(comm[r][c]) == -1) {
	// Error creating pipe
	char* errorMessage = strerror(errno);
        std::cerr << "At row/col (" << r << ":" << c << "): ";
	std::cerr << "Error creating pipe, aborting: " << errorMessage << std::endl;
	exit(1);
      }
      pid_t cid = fork();
      if (cid == -1) {
	// Error forking new process
	char* errorMessage = strerror(errno);
	std::cerr << "Error forking new process, aborting: " << errorMessage << std::endl;
	exit(1);
      }
      if (cid == 0) {
	// Child process, do the child work
	double sum = this->a[r][0] * other->a[0][c];
	for (k = 1; k < this->nC; k++) {
	  sum += this->a[r][k] * other->a[k][c];
	}
	write(comm[r][c][1], &sum, sizeof(double));
	exit(0); // Dont forget this!!! (See what happens if you dont do this statement!)
      }
    }
  }

  // Now we have all processes working, let us read each one back
  // Could do a wait and check for each process as they complete but here we need them
  // all anyway
  for (r = 0; r < answer->nR; r++) {
    for (c = 0; c < answer->nC; c++) {
      double sum;
      read(comm[r][c][0], &sum, sizeof(double));
      answer->a[r][c] = sum;
      close(comm[r][c][0]);
      close(comm[r][c][1]);
    }
  }
  return answer;
}


int main() {
  // First let us create the matrix
  int dimension = 100;

  Matrix a(dimension, dimension);
  Matrix b(dimension, dimension);
  
  srandom(time(NULL));

  // Fill the array with values
  a.fillMatrix(-10.0, 10.0);
  b.fillMatrix(-10.0, 10.0);
  
  // Now let us multiply normally
  Matrix* c = a.multMatrix(b);
  if (dimension <= 10) {
    std::cout << "Here is the matrix multiplied normally:" << std::endl;
    c->printMatrix();
  }
  

  // and in parallel
  Matrix* d = a.multMatrixParallel(b);
  if (dimension <= 10) {
    std::cout << "\n\nHere is the matrix multiplied in parallel:" << std::endl;
    d->printMatrix();
  }
}
