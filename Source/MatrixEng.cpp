/*
  ==============================================================================

    MatrixEng.cpp
    Created: 6 Apr 2020 7:11:46pm
    Author:  Maurizio de Bari

  ==============================================================================
*/

#include "MatrixEng.h"

double** MatrixEng::onesFill(int numRow, int numCol){
    
    double** result = new double*[numRow];
    
    for (int i = 0; i<numRow; i++) {
        result[i] = new double[numCol];
        for (int j=0; j<numCol; j++) {
            result[i][j] = 1.0;
        }
    }
    
    return result;
}

double** MatrixEng::identity(int N){
    
    double** result = new double*[N];
    
    for (int i = 0; i<N; i++) {
        result[i] = new double[N];
        for (int j=0; j<N; j++) {
            if (i==j){
                result[i][j] = 1.0;
            }else{
                result[i][j] = 0.0;
            }
        }
    }
    
    return result;
}
