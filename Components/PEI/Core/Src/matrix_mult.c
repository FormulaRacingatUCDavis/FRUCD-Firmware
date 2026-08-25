/*
 * matrix_mult.c
 *
 *  Created on: Aug 23, 2024
 *      Author: Abhineet
 */


#include "matrix_mult.h"

void mat_init(uint8_t rows, uint8_t columns, Matrix* const matrix) {

  matrix->rows = rows;
  matrix->columns = columns;
  mat_fill_zero(matrix);

  return;
}

void mat_set(uint8_t row, uint8_t column, float element,
        	 Matrix* const matrix) {

  matrix->elements[(row - 1) * matrix->columns + (column - 1)] = element;

  return;
}

float mat_get(uint8_t row, uint8_t column, Matrix* const matrix) {
  return matrix->elements[(row - 1) * matrix->columns + (column - 1)];
}

void mat_fill_zero(Matrix* const matrix) {

  for (uint8_t i = 0; i < MAX_ELEMENTS; i++) {
    matrix->elements[i] = 0;
  }

  return;
}

void mat_multiply(Matrix* const matrixA, Matrix* const matrixB, Matrix* const matrixC) {

  // matrixA.columns != matrixB.rows should yield an error
  // BUT it should not check that in this application

  // set the result matrix's rows and columns numbers and initialize its
  // elements to zero
  mat_init(matrixA->rows, matrixB->columns, matrixC);
  mat_fill_zero(matrixC);

  for (uint8_t i = 1; i <= matrixC->rows; i++) {
    for (uint8_t j = 1; j <= matrixC->columns; j++) {
      for (uint8_t k = 1; k <= matrixA->columns; k++) {
        mat_set(i, j,
                mat_get(i, j, matrixC) +
                    mat_get(i, k, matrixA) * mat_get(k, j, matrixB),
                matrixC);
      }
    }
  }
}

void mat_scale(Matrix* const A, float k, Matrix* const result) {
  mat_init(A->rows, A->columns, result);

  for (uint8_t i = 1; i <= A->rows; i++) {
    for (uint8_t j = 1; j <= A->columns; j++) {
      float val = k * mat_get(i, j, A);
      mat_set(i, j, val, result);
    }
  }
}

// Function to transpose a 2x2 matrix
//                         ^^^
void mat_transpose(Matrix* const matrix, Matrix* const result) {
  mat_init(2, 2, result);

  float matrix_1_2 = mat_get(1, 2, matrix);
  mat_set(2, 1, matrix_1_2, result);

  float matrix_1_1 = mat_get(1, 1, matrix);
  mat_set(1, 1, matrix_1_1, result);

  float matrix_2_1 = mat_get(2, 1, matrix);
  mat_set(1, 2, matrix_2_1, result);

  float matrix_2_2 = mat_get(2, 2, matrix);
  mat_set(2, 2, matrix_2_2, result);
}

void mat_add(Matrix* const A, Matrix* const B, Matrix* const result) {
  for (uint8_t i = 1; i <= A->rows; i++) {
    for (uint8_t j = 1; j <= A->columns; j++) {
      float sum = mat_get(i, j, A) + mat_get(i, j, B);
      mat_set(i, j, sum, result);
    }
  }
}
