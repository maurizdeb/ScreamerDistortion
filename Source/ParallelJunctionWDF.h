/*
  ==============================================================================

    ParallelJunctionWDF.h
    Created: 5 Apr 2020 11:58:48am
    Author:  Maurizio de Bari

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MatrixEng.h"

class ParallelJunctionWDF{
    
public:
        
    ParallelJunctionWDF();
    ParallelJunctionWDF(double portRes1, double portRes2, int adaptPortID);
    ~ParallelJunctionWDF();
        
    // getter
    double** getS();
    double getAdaptRes();
        
private:
        
    double** S;
    double adaptRes;
    
};
