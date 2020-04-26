/*
  ==============================================================================

    SeriesJunctionWDF.h
    Created: 5 Apr 2020 11:57:46am
    Author:  Maurizio de Bari

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MatrixEng.h"

class SeriesJunctionWDF
{
public:
    
    SeriesJunctionWDF();
    SeriesJunctionWDF(double portRes1, double portRes2, int adaptPortID);
    ~SeriesJunctionWDF();
    
    // getter
    double** getS();
    double getAdaptRes();
    
private:
    
    double** S;
    double adaptRes;
};
