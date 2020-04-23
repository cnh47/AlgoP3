#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <bits/stdc++.h>

void OutputMatrix(std::vector<std::vector<int> > &matrix);
void FillFinalMatrix(std::vector<std::vector<int> > &ValueMatrix, std::vector<std::vector<int> > &FinalMatrix);
void FillValueMatrix(std::vector<std::vector<int> > &matrix, std::ifstream &input);
void FillEnergyMatrices(std::vector<std::vector<int> > &ValueMatrix, std::vector<std::vector<int> > &EnergyMatrix, std::vector<std::vector<int> > &CumulativeMatrix);
void FindSeam(std::vector<std::vector<int> > &CumulativeMatrix, std::vector<std::vector<int> > &ValueMatrix);
void TransposeMatrix(std::vector<std::vector<int> > &ValueMatrix);
void RemoveSeam(int &Y_HEIGHT, int &X_WIDTH, int SEAMS, std::vector<std::vector<int> > &ValueMatrix);
void OutputFinalMatrix(const std::vector<std::vector<int>> &matrix, std::ofstream &output, int MAX_VALUE);
void GetValuesFromFile(int &X_WIDTH, int &Y_HEIGHT, int &MAX_VALUE, std::ifstream &input);
int minCheck(int i, int j, int k);


int main(int argc, char *argv[]){

    if (argc != 4)
    {
        std::cout << "Please use the following format: '[filename] [VERTICAL_SEAMS] [HORIZTONAL_SEAMS]'\n";
        return 0;
    }

    int MAX_VALUE, X_WIDTH, Y_HEIGHT;
    int VERTICAL_SEAMS =   atoi(argv[2]);
    int HORIZTONAL_SEAMS = atoi(argv[3]);

    std::ifstream input(argv[1]);
    std::string filename = argv[1];

    GetValuesFromFile(X_WIDTH, Y_HEIGHT, MAX_VALUE, input);
    std::vector<std::vector<int>> ValueMatrix(Y_HEIGHT, std::vector<int> (X_WIDTH, 0));

    FillValueMatrix(ValueMatrix, input);
    RemoveSeam(Y_HEIGHT, X_WIDTH, VERTICAL_SEAMS, ValueMatrix);
    TransposeMatrix (ValueMatrix);
    std::swap(X_WIDTH, Y_HEIGHT);
    RemoveSeam(Y_HEIGHT, X_WIDTH, HORIZTONAL_SEAMS, ValueMatrix);
    TransposeMatrix(ValueMatrix);
    std::size_t pos = filename.find_last_of(".");
    if (pos != std::string::npos) filename = filename.substr(0, pos) + "_processed" + filename.substr(pos, pos + 2);
    std::ofstream output(filename);
    OutputFinalMatrix(ValueMatrix, output, MAX_VALUE);
    output.close();
    input.close();
    return 0;
}

void GetValuesFromFile(int &X_WIDTH, int &Y_HEIGHT, int &MAX_VALUE, std::ifstream &input) {
  std::string P2;
  std::getline(input, P2);

  input.ignore(500, '\n');

  input >> X_WIDTH >> Y_HEIGHT;
  input >> MAX_VALUE;

}

void FillValueMatrix(std::vector<std::vector<int> > &matrix, std::ifstream &input) {
  for (int i = 0; i < matrix.size(); ++i) {
    for (int j = 0; j < matrix[i].size(); ++j) {
      int temp;
      input >> temp;
      matrix[i][j] = temp;
    }
  }
}

void RemoveSeam(int &Y_HEIGHT, int &X_WIDTH, int SEAMS, std::vector<std::vector<int> > &ValueMatrix) {
  for (int i=0; i<SEAMS; ++i) {

    std::vector<std::vector<int>> EnergyMatrix     (Y_HEIGHT, std::vector<int> (X_WIDTH,    0  ));
    std::vector<std::vector<int>> CumulativeMatrix (Y_HEIGHT, std::vector<int> (X_WIDTH,    0  ));
    std::vector<std::vector<int>> FinalMatrix      (Y_HEIGHT, std::vector<int> (X_WIDTH-1,  0  ));

    FillEnergyMatrices (ValueMatrix, EnergyMatrix, CumulativeMatrix);
    FindSeam(CumulativeMatrix, ValueMatrix);
    FillFinalMatrix(ValueMatrix, FinalMatrix);
    --X_WIDTH;
    ValueMatrix.clear();
    ValueMatrix = FinalMatrix;

  }
}

void FillEnergyMatrices(std::vector<std::vector<int> > &ValueMatrix, std::vector<std::vector<int> > &EnergyMatrix, std::vector<std::vector<int> > &CumulativeMatrix) {

    for (int i = 0; i < ValueMatrix.size(); ++i) {
        for (int j = 0; j < ValueMatrix[i].size(); ++j) {

            int Y_HEIGHT = ValueMatrix.size();
            int X_WIDTH = ValueMatrix[i].size();

            bool widthMin   = (j == 0);
            bool widthMax   = (j == X_WIDTH - 1);
            bool heightMin  = (i == 0);
            bool heightMax  = (i== Y_HEIGHT - 1);

            int leftUp      =  INT_MAX,
            rightUp      =  INT_MAX,
            Up      =  0;

            int Energy      = ValueMatrix[i][j];

            int FinalEnergy = 0;

            if (!widthMax)  { FinalEnergy += abs(Energy - ValueMatrix[i][j+1]); }
            if (!widthMin)  { FinalEnergy += abs(Energy - ValueMatrix[i][j-1]); }
            if (!heightMax) { FinalEnergy += abs(Energy - ValueMatrix[i+1][j]); }
            if (!heightMin) { FinalEnergy += abs(Energy - ValueMatrix[i-1][j]); }

            EnergyMatrix[i][j] = FinalEnergy;
            CumulativeMatrix[i][j] = FinalEnergy;

            if (i > 0) {

                if (!widthMin)  {  leftUp = FinalEnergy + CumulativeMatrix[i-1][j-1]; }
                if (!widthMax)  { rightUp = FinalEnergy + CumulativeMatrix[i-1][j+1]; }
                Up = FinalEnergy + CumulativeMatrix[i-1][j];

                CumulativeMatrix[i][j] = minCheck(leftUp, rightUp, Up);
            }

        }
    }
}

int minCheck(int i, int j, int k){
    int a = std::min(i, j);
    return std::min(a, k);
}

void FindSeam(std::vector<std::vector<int> > &CumulativeMatrix, std::vector<std::vector<int> > &ValueMatrix) {

  int Y_HEIGHT = CumulativeMatrix.size();
  int X_WIDTH  = CumulativeMatrix[0].size();
  int minimum_index = 0;

  for (int i = 1; i < X_WIDTH; ++i) {;
      if (CumulativeMatrix[Y_HEIGHT-1][minimum_index] > CumulativeMatrix[Y_HEIGHT-1][i]) minimum_index = i;
  }

  ValueMatrix[Y_HEIGHT-1][minimum_index] = -1;

  for (int i = Y_HEIGHT - 1; i >= 0; --i) {

   bool widthMin  = (minimum_index == 0);
   bool widthMax  = (minimum_index == X_WIDTH - 1);

   int leftUp =  INT_MAX;
   int rightUp =  INT_MAX;
   int Up =  0;

   if (i > 0) {

     if (!widthMin)  {  leftUp = CumulativeMatrix[i-1][minimum_index-1]; }
     if (!widthMax)  { rightUp = CumulativeMatrix[i-1][minimum_index+1]; }
     Up = CumulativeMatrix[i-1][minimum_index];
     if(leftUp ==  minCheck(leftUp, rightUp, Up)) { ValueMatrix[i-1][minimum_index-1]  = -1; --minimum_index;}
     else if (    Up ==  minCheck(leftUp, rightUp, Up)) { ValueMatrix[i-1][minimum_index]    = -1;}
     else { ValueMatrix[i-1][minimum_index+1]  = -1; ++minimum_index; }
   }
 }
}

void FillFinalMatrix(std::vector<std::vector<int> > &ValueMatrix, std::vector<std::vector<int> > &FinalMatrix) {
  bool SeamFlag = false;

  for (int i = 0; i < ValueMatrix.size(); i++) {
    for (int j = 0; j < ValueMatrix[i].size(); j++) {
     if (ValueMatrix[i][j] == -1) SeamFlag = true;
     else { FinalMatrix[i][j-SeamFlag] = ValueMatrix [i][j]; }
   }
   SeamFlag = false;
 }
}

void TransposeMatrix(std::vector<std::vector<int> > &ValueMatrix)
{
  std::vector<std::vector<int>> RotatedMatrix    (ValueMatrix[0].size(),  std::vector<int> (ValueMatrix.size(),   0  ));
  int X_WIDTH = ValueMatrix[0].size();
  int Y_HEIGHT = ValueMatrix.size();
  for(int i=0; i<Y_HEIGHT; i++) {
    for(int j=0; j<X_WIDTH; j++) {
      RotatedMatrix[j][i]=ValueMatrix[i][j];
    }
  }
}

void OutputFinalMatrix(const std::vector<std::vector<int>> &matrix, std::ofstream &output, int MAX_VALUE) {

  output << "P2\n";
  output << matrix[0].size() << " " << matrix.size() << "\n";
  output << MAX_VALUE << "\n";

  for (int i = 0; i < matrix.size(); i++) {
    for (int j = 0; j < matrix[i].size(); j++) {
      output << matrix[i][j] << " ";
    }
    if (i != matrix.size()-1) output << "\n";
  }
}
