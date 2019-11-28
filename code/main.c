#include <stdio.h> /* needed for input, output, ... */
#include <stdlib.h> /* needed for EXIT_SUCCESS, ... */
#include <math.h> /* nedded for myFloor() and myCeil() */
#include <glpk.h> /* the linear programming toolkit */

#define EPSILON 0.00001 /* small value to deal with rounding issues */

/* global variables -- YOU SHOULD NOT CHANGE THESE! */
/* (You are allowed to add your own if you want.) */
int size; /* number of rows/columns in the input */
double *input; /* array that contains the input */
double *solution; /* array that contains the solution */
int debug; /* flag for debug mode; 1 means debug mode, 0 means debug off */

/* prototypes of functions -- YOU SHOULD NOT CHANGE THESE! */
/* (Feel free to add your own as you like.) */
int readInput(char *filename); /* reads graph from file */
/* readInput creates/fills the data structures (global variables) as needed */
/* it returns 0 if all is okay and 1 otherwise */
int computeSolution(void); /* computes the solution, stores it in input */
/* the return value is 1 if a solution exists, 0 otherwise */
void checkSolution(void); /* checks if a solution is valid */
double myFloor(double x); /* slightly 'hacky' myFloor function */
double myCeil(double x); /* slightly 'hacky' myCeil function */
void printMatrix(char *title, double *matrix); /* print a matrix */

/* This is the function that actually solves the problem. */
/* It is essentially empty and not functional. */
/* Your own implementation needs to go in here. */
int computeSolution(void) {
    //need to find the sums for each row and column

    //store the row and column sums here in an array
    double *rowSumArray = malloc(sizeof(double) * size);
    double *columnSumArray = malloc(sizeof(double) * size);

    for (int col = 0; col < size; col++) {
        double columnSum = 0;
        for (int row = 0; row < size; row++)
        {
            columnSum += input[row * size + col];
        }
        printf("Sum for col %d = %f\n", col, columnSum);
        columnSumArray[col] = columnSum;
    }

    for (int row = 0; row < size; row++)
    {
        double rowSum = 0;
        for (int col = 0; col < size; col++)
        {
            rowSum += input[row * size + col];
        }
        printf("Sum for row %d = %f\n", row, rowSum);
        rowSumArray[row] = rowSum;
    }

    glp_prob *lp; /* the linear programming problem */

    lp = glp_create_prob(); /* create LP problem instance */
    glp_set_obj_dir(lp, GLP_MAX); /* set maximisation as objective */

    //the number of constraints is the dimension squared plus 2 lots of the dimension
    //because if you have a 3x3 for example then the first set of 3 nodes are each connected to 3 other nodes
    //so that's 9. Then the source and sink are each connected to 3 more so another 6 + 9 = 15 == 3^2 + 6 = 15
    //with a 4x4 as well then you have the first 4 nodes each connected to 4 other nodes so 16 plus 2 lots of 4
    // which is 24
    int constraints = (size * size) + size + size; // size squared plus 2 lots of size
    glp_add_rows(lp, constraints);


    return 0; /* This is not always correct, of course, and needs to be changed. */
}

/* YOU SHOULD NOT CHANGE ANYTHING BELOW THIS LINE! */

int main(int argc, char **argv) {
    int arg; /* used to run over the command line parameters */

    if (argc < 2) { /* no command line parameter given */
        fprintf(stderr, "Usage: %s [file1] [file2] [file3] [...]\n"
                        "Where each [file] indicates the name of a file with an input.\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argv[1][0] == '-' && argv[1][1] == 'd' && argv[1][2] == 0) {
        /* If the first parameter is -d we activate debug mode. */
        debug = 1; /* switch debug mode on */
        fprintf(stdout, "DEBUG: Debug mode activated\n"); /* tell us */
    } else {
        debug = 0; /* switch debug mode off */
    }

    for (arg = 1 + debug; arg < argc; arg++) { /* go over remaining parameters */
        if (readInput(argv[arg])) { /* try to read file */
            /* readInput returned with error message */
            fprintf(stderr, "%s: Cannot read input with filename %s. Skipping.\n",
                    argv[0], argv[arg]);
        } else {
            if (computeSolution()) {
                fprintf(stdout, "%s: Input %s has a solution.\n",
                        argv[0], argv[arg]);
                checkSolution();
                printMatrix("Input", input);
                printMatrix("Solution", solution);
            } else {
                fprintf(stdout, "%s: Input %s does not have a solution.\n",
                        argv[0], argv[arg]);
            }
            /* free memory for next input */
            free(input);
            free(solution);
        }
    }
    return EXIT_SUCCESS;
}

/* The following function prints a matrix including the row and column sums */
void printMatrix(char *title, double *matrix) {
    int i, j; /* looping over rows and columns */
    double sum; /* to compute the sum */

    fprintf(stdout, "%s:\n", title);
    /* print the matrix and compute the row sums on the fly */
    for (i = 0; i < size; i++) {
        for (j = 0, sum = 0.0; j < size; j++) {
            sum += matrix[i * size + j];
            fprintf(stdout, "%8.2f ", matrix[i * size + j]);
        }
        fprintf(stdout, "(row sum: %8.2f)\n", sum);
    }
    /* print separating line */
    for (j = 0; j < size; j++) {
        fprintf(stdout, "---------");
    }
    fprintf(stdout, "\n");
    /* now compute the column sums and print them */
    for (j = 0; j < size; j++) { /* we consistently use j for columns */
        for (i = 0, sum = 0.0; i < size; i++) {
            sum += matrix[i * size + j];
        }
        fprintf(stdout, "%8.2f ", sum);
    }
    fprintf(stdout, "(column sums)\n");
}

/* The following function checks of a solution is valid. */
void checkSolution(void) {
    int i, j; /* to run over the arrays */
    double sum1, sum2; /* to compute the sums over the rows and columns */

    /* check rows and that all numbers are integers and rounded */
    for (i = 0; i < size; i++) {
        for (j = 0, sum1 = sum2 = 0.0; j < size; j++) {
            sum1 += input[i * size + j];
            sum2 += solution[i * size + j];
            if (myFloor(solution[i * size + j]) != solution[i * size + j]) {
                fprintf(stdout, "Error: %lf is not an integer (%d/%d).\n",
                        solution[i * size + j], i, j);
                return;
            }
            if ((myFloor(input[i * size + j]) != solution[i * size + j]) &&
                (myCeil(input[i * size + j]) != solution[i * size + j])) {
                fprintf(stdout, "Error: %lf is not rounded from %lf (%d/%d).\n",
                        solution[i * size + j], input[i * size + j], i, j);
                return;
            }
        }
        if ((myFloor(sum1) != sum2) && (myCeil(sum1) != sum2)) {
            fprintf(stdout, "Error: Row sum for row %d not valid.\n", i);
            return;
        }
    }
    /* check columns*/
    for (j = 0; j < size; j++) {
        for (i = 0, sum1 = sum2 = 0.0; i < size; i++) {
            sum1 += input[i * size + j];
            sum2 += solution[i * size + j];
        }
        if ((myFloor(sum1) != sum2) && (myCeil(sum1) != sum2)) {
            fprintf(stdout, "Error: Row sum for row %d not valid.\n", i);
            return;
        }
    }
}

int readInput(char *filename) {
    FILE *fh; /* file handle to read input */
    int i, j; /* variables to run over array */
    double value; /* value read from input file */

    /* try to open the file for reading */
    if ((fh = fopen(filename, "rt")) == NULL) {
        if (debug) {
            fprintf(stdout, "DEBUG: Unable to open file %s for reading.\n",
                    filename);
        }
        return 1; /* unable to open file, flag failure */
    }
    /* read the first integer, the number of columns/rows */
    if (fscanf(fh, "%d", &size) != 1) {
        if (debug) {
            fprintf(stdout, "DEBUG: Unable to read input size.\n");
        }
        fclose(fh); /* close file to avoid ununsed open files */
        return 1; /* flag failure */
    }

    if (size < 2) {
        if (debug) {
            fprintf(stdout, "DEBUG: Received %d as input size.\n", size);
        }
        fclose(fh); /* close file to avoid unused open files */
        return 1; /* flag failure */
    }
    /* allocate the memory for the input */
    if ((input = (double *) malloc(sizeof(double) * size * size)) == NULL) {
        if (debug) {
            fprintf(stdout, "DEBUG: Unable to allocate %ld bytes.\n",
                    sizeof(int) * size * size);
        }
        fclose(fh); /* close file to avoid unused open files */
        return 1; /* flag failure */
    }
    /* allocate the memory for the solution */
    if ((solution = (double *) malloc(sizeof(double) * size * size)) == NULL) {
        if (debug) {
            fprintf(stdout, "DEBUG: Unable to allocate %ld bytes.\n",
                    sizeof(int) * size * size);
        }
        fclose(fh); /* close file to avoid unused open files */
        return 1; /* flag failure */
    }

    /* read the actual values */
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            /* attempt to read next value */
            if (fscanf(fh, "%lf", &value) != 1) {
                if (debug) {
                    fprintf(stdout, "DEBUG: Unable to read input value (%d/%d).\n",
                            i, j);
                }
                /* free memory and close file */
                free(input);
                free(solution);
                fclose(fh);
                return 1; /* flag failure */
            }
            input[i * size + j] = value;
        }
    }
    if (debug) {
        fprintf(stdout, "Read the following input with size %d x %d\n", size, size);
        for (i = 0; i < size; i++) {
            for (j = 0; j < size; j++) {
                fprintf(stdout, "%5.2f ", input[i * size + j]);
            }
            fprintf(stdout, "\n");
        }
    }
    return 0; /* all okay */
}

double myFloor(double x) {
    return floor(x + EPSILON);
}

double myCeil(double x) {
    return ceil(x - EPSILON);
}