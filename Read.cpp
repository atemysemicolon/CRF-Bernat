#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <malloc.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <opengm/graphicalmodel/graphicalmodel.hxx>
#include <opengm/graphicalmodel/space/simplediscretespace.hxx>
#include <opengm/functions/potts.hxx>
#include <opengm/operations/adder.hxx>
#include <opengm/inference/messagepassing/messagepassing.hxx>
#include <opengm/inference/gibbs.hxx>

// Define filename etc
#define filename "/home/bernat/Desktop/Projecte/FEATS/0001TP_006690.boosted.txt"
#define rows 880
#define cols 6
using namespace std; // 'using' is used only in example code
using namespace opengm;

// model parameters (global variables are used only in example code)
const size_t nx = 1; // width of the grid
const size_t ny = 880; // height of the grid
const size_t numberOfLabels = 6;
double lambda = 0.1; // coupling strength of the Potts model

// this function maps a node (x, y) in the grid to a unique variable index
inline size_t variableIndex(const size_t x, const size_t y) { 
   return x + nx * y; 
}


main(){
    //readFile();
    float data[880][6];
    std::ifstream file("/home/bernat/Desktop/Projecte/FEATS/0001TP_006690.boosted.txt");

    for(int row = 0; row < 880; ++row) //Saving the data input into a array data.
    {
        std::string line;
        std::getline(file, line);
        if ( !file.good() )
            break;

        std::stringstream iss(line);

        for (int col = 0; col < 6; ++col)
        {
            std::string val;
            std::getline(iss, val, ' ');
            if ( !iss.good() )
                break;

            std::stringstream convertor(val);
            convertor >> data[row][col];
        }
    } //Printing the data
    for (int i =0; i < 880; i++) {

        for (int j = 0; j<6 ;j++) {

        std::cout <<" "<< data[i][j];
       // return 0;
        }
    }
   typedef SimpleDiscreteSpace<size_t, size_t> Space; //We generate the DiscreteSpace [880]
   Space space(nx * ny, numberOfLabels);//([880],6)

   // construct a graphical model with 
   // - addition as the operation (template parameter Adder)
   // - support for Potts functions (template parameter PottsFunction<double>)
   typedef GraphicalModel<double, Adder, OPENGM_TYPELIST_2(ExplicitFunction<double> , PottsFunction<double> ) , Space> Model;
   Model gm(space);
   // for each node (x, y) in the grid, i.e. for each variable 
   // variableIndex(x, y) of the model, add one 1st order functions 
   // and one 1st order factor


//My ExplicitFunction Put values into function?? factor*

   for(size_t y = 0; y < ny; ++y) 
   {
   for(size_t x = 0; x < nx; ++x) {
      // function
      const size_t shape[] = {numberOfLabels};
      ExplicitFunction<double> f(shape, shape + 1);
      double rnd = (double)rand()/RAND_MAX;
      bool rnd_flag = (rnd>0.);
      //std::cout<<rnd<<","<<rnd_flag<<std::endl;
      if(x<(nx/2))
      {
        f(0) = 0.9;
        f(1) = 0.1;
        if(rnd_flag)
        {
            f(0) = 0.1;
            f(1) = 0.9;
        }
      }
      if(x>=(nx/2))
      {
        f(1) = 0.9;
        f(0) = 0.1;
        if(rnd_flag)
        {
            f(1) = 0.1;
            f(0) = 0.9;
        }

      }

      Model::FunctionIdentifier fid = gm.addFunction(f);
      std::cout<<f(0)<<","<<f(1)<<"  ";
      // factor
      size_t variableIndices[] = {variableIndex(x, y)};
      gm.addFactor(fid, variableIndices, variableIndices + 1);
   }
   //std::cout<<std::endl;
   }

}