/*
 * matrix_mult.h
 *
 *  Created on: Aug 23, 2024
 *      Author: Abhineet
 */

#ifndef INC_MATRIX_MULT_H_
#define INC_MATRIX_MULT_H_

#include <stdint.h>

// Constants
#define MAX_ELEMENTS 4

// Type(s)

typedef struct MatrixStruct {
  uint8_t rows;
  uint8_t columns;
  float elements[MAX_ELEMENTS];
} Matrix;

// Function declarations

void mat_init(uint8_t rows, uint8_t columns, Matrix* const matrix);
void mat_set(uint8_t row, uint8_t column, float element,
             Matrix* const matrix);
float mat_get(uint8_t row, uint8_t column, Matrix* const matrix);
void mat_fill_zero(Matrix* const matrix);
void mat_multiply(Matrix* const matrixA, Matrix* const matrixB, Matrix* const matrixC);
void mat_add(Matrix* const A, Matrix* const B, Matrix* const result);
void mat_scale(Matrix* const A, float k, Matrix* const result);
void mat_transpose(Matrix* const matrix,
                   Matrix* const result); // Function to transpose a 2x2 matrix
                                    //                         ^^^

#endif /* INC_MATRIX_MULT_H_ */
