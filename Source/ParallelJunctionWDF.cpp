/*
  ==============================================================================

    ParallelJunctionWDF.cpp
    Created: 5 Apr 2020 11:58:48am
    Author:  Maurizio de Bari

  ==============================================================================
*/

#include "ParallelJunctionWDF.h"

ParallelJunctionWDF::ParallelJunctionWDF(){
    
    
}

ParallelJunctionWDF::ParallelJunctionWDF(double portRes1, double portRes2, int adaptPortId){
    
    double adaptG = (1/portRes1) + (1/portRes2);
    double g_n[3];
    
    switch (adaptPortId) {
        case 1:
            g_n[0] = adaptG;
            g_n[1] = 1/portRes1;
            g_n[2] = 1/portRes2;
            break;
        case 2:
            g_n[0] = 1/portRes1;
            g_n[1] = adaptG;
            g_n[2] = 1/portRes2;
            break;
        case 3:
            g_n[0] = 1/portRes1;
            g_n[1] = 1/portRes2;
            g_n[2] = adaptG;
    }
    double sum_elements = g_n[0] + g_n[1] + g_n[2];
    double alpha[3];
    alpha[0] = g_n[0]*(2/sum_elements);
    alpha[1] = g_n[1]*(2/sum_elements);
    alpha[2] = g_n[2]*(2/sum_elements);
    double** eye = MatrixEng::identity(3);
    S = new double*[3];
    for (int i = 0; i < 3; i++) {
        S[i] = new double[3];
        for (int j = 0; j<3; j++) {
            S[i][j] = -eye[i][j] + alpha[j];
        }
    }
    
    adaptRes = 1/adaptG;
}

ParallelJunctionWDF::~ParallelJunctionWDF(){
    
}

double** ParallelJunctionWDF::getS(){
    return S;
}

double ParallelJunctionWDF::getAdaptRes(){
    return adaptRes;
}
