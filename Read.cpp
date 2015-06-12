#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <malloc.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <iterator>
#include <opengm/graphicalmodel/graphicalmodel.hxx>
#include <opengm/graphicalmodel/space/simplediscretespace.hxx>
#include <opengm/functions/potts.hxx>
#include <opengm/operations/adder.hxx>
#include <opengm/inference/messagepassing/messagepassing.hxx>
#include <opengm/inference/gibbs.hxx>
#include "settings.h"
#include <dirent.h>

// Define filename etc
//#define filename "/home/bernat/Desktop/Projecte/FEATS/0001TP_006690.boosted.txt"
#define rows 880
#define cols 6
#define rowsSeg 720
#define colsSeg 960
using namespace std; // 'using' is used only in example code
using namespace opengm;

// model parameters (global variables are used only in example code)
const size_t nx = 1; // width of the grid
const size_t ny = 880; // height of the grid
const size_t numberOfLabels = 6;
double lambda = 0.1; // coupling strength of the Potts model
struct difference
{
	int origin;
	int objective;
};

// this function maps a node (x, y) in the grid to a unique variable index
inline size_t variableIndex(const size_t x, const size_t y) { 
   return x + nx * y; 
}
//Function that sorts vector to let me delete repeted values
bool sortByNumber(const difference &lhs, const difference &rhs) { 


	if(lhs.origin==rhs.origin && lhs.objective == rhs.objective){
		return true;

	}else{
		return false;
	}

}
bool sortByNumber2(const difference &lhs, const difference &rhs) { 


return(lhs.origin<rhs.origin);

}

//bool sortByNumber2(const difference &lhs, const difference &rhs) { return lhs.objective< rhs.objective;}
std::vector<std::string> readFile(std::string filename)
{
    std::string line;
    std::vector<std::string> lines;
    ifstream fin;
	//std::cout << "filename"<< filename<<std::endl;
    fin.open(filename);
    if(fin.is_open())
    {
    	//std::cout << "obert";
        while(std::getline(fin,line))
        {
            lines.push_back(line);

        }
    }

    return lines;
}

bool checkExtension(std::string &file_name, std::string extension)
{
    return (file_name.rfind(extension)!=std::string::npos);
}

std::vector<std::string> readFolder(std::string folder_name,std::string extension)
{
    std::vector<std::string> filenames;
    DIR *directory;
    struct dirent *directory_entry;
    directory = opendir(folder_name.c_str());

    if(directory!=NULL)
    {
        while( (directory_entry = readdir (directory))!=NULL)
        {
            string file_name = directory_entry->d_name;

            if(checkExtension(file_name, extension))
                filenames.push_back(directory_entry->d_name);
        }

    }
    return filenames;



}


std::vector<float> parseLine(std::string &line)
{
    std::istringstream iss(line);
    std::vector<float> v;
    std::copy(std::istream_iterator<float>(iss),
              std::istream_iterator<float>(),
              std::back_inserter(v));

    return v;
}

std::vector<std::vector<float>> parseStringFile(std::vector<std::string> &lines)
{
    std::vector<std::vector<float>> vals;
    for(int i=0;i<lines.size();i++)
        vals.push_back(parseLine(lines[i]));


    return vals;
}


main(){
    //readFile();
    //float data[880][6];

    //----------------------Reading DATA SEGS---------------------------------
    std::vector<std::string> filesSeg = readFolder(folder_name2,".seg");
    //std::cout<<"Numberfiles"<<filesSeg.size();
    //std::cout<<std::endl;
    int file_seg=1;

       for(int i =0;i<filesSeg.size();i++) {
           //std::cout<<filesSeg[i]<<std::endl;
       }
    std::string filenameseg = folder_name + filesSeg[file_seg];
    //std::cout<<"name of the first file"<<filesSeg[file_seg]<<std::endl;
    //std::vector<std::string> linesseg = readFile(filenameseg); Not working because of the .seg format.
    std::string filenametest = "/home/bernat/Desktop/Projecte/0001TP_006720.txt";
    std::vector<std::string> linesseg = readFile(filenametest);
    //std::cout << "lineseg"<< linesseg;
    std::vector<std::vector<float> > dataseg = parseStringFile(linesseg);
    std::cout << "rows"<< dataseg.size()<<std::endl; //720
    std::cout << "cols"<< dataseg[1].size()<<std::endl; //960
     //for (int i =0; i < dataseg.size(); i++) {


    //------------------------Extracting conections/contourns----------
    const int dx[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
    const int dy[8] = { 0, -1, -1, -1, 0, 1, 1, 1 };
    int pos,obj;
    bool should_push;
    vector <difference> diff;
    vector <difference> Vedge;
    diff.push_back(difference());
    int ant = 0;
    vector<vector<bool> > taken;
    should_push=false;
    for ( int y = 0; y <rowsSeg; y++ ){
        for (int x = 0; x < colsSeg; x++){
        		//std::cout<<dataseg[y][x];

            for (int k = 0; k<8; k++){
                    int nx = x + dx[k];
                    int ny = y + dy[k];
                    //ant = ant +1;
                    if (nx >= 0 && ny >= 0 && nx < colsSeg && ny < rowsSeg){
                    	if (dataseg[y][x]!= dataseg[ny][nx]) {
                    		pos = dataseg[y][x];
                    		obj = dataseg[ny][nx];
                    		//should_push = true;
                    		diff.push_back ({pos, obj});
                    	}
                    	/*if (should_push){
                    		if (diff.size() == 1){
                    			diff[0].origin=pos;
                    			diff[0].objective= obj;
                    		}else{
                    			diff.push_back ({pos, obj});
                    		}
                    	}*/

                    }
                }
            }
        }

	//sort(diff.begin(),diff.end(),sortByNumber);
	sort(diff.begin(),diff.end(),sortByNumber);

	auto comp = [](const difference& lhs, const difference& rhs ){return lhs.origin == rhs.origin && lhs.objective == rhs.objective ;};
	auto last = std::unique(diff.begin(), diff.end(),comp);
	diff.erase(last,diff.end());
	std::cout<<diff.size();
	//sort(diff.begin(),diff.end(),sortByNumber2);
	//diff.erase(last,diff.end());
	sort(diff.begin(),diff.end(),sortByNumber2);
    for (int i = 0; i<15;i++){
        	std::cout<<"origen"<<diff[i].origin;
        	std::cout<<"obj"<<diff[i].objective<<std::endl;
    }

	//--------------READING DATA Boosted -------------------------------
    std::vector<std::string> files = readFolder(folder_name,".boosted.txt");

    for(int i =0;i<files.size();i++) {
        //std::cout<<files[i]<<std::endl;
    }
	//std::cout << "arriba";
    int file_nr=1;
    //for (file_nr = 0;file_nr<files_to_train;file_nr++)


    std::string filename = folder_name + files[file_nr];

    std::vector<std::string> lines = readFile(filename);
    std::vector<std::vector<float>> data = parseStringFile(lines);
    for (int i =0; i < 880; i++) {

        for (int j = 0; j<6 ;j++) {

        //std::cout <<" "<< data[i][j];
       // return 0;
        }
        //std::cout<<std::endl;
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

   //------------uNARY ENERGIES------------
   //Change limits
   double w0,w1,w2,w3,w4,w5 = 1; //Node weights
   double wa,wb  = 1; //Edge weights
   
   for(size_t y = 0; y < ny; ++y) 
   { //Restored this bucle, i think it's ned for the execution
   for(size_t x = 0; x < nx; ++x) {
      // function
      const size_t shape[] = {numberOfLabels}; //numberOFLabels = 6
      ExplicitFunction<double> f(shape, shape + 1);
      
      //std::cout<<rnd<<","<<rnd_flag<<std::endl;
      
      //i -> 0 to 880
      //j-> 0 - 6
        f(0) = data[y][0] * w0;
        f(1) = data[y][1] * w1;
        f(2) = data[y][2] * w2 ;
      	f(3) = data[y][3] * w3;
      	f(4) = data[y][4] * w4;
      	f(5) = data[y][5] * w5;
      
     
       Model::FunctionIdentifier fid = gm.addFunction(f);
     // std::cout<<f(0)<<","<<f(1)<<"  ";
      // factor
       //---------------------------MODIFIED------------------------------------
      size_t variableIndices[] = {variableIndex(y,0)}; //changed variableIndex(x,0) to variableIndex(y,0)
      gm.addFactor(fid, variableIndices, variableIndices + 1); 

      }

     
   }
   //std::cout<<std::endl;


   //---PAIRWISE ENERGY----------

   //Defining Potts, and doing nothing yet with it
   // add one (!) 2nd order Potts function
   //0 energy when same, lambda energy when different.
   PottsFunction<double> f(numberOfLabels, numberOfLabels, 0.0*wa, lambda*wb);
   Model::FunctionIdentifier fid = gm.addFunction(f);

   // for each pair of nodes (x1, y1), (x2, y2) which are adjacent on the grid,
   // add one factor that connects the corresponding variable indices and 
   // refers to the Potts function

//----------------------------MODIFIED--------------------------------
   //Change limits
   for(size_t x = 0; x < nx; ++x) { //Should this iterate through Y? if done, seems to do 880 steps, and also get even more 5 classification
      if(x + 1 < nx) { // (x, y) -- (x + 1, y)
         size_t variableIndices[] = {variableIndex(x,0), variableIndex(x + 1,0)};
         sort(variableIndices, variableIndices + 2);
         gm.addFactor(fid, variableIndices, variableIndices + 2);
      }
      
   }   



   //Once model is defined
   //-----INFERENCE---------
   typedef BeliefPropagationUpdateRules<Model, opengm::Minimizer> UpdateRules;
   typedef MessagePassing<Model, opengm::Minimizer, UpdateRules, opengm::MaxDistance> BeliefPropagation;
   const size_t maxNumberOfIterations = 40;
   const double convergenceBound = 1e-7;
   const double damping = 0.5;
   BeliefPropagation::Parameter parameter(maxNumberOfIterations, convergenceBound, damping);
   //End of defining inference method

   //Telling inference method to connect to this graphical model
   BeliefPropagation bp(gm, parameter);
   
   // optimize (approximately)
   BeliefPropagation::VerboseVisitorType visitor;
   bp.infer(visitor); //Actual Optimization


   // obtain the (approximate) argmin
   vector<size_t> labeling(nx * ny);
   bp.arg(labeling); //gives you best labelling : or result of inference.

   /* Labelling = for example
   *
   * 1 --- 2 ---1 --- 3--- 1 ..... so on

   */
   // output the (approximate) argmin
   size_t variableIndex = 0;
   for(size_t y = 0; y < ny; ++y) {
      for(size_t x = 0; x < nx; ++x) {
         //cout << labeling[variableIndex] << ' ';
         ++variableIndex;
      }   
     // cout << endl;
   }





}
