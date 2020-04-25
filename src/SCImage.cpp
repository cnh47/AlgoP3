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

// constructor
SCImage::SCImage(string fileName)
{
    collectFromFile(fileName);
    peMatrix = NULL;
    genPixEnerMatrix();
}

// get stuff from file
void SCImage::collectFromFile(string imageName)
{
            // gather file info
        ifstream inFile;
        inFile.open(imageName);

        string processInFile;
        int stepCount = 1;
        vector<string> cArray;

        // collect File into a string
        while (getline(inFile, processInFile))
        {
            if (processInFile[0]=='#')
            {
                // comment line do nothing
            }//comment
            else if (stepCount == 1)
            {
                // p2 literal
                stepCount++;
            } // End P2
            else if (stepCount == 2)
            {
                // size of image
                int space = processInFile.find(" ");
                string xStr = processInFile.substr(0,space);
                string yStr = processInFile.substr(space + 1);
                w = atoi(xStr.c_str());
                h = atoi(yStr.c_str());

                // make temp image array
                image = new int*[w];
                for(int i = 0; i < w; i++)
                    image[i] = new int[h];

                stepCount++;
            } // end X Y
            else if(stepCount == 3)
            {
                // max Greyscale value
                maxGreyScale = processInFile;
                stepCount++;
            }// end greyscale
            else
            {
                // content of image
                string numString = "";
                int strLen = processInFile.length();
                // get actual content
                for (int i = 0; i < strLen; i++)
                {
                    if(processInFile[i] == ' ' || processInFile[i] == '\t' || processInFile[i] == '\n')
                    {
                        cArray.push_back(numString);
                        numString = "";
                    }
                    else
                    {
                        numString += processInFile[i];
                    }
                }// end for

                if (numString != "")
                    cArray.push_back(numString);

            }// end data else

        }// end data collection while

        inFile.close();

        // fill image with contetn from contentArray
        int vectorIterator = 0;
        for (int y = 0; y < h; y++)
        {
            for(int x = 0; x < w; x++)
            {
                string curIter = cArray.at(vectorIterator);
                image[x][y] = atoi(curIter.c_str());
                vectorIterator++;
            }// x For
        }// y For
}// End Collect From File

// output carved image
void SCImage::outputCarvImage(string fileName)
{
    ofstream outFile(fileName + "_processed.pgm");
    // output housekeeping variables
    outFile << "P2" << endl << w << " " << h << endl << maxGreyScale << endl;

    // output image
    for (int y = 0; y < h; y++)
    {
        for(int x = 0; x < w; x++)
        {
            outFile<< image[x][y] << " ";
        }// x
        outFile << endl;
    }// y
    outFile.close();
}// output carved image

// Generate the Pixel Energy Matrix
void SCImage::genPixEnerMatrix()
{
    // Get Rid of old Matrix
    if (peMatrix != NULL)
    {
        for (int i = 0; i < w -1; i++)
            delete [] peMatrix[i];

        delete [] peMatrix;
    }

    // make new
    peMatrix = new int*[w];

    // populate each column
    for (int i = 0; i < w; i++)
        peMatrix[i] = new int[h];

    for (int y =0; y < h; y++)
    {
        for (int x = 0; x<w; x++)
            peMatrix[x][y] = calcPixEner(x,y);
    }// y
} // end Gen Pix Energy Matrix

// Calculate the Pixel Energy
int SCImage::calcPixEner(int x, int y)
{
    int currPix = image[x][y]; // current pixel
    int abovePix,belowPix,rightPix, leftPix;

    // initialize values, with test cases inmind
    if (x==0)
        leftPix = image[x][y];
    else
        leftPix = image[x-1][y];

    if ( y ==0)
        belowPix = image[x][y];
    else
        belowPix = image[x][y-1];

    if (x == w - 1)
        rightPix = image[x][y];
    else
        rightPix = image[x+1][y];

    if (y == h-1)
        abovePix = image[x][y];
    else
        abovePix = image[x][y+1];

    return (abs(currPix-abovePix) + abs(currPix-belowPix) + abs(currPix-rightPix) + abs(currPix - leftPix));

} // end calc pixel energy

// Seam carve the Image
void SCImage::sC(int vCarves, int hCarves)
{
    for ( int i = 0; i < vCarves; i++)
    {
        vCarve();
        genPixEnerMatrix();
    }
    for (int i = 0; i < hCarves; i++)
    {
        hCarve();
        genPixEnerMatrix();
    }
}// end seam carve image


void SCImage::vCarve()
{
    idVSeam();
    deleteVSeam();
}

void SCImage::hCarve()
{
    // if Rotate Array from [x][y] to [y][x] I can just do vertical seam logic, and then swap back using same logic for identical image
    rotateImage();
    vCarve();
    rotateImage();
}

void SCImage::idVSeam()
{
    // get Cumulative Energy Matrx
    int** cumEnergy = new int*[w];
    // populate
    for(int i = 0; i < w; i++)
        cumEnergy[i] = new int[h];

    for (int y = 0; y<h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            if (y==0)
            {
                cumEnergy[x][y] = peMatrix[x][y];
            }// if
            else
            {
                int minLast;
                if (x == 0) // edge base case
                {
                    minLast = min(cumEnergy[x+1][y-1], cumEnergy[x][y-1]);
                }
                else if (x == (w-1)) // edge basecase
                {
                    minLast = min(cumEnergy[x-1][y-1], cumEnergy[x][y-1]);
                }
                else // general case
                {
                    minLast = min(min(cumEnergy[x-1][y-1], cumEnergy[x+1][y-1]),cumEnergy[x][y-1]);
                }
                cumEnergy[x][y] = peMatrix[x][y] + minLast;
            }// else
        }// x
    }// y


    // Identify the Seam
    int lowestIndex = 0;

    for (int i = 0; i < w; i++)
    {
        if(cumEnergy[i][h-1] < cumEnergy[lowestIndex][h-1])
            lowestIndex=i;
    }
    image[lowestIndex][h-1] = -1; // -1 So we can find and delete the pixel

    for(int i = 1; i <h; i++)
    {
        int currentH = h -1 - i;
        int rIndex = lowestIndex + 1;
        int mIndex = lowestIndex;
        int lIndex = lowestIndex -1;

        if (rIndex <0 || rIndex >= w)
            rIndex = mIndex;
        if(lIndex < 0 || lIndex >= w)
            lIndex = mIndex;

        int lowestValue = min(min(cumEnergy[rIndex][currentH], cumEnergy[lIndex][currentH]),cumEnergy[mIndex][currentH]);
        // determine if left right or center
        if(cumEnergy[lIndex][currentH] == lowestValue)
            lowestIndex = lIndex;
        else if(cumEnergy[mIndex][currentH]== lowestValue)
            lowestIndex = mIndex;
        else if(cumEnergy[rIndex][currentH] == lowestValue)
            lowestIndex = rIndex;

        image[lowestIndex][currentH] = -1; // Update new Lowest index spot to a delete pixel

    }// for

} // id VSeam

void SCImage::deleteVSeam()
{
    int oldW = w;
    w--;
    vector<int> contArray;

    // collect non deleted image
    for (int y = 0; y <h; y++)
    {
        for (int x = 0; x < oldW; x++)
        {
            if (image[x][y]!= -1)
                contArray.push_back(image[x][y]);
        }//x
    }// y

    // Delete old image;
    for (int i = 0; i < w; i++)
        delete [] image[i];

    delete [] image;

    // create new image
    image = new int*[w];
    for (int i = 0; i <w; i++)
    {
        image[i] = new int[h];
    }

    int vectIter = 0;
    for(int y = 0; y <h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            image[x][y] = contArray.at(vectIter);
            vectIter++;
        }// x
    }//y
}// end delete VSeam

// Rotate the Image for Horizontal Seam
// XtoY and YtoX
// reversible by just doing process again
void SCImage::rotateImage()
{
    // delete energy matrix
    for (int i = 0; i <w; i++)
        delete [] peMatrix[i];

    delete [] peMatrix;
    peMatrix = NULL;

    int preRotateW = w;
    int preRotateH = h;

    h = preRotateW;
    w = preRotateH;

    // rotate
    int** oldImage = image;
    image = new int*[w];
    for(int i = 0; i<w; i++)
        image[i] = new int[h];

// rotate
    for (int y = 0; y<h; y++)
    {
        for (int x = 0; x< w; x++)
        {
            image[x][y] = oldImage[y][x];
        }
    }//y

    // generate energy matrix
    genPixEnerMatrix();
    // delete old image
    for(int i = 0; i < h; i++)
        delete [] oldImage[i];

    delete [] oldImage;

}
