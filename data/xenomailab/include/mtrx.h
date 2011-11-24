/*
 * Xenomai Lab
 * Copyright (C) 2011 Jorge Azevedo
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MTRX2_H
#define	MTRX2_H

#define Rmax 4
#define Cmax 4

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct {
    unsigned char rows;
    unsigned char columns;
    double matrix[Rmax][Cmax];
}Matrix;

Matrix new_matrix(char *str);
int new_matrix_safe(Matrix*, char *str);
Matrix empty_matrix(int rows,int col);
Matrix matrix_mul(Matrix *M1, Matrix *M2);
Matrix matrix_mul_double(Matrix *M1, double num);
Matrix matrix_tran(Matrix *Msrc);
double matrix_det(Matrix *Msrc);
Matrix matrix_inv(Matrix *Msrc);
Matrix matrix_eye(unsigned char dim, double value);
Matrix matrix_sum(Matrix *M1, Matrix *M2);
Matrix matrix_sub(Matrix *M1, Matrix *M2);
void matrix_string(Matrix *M1, char* str);
void matrix_print(Matrix * M1);


#ifdef	__cplusplus
}
#endif

#endif	/* MTRX_H */

