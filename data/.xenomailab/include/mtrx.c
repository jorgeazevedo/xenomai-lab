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
#include <stdlib.h>
#include <stdio.h>

Matrix new_matrix2(char *str) {
    Matrix aux = empty_matrix(Rmax, Cmax);
    double temp = 0;
    int signal=1;
    int i = 1;
    int dec = 1;
    int col = 0;
    int row = 0;
    int num = 0;
    while (str[i] != ']') {
        if (str[i] == '\0') {
            printf("\n NEW_MATRIX : Invalid Input (Missing bracket ']')");
            fflush(stdout);
            exit(1);
        }
        dec = 1;
        temp = 0;
        //printf("\n 0| %d |", i);
        signal=1;
        if(str[i]=='-'){
            signal=-1;
            i++;
        }
        while ((str[i] >= '0') && (str[i] <= '9')) {
            // printf("\n 1| %d |", i);
            aux.matrix[row][col] = 10 * (aux.matrix[row][col]) + str[i] - '0';
            i++;
        }
        
        if (str[i] == '.') {
            //printf("\n 3| %d |", i);
            i++;
            while ((str[i] >= '0') && (str[i] <= '9')) {
                // printf("\n 4| %d |", i);
                temp = 10 * temp + str[i] - '0';
                dec = 10 * dec;
                i++;
            }
            aux.matrix[row][col] += (temp / dec);
        }
        aux.matrix[row][col]=signal*aux.matrix[row][col];
        if (str[i] == ';') {
            //printf("\n 5| %d |", i);
            num++;
            row++;
            col = 0;
            i++;
        }
        if (str[i] == ' ' || str[i] == ',') {
            if (str[i - 1] == ' ' || str[i - 1] == ',') {
                printf("\n NEW_MATRIX : Invalid Input %s (%c%c)", str,str[i - 1], str[i]);
                fflush(stdout);
                exit(1);
            }
            //printf("\n 6| %d |", i);
            num++;
            col++;
            i++;
        }
    }
    num++;
    aux.rows = row + 1;
    aux.columns = col + 1;
    if ((aux.rows * aux.columns) != num) {
        printf("\n NEW_MATRIX : Invalid Input (Number of values = %d | dimension = %dx%d )", num, aux.rows, aux.columns);
        fflush(stdout);
        exit(1);
    }
    return aux;
}


Matrix new_matrix(char *str) {
    Matrix aux = empty_matrix(Rmax, Cmax);
    int i=0, j=0, num=0;
    char* newStr;

    //TODO:
    //don't use malloc, use a char vector
    //and change pointer arithmetic to 
    //vector index
    newStr=malloc(255*sizeof(char));

    //Go to first number
    while((*str<'0'||*str>'9') && (*newStr!='-') )
	    str++;

    //Make a copy of string, substitute , for ' '
    for(i=0;str[i]!='\0';i++){
	    if(str[i]==',')
                    newStr[i]=' ';
	    else
                    newStr[i]=str[i];
    }
    newStr[i]='\0';

    //until the end of string
    for(i=0,j=0,num=0;*newStr!='\0';j++,num++){

    	//get number
        aux.matrix[i][j]=atof(newStr);
	
	//go to next non-number
        while((*newStr>='0'&&*newStr<='9')||*newStr=='.'||*newStr=='-'){
                newStr++;
	}

	//go to next number, check for ; and for \0!
        while( ( (*newStr<'0') || (*newStr>'9') ) && (*newStr!='-') ){
                if(*newStr=='\0')
			break;
                if(*newStr==';'){
			j=-1;
			i++;
		}
                newStr++;
	}
    }

    aux.rows = i + 1;
    aux.columns = j;
    if ((aux.rows * aux.columns) != num) {
        fprintf(stderr,"\n new_MATRIX : Invalid Input (Number of values = %d | dimension = %dx%d)\n", num, aux.rows, aux.columns);
        fflush(stdout);
        exit(1);
    }

    return aux;
  
}

/*
 * Same as new_matrix, but returns 1 instead
 * of running exit(1);
 */
int new_matrix_safe(Matrix* M1,char *str) {
    Matrix aux = empty_matrix(Rmax, Cmax);
    int i=0, j=0, num=0;
    char* newStr;

    fprintf(stderr,"Start at %s\n",str);

    //TODO:
    //don't use malloc, use a char vector
    //and change pointer arithmetic to
    //vector index
    newStr=malloc(255*sizeof(char));

    //Go to first number
    while((*str<'0'||*str>'9') && (*newStr!='-') )
            str++;

    //Make a copy of string, substitute , for ' '
    for(i=0;str[i]!='\0';i++){
            if(str[i]==',')
                    newStr[i]=' ';
            else
                    newStr[i]=str[i];
    }
    newStr[i]='\0';
    fprintf(stderr,"Copy is %s\n",newStr);

    //until the end of string
    for(i=0,j=0,num=0;*newStr!='\0';j++,num++){
        fprintf(stderr,"Outter Cycle Start %s\n",newStr);

        //get number
        aux.matrix[i][j]=atof(newStr);
        fprintf(stderr,"Got number: %8.4f \n",aux.matrix[i][j]);

        //go to next non-number
        while((*newStr>='0'&&*newStr<='9')||*newStr=='.'||*newStr=='-'){
                newStr++;
        }

        //go to next number, check for ; and for \0!
        while( ( (*newStr<'0') || (*newStr>'9') ) && (*newStr!='-') ){
            fprintf(stderr,"Inner Cycle Start\n");
                if(*newStr=='\0')
                        break;
                if(*newStr==';'){
                        j=-1;
                        i++;
                }
                newStr++;
            fprintf(stderr,"Inner Cycle End\n");
        }
        fprintf(stderr,"Outter Cycle End\n");
    }

    fprintf(stderr,"Nice\n");
    aux.rows = i + 1;
    aux.columns = j;
    if ((aux.rows * aux.columns) != num) {
        fprintf(stderr,"\n new_MATRIX : Invalid Input (Number of values = %d | dimension = %dx%d)\n", num, aux.rows, aux.columns);
        return 1;
    }

    *M1=aux;

    fprintf(stderr,"Finish\n");
    return 0;
}


Matrix empty_matrix(int rows, int col) {
    Matrix aux;
    int i, j;

    for (i = 0; i < Rmax; i++) {
        for (j = 0; j < Cmax; j++) {
            aux.matrix[i][j] = 0;
        }
    }
    aux.rows = rows;
    aux.columns = col;

    return aux;
}

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

/*
 * This was used for a test,
 * but shouldn't really be here. I think.
 */

void matrix_print(Matrix *M1){
	int j,k;

	printf("|\t");
	for(j=0;j < M1->rows;j++){
		for(k=0;k<M1->columns;k++){

			printf("%4.2f",M1->matrix[j][k]);


			if(k!=(M1->columns-1))
				printf("\t");
			else{
				if(j!=(M1->rows-1))
					printf("; ");
				else
					printf("|");
			}
		}
	}
	printf("\n");
		
}
