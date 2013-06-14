#include "mtrx.h"

void main(void){
        DEBUG("Hello MOTHERFUCKERS!\n");

        Matrix M1 = new_matrix("[1 2 3 4 5 6;1 2 3 4 5 6;1 2 3 4 5 6;1 2 3 4 5 6;1 2 3 4 5 6;1 2 3 4 5 6]");
        matrix_print(&M1);
        matrix_print_pretty(&M1,"6x6",NULL);
}
