/*
  ==============================================================================

    MatrixEng.h
    Created: 6 Apr 2020 7:11:46pm
    Author:  Maurizio de Bari

  ==============================================================================
*/

#pragma once

class MatrixEng{
    
public:
    
    static double** onesFill(int numRow, int numCol);
    static double** identity(int N);
    
private:
    MatrixEng() {};
    
};
