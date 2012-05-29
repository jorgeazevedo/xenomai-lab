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

#include "mtrx.h"

///This function is not part of the public API
int strlen(char* str);

/**
 * Initializes M1 to matrix described in string.
 * Returns 0 on sucess, otherwise on failure.
 */

int new_matrix_safe(Matrix* M1,char *str) {
	Matrix aux = empty_matrix(RMAX, CMAX);
	int j=0,k=0,n=0;

	if(matrix_is_invalid(str))
		RETERROR("Matrix\n%s\nhas a syntax error!\n",str);

	while(*str!=']' && *str!='\0') {
	
		//if not a number, travel to next number
		if((*str<'0'||*str>'9') && (*str!='-')){
			if(*str==';')
				j++,k=0;

			str++;
		} else {
			//stop! we found a number.

			//Are we out of bounds?
			if((k>=CMAX)||(j>=RMAX))
				RETERROR("Matrix %s exceedes max dim %dx%d!\n",str,CMAX,RMAX);
		
			//store it
			aux.matrix[j][k] = atof(str);
			k++,n++;

			//skip number
			while((*str>='0'&&*str<='9') || (*str=='-') || (*str == '.') )
				str++;
		}
	}

	if((j+1)*k != n)
		RETERROR("Matrix %s dimension mismatch!\n",str);

	aux.rows=j+1;
	aux.columns=k;

	*M1=aux;

	return 0;
}

/**
 * Returns a Matrix initialized to the str description.
 *
 * This is a more direct wrapper for new_matrix_safe().
 * Supported formats:
 *       [1 0;0 1]
 *       [1,0;0,1]
 */

Matrix new_matrix(char *str){
	Matrix aux = empty_matrix(RMAX, CMAX);

	if(new_matrix_safe(&aux,str))
		ERROR("The given string is invalid %s",str);

	return aux;
}

/**
 * Checks if str does not conform to Matrix syntax
 * Rules: '[' is first non-whitespace character, ']' is last
 *        Only contains the following list of chars between brackets
 *        * 0-9
 *        * -
 *        * .
 *        * ,
 *        * whitespace
 *        * ;
 */
int matrix_is_invalid(char *str){
	int size=0;
	char* end;
	
	size = strlen(str);
	
	if(!size)
		RETERROR("Size is %d!\n",size);

	end = str + size -1;

	while(*str==' ')
		str++;
	while(*end==' ')
		end--;

	if(*end != ']')
		RETERROR("No terminating ]!\n");
	if(*str != '[')
		RETERROR("No opening [!\n");

	str++;
	while(str < end){
		if(	(*str>='0'&&*str<='9') || (*str == '-') ||
			(*str == '.') || (*str == ',') ||
			(*str == ' ') || (*str == ';') ){
			str++;
		} else {
			RETERROR("Invalid char: %c\n",*str);
		}
	}

	return 0;
}

Matrix empty_matrix(int rows, int col) {
    Matrix aux;
    int i, j;

    for (i = 0; i < RMAX; i++) {
        for (j = 0; j < CMAX; j++) {
            aux.matrix[i][j] = 0;
        }
    }
    aux.rows = rows;
    aux.columns = col;

    return aux;
}

Matrix matrix_eye(unsigned char dim, double value) {
    Matrix Mdest = empty_matrix(dim, dim);
    Mdest.rows = dim;
    Mdest.columns = dim;
    int i;
    for (i = 0; i < dim; i++) {
        Mdest.matrix[i][i] = value;
    }
    return Mdest;
}

/**
 * Writes M1 to str.
 *
 */
void matrix_string(Matrix *M1, char* str){
	int j,k;

	*str='[';
	str++;
	
	//for every row
	for(j=0;j<M1->rows;j++){
		//print every number in the row except last one
		for(k=0;k<M1->columns-1;k++){
			//print number
			sprintf(str,"%12.6f",M1->matrix[j][k]);
			str+=strlen(str);
			//print space
			*str=' ';
			str++;
		}

		//print last number in the row and ';' if
		//we still have rows to go, otherwise ']\0'
		sprintf(str,"%12.6f",M1->matrix[j][k]);
		str+=strlen(str);
		
		//if this isn't the last row
		if(j<M1->rows-1){
			*str=';';
			str++;
		} else {
			//close brackets and close string
			*str=']';
			str++;
			*str='\0';
		}
	}
	return;
}

/**
 * Prints M1 to stdout. Uses matrix_string().
 */

void matrix_print(Matrix *M1){
        char buf[500];

        matrix_string(M1,buf);
        printf("%s\n",buf);

        return;
}

//TODO: matrix_mul_safe(Matrix *M1, Matrix *M2, Matrix *res)
Matrix matrix_mul(Matrix *M1, Matrix *M2) {
    if (M1->columns != M2->rows) {
        printf("matrix_mul: Dimensions mismatch");
        exit(1);
    }
    // Mdest=M1*M2
    int i, j, k;
    // Inicializar Mdest
    Matrix Mdest = empty_matrix(M1->rows, M2->columns);
    //alterar para evitar desperdicios de memoria, criar flag initialized
    for (i = 0; i < (M1->rows); i++) {
        for (j = 0; j < (M2->columns); j++) {
            for (k = 0; k < (M1->columns); k++) {
                Mdest.matrix[i][j] += M1->matrix[i][k] * M2->matrix[k][j];
            }
        }
    }
    return Mdest;
}

Matrix matrix_mul_double(Matrix *M1, double num) {
	int i,j;
    	Matrix Mdest = empty_matrix(M1->rows, M1->columns);

	for(i=0;i < M1->rows;i++){
		for(j=0;j < M1->columns;j++)
			Mdest.matrix[i][i]=M1->matrix[i][j]*num;
	}

	return Mdest;
}

Matrix matrix_tran(Matrix *Msrc) {
    //Mdest=Msrc'
    Matrix Mdest = empty_matrix(Msrc->columns, Msrc->rows); //verificar tambem caso ja esteja alocada a variavel
    int i, j;

    for (i = 0; i < (Mdest.rows); i++) {
        for (j = 0; j < (Mdest.columns); j++) {
            Mdest.matrix[i][j] = Msrc->matrix[j][i];
        }
    }
    return Mdest;
}

double matrix_det(Matrix *Msrc) {

    //Calculo do determinante de uma matriz por triangulaçao

    int i, j, k;
    double aux;
    double values[Msrc->rows];
    double matrix[Msrc->rows][Msrc->columns];

    if ((Msrc->columns == 1) && (Msrc->rows) == 1) {
        return Msrc->matrix[0][0];

    }
    //copia de matriz
    for (i = 0; i < (Msrc->rows); i++) {
        for (j = 0; j < (Msrc->columns); j++) {
            matrix[i][j] = Msrc->matrix[i][j];
        }
    }

    //1ªlinha
    values[0] = matrix[0][0];
    if (values[0] != 1) {
        for (i = 0; i < (Msrc->columns); i++) {
            matrix[0][i] = matrix[0][i] / values[0];
        }
    }
    //linhas seguintes
    for (i = 1; i < (Msrc->rows); i++) {
        for (k = 0; k < i; k++) {
            aux = matrix[i][k];
            for (j = k; j < (Msrc->columns); j++) {
                matrix[i][j] = matrix[i][j] - aux * matrix[i - 1][j];
            }
        }
        values[i] = matrix[i][i];
        for (j = 0; j < (Msrc->columns); j++) {
            matrix[i][j] = matrix[i][j] / values[i];
        }
    }
    aux = 1;
    for (i = 0; i < (Msrc->rows); i++) {
        aux = aux * values[i];
    }
    return aux;
}

//TODO: matrix_inv_safe
Matrix matrix_inv(Matrix *Msrc) {
    int i, j, m, n, x, y;
    double det;
    if (Msrc->rows != Msrc->columns) {
        printf("\n Matrix isn't square \n");
        fflush(stdout);
        exit(1);
    }
    det = matrix_det(Msrc);
    if (det == 0) {
        printf("\n Matrix is not inversable \n");
        fflush(stdout);
        exit(1);
    }

    Matrix Mtmp = empty_matrix(Msrc->rows, Msrc->columns);
    Matrix Maux = empty_matrix(Msrc->rows - 1, Msrc->columns - 1);

    //matriz adjunta de Msrc
    for (i = 0; i < (Mtmp.rows); i++) {
        for (j = 0; j < (Mtmp.columns); j++) {
            for (m = 0; m < (Mtmp.rows - 1); m++) {
                for (n = 0; n < (Mtmp.columns - 1); n++) {
                    x = m;
                    y = n;
                    if (m >= i) {
                        x = m + 1;
                    }
                    if (n >= j) {
                        y = n + 1;
                    }
                    Maux.matrix[m][n] = Msrc->matrix[x][y];
                }
            }
            Mtmp.matrix[i][j] = matrix_det(&Maux) / det;
            if (((i + j) % 2) != 0) {
                Mtmp.matrix[i][j] = -1 * Mtmp.matrix[i][j];
            }
        }
    }
    Matrix Mdest = matrix_tran(&Mtmp);
    return Mdest;
}

Matrix matrix_sum(Matrix *M1, Matrix *M2) {
    int i, j;
    Matrix Mdest = empty_matrix(M1->rows, M1->columns);

    for (i = 0; i < (M1->rows); i++) {
        for (j = 0; j < (M1->columns); j++) {
            Mdest.matrix[i][j] = M1->matrix[i][j] + M2->matrix[i][j];
        }
    }
    return Mdest;
}

Matrix matrix_sub(Matrix *M1, Matrix *M2) {
    int i, j;
    Matrix Mdest = empty_matrix(M1->rows, M1->columns);

    for (i = 0; i < (M1->rows); i++) {
        for (j = 0; j < (M1->columns); j++) {
            Mdest.matrix[i][j] = M1->matrix[i][j] - M2->matrix[i][j];
        }
    }
    return Mdest;
}

int strlen(char* str){
	int i;

	for(i=0;str[i]!='\0';i++);

	return i;
}

