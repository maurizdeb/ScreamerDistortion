/*
  ==============================================================================

    SeriesJunctionWDF.cpp
    Created: 5 Apr 2020 11:57:46am
    Author:  Maurizio de Bari

  ==============================================================================
*/

#include "SeriesJunctionWDF.h"

SeriesJunctionWDF::SeriesJunctionWDF(){
    
}

SeriesJunctionWDF::SeriesJunctionWDF(double portRes1, double portRes2, int adaptPortId){
    
    adaptRes = portRes1 + portRes2;
    
    double r_n[3];
    
    switch (adaptPortId) {
        case 1:
            r_n[0] = adaptRes;
            r_n[1] = portRes1;
            r_n[2] = portRes2;
            break;
        case 2:
            r_n[0] = portRes1;
            r_n[1] = adaptRes;
            r_n[2] = portRes2;
            break;
        case 3:
            r_n[0] = portRes1;
            r_n[1] = portRes2;
            r_n[2] = adaptRes;
            break;
    }
    double sum_elements = r_n[0] + r_n[1] + r_n[2];
    double alpha[3];
    alpha[0] = r_n[0]*(2 / sum_elements );
    alpha[1] = r_n[1]*(2 / sum_elements );
    alpha[2] = r_n[2]*(2 / sum_elements );
    double** eye =MatrixEng::identity(3);
    S = new double*[3];
    for (int i = 0; i < 3; i++) {
        S[i] = new double[3];
        for (int j = 0; j<3; j++) {
            S[i][j] = eye[i][j] - alpha[i];
        }
    }
}

SeriesJunctionWDF::~SeriesJunctionWDF(){
    
}

double** SeriesJunctionWDF::getS(){
    return S;
}

double SeriesJunctionWDF::getAdaptRes(){
    return adaptRes;
}
