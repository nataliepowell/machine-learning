#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double** transpose (int m, int n, double **input);
double** newMatrix (int m, int n);
double** identity (int n);
double** invert (int m, int n, double** M);
double** multiply (double** a, double** b, int rows_a, int cols_a, int rows_b, int cols_b);
double* matrixVectorProduct (double**a, double* b, int rows_a, int cols_a);
void freeMatrix (int m, double** matrix);

int main(int argc, const char * argv[]) {
 if (argc != 3) {
    printf("Error: incorrect number of arguments.\n");
     return 1; } // failure

FILE *matrix;
 char line[6];
 double **x = NULL, **data = NULL;
double *y = NULL;
int  k=0, n=0, check = 0, fileType = 0; // fileType = 0: train, fileType = 1: data
 int trainRows=0, dataRows=0, cols=0;
 for  (int i=1; i<=2; i++){
     matrix = fopen(argv[i], "r");
         if (!matrix) {
             printf("error\n");
             return 1; } // failure - could not open file
     fscanf(matrix, "%5s", line);
     if (strcmp(line, "train") == 0)
             fileType = 0; // "train"
     else if (strcmp(line, "data") == 0)
             fileType = 1; // "data"
     else
             return 1; // failure - not "train" or "data"
    fscanf(matrix, "%d", &k); // number of attributes (columns)
     if (i==1)
         check = k; // tmp variable; holds value of "k" in training file
     if (i==2 && k!=check){
         printf("error\n");
         return 1; } // failure - k's do not match in each file
    fscanf(matrix, "%d", &n); // number of houses (rows)
     cols = k+1;
         if (fileType == 0) {
             trainRows = n;
             x = malloc(trainRows*sizeof(double*));
             y = malloc(trainRows*sizeof(double*));
             for (int i=0; i<trainRows; i++) {
                 x[i] = malloc(cols*sizeof(double));
                 x[i][0] = 1;
                 for (int j=1; j<=k; j++)
                     fscanf(matrix, "%lf", &x[i][j]);
                 fscanf(matrix, "%lf", &y[i]);
             }
         }
         else if (fileType == 1) {
             dataRows = n;
             data = malloc(dataRows*sizeof(double));
             for (int i=0; i<dataRows; i++) {
                 data[i] = malloc(cols*sizeof(double));
                 data[i][0] = 1;
                 for (int j=1; j<=k; j++)
                     fscanf(matrix, "%lf", &data[i][j]);
             }
         }
     }
    
    double **trans = transpose(trainRows, cols, x);
    double **mult1 = multiply(trans, x, cols, trainRows, trainRows, cols);
    double **inv = invert(cols, cols, mult1);
    double **mult2 = multiply(inv, trans, cols, cols, cols, trainRows);
    double *w = matrixVectorProduct(mult2, y, cols, trainRows);
    double *newPrices = matrixVectorProduct(data, w, dataRows, cols);
    for (int i=0; i<dataRows; i++)
        printf("%.0f\n", newPrices[i]);
    
    freeMatrix(trainRows, x);
    freeMatrix(dataRows, data);
    freeMatrix(cols, trans);
    freeMatrix(cols, mult1);
    freeMatrix(cols, inv);
    freeMatrix(cols, mult2);
    free(y);
    free(w);
    free(newPrices);
    
    return 0;
 }

double** newMatrix (int m, int n) {
    double **new = malloc(m*sizeof(double*));
    for (int i=0; i<m; i++) {
        new[i] = malloc(n*sizeof(double));
        for (int j=0; j<n; j++)
            new[i][j] = 0; // populates with zero
    }
    return new;
}

double** transpose (int m, int n, double **input) {
    double **output = newMatrix(n, m);
    for (int i=0; i<n; i++) {
        for (int k=0; k<m; k++)
            output[i][k] = input[k][i];
    }
    return output;
}

double** identity (int n) {
    double** id = newMatrix(n, n);
    for (int i=0; i<n; i++)
        id[i][i] = 1;
    return id;
}

double** invert (int m, int n, double** M) {
    if (n != m)
        return NULL; // cannot be inverted
    double** N = identity(n);
    double f;
    for (int p=0; p<n; p++) {
        f = M[p][p];
        for (int q=0; q<n; q++) {
            M[p][q] /= f;
            N[p][q] /= f;
        }
        for (int i=p+1; i<n; i++) {
            f = M[i][p];
            for (int q=0; q<n; q++) {
                M[i][q] -= (M[p][q] * f);
                N[i][q] -= (N[p][q] * f); }
        }
    }
    for (int p=n-1; p>=0; p--) {
        for (int i=p-1; i>=0; i--) {
            f = M[i][p];
            for (int q=0; q<n; q++) {
                 M[i][q] -= (M[p][q] * f);
                N[i][q] -= (N[p][q] * f);
            }
        }
    }
    return N;
}


double** multiply (double** a, double** b, int rows_a, int cols_a, int rows_b, int cols_b) {
    double** product = newMatrix(rows_a, cols_b);
    double sum;
    for (int i=0; i<rows_a; i++) {
        for(int j=0; j<cols_b; j++) {
            sum=0;
            for(int k=0; k<cols_a; k++)
                sum+=(a[i][k]*b[k][j]);
            product[i][j]=sum; }
    }
    return product;
}

double* matrixVectorProduct (double**a, double* b, int rows_a, int cols_a) {
     double* product = malloc(sizeof(double)*rows_a);
    double sum;
    for (int i=0; i<rows_a; i++) {
        sum=0;
        for(int k=0; k<cols_a; k++) {
            sum+=(a[i][k]*b[k]);
            product[i]=sum; }
    }
    return product;
}

void freeMatrix (int m, double** matrix) {
    for (int i=0; i<m; i++) {
        free(matrix[i]); // indicates new row
    }
    free (matrix);
}
