#ifndef SCIMAGE_H_INCLUDED
#define SCIMAGE_H_INCLUDED

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

class SCImage
{
    private:
        int** image;
        int** peMatrix;
        int w;
        int h;
        string maxGreyScale;


    public:
        SCImage(string);
        void sC(int,int);
        void outputCarvImage(string);

    private:
        void collectFromFile(string);
        void genPixEnerMatrix();
        int calcPixEner(int,int);
        void vCarve();
        void hCarve();
        void idVSeam();
        void deleteVSeam();
        void rotateImage();


};


#endif // SCIMAGE_H_INCLUDED
