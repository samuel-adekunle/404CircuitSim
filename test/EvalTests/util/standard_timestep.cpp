#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cassert>
namespace fs = std::filesystem;

void standardStep( std::filesystem::path filename, double timestep ){
	std::fstream f(filename);
	std::string line;
	std::string header;
	std::getline(f,header);//header
	std::vector<std::vector<double>> steps;
	while(std::getline(f, line)){
		std::stringstream stream(line);
		std::string num;
		std::vector<double> numVec;
		while(std::getline(stream, num,',')){
			numVec.push_back(std::stod(num));
		}
		steps.push_back(numVec);
	}
	double endVal = timestep*std::floor((*(std::prev(steps.end(),1)))[0]/timestep);
	std::vector<std::vector<double>> new_steps;
	int n = 1;
	for(double t = timestep; t<=endVal; t+=timestep){
		std::vector<double> new_row;
		new_row.push_back(t);
		while(steps[n][0]<t){
			n++;
		}
		double dx = steps[n][0] - steps[n-1][0];
		for(int i = 1;i<steps[n].size();i++){
			double dy = steps[n][i] - steps[n-1][i];
			double c = steps[n][i]-(dy/dx)*steps[n][0];
			double value = (dy/dx)*t+c;
			new_row.push_back(value);
		}
		new_steps.push_back(new_row);
	}
	
	std::stringstream output;
	output<<header<<std::endl;
	for(auto x : new_steps){
		bool start = false;
		for(auto y : x ){
			if(start){
				output<<",";
			}
			output<<std::to_string(y);
			start = true;
		}
		output<<"\n";

	}
	std::ofstream out((std::string)filename.parent_path()+"/new"+(std::string)filename.filename());
	out<<output.str();
	std::cout<<filename.parent_path()<<std::endl;
	out.close();
}
int main(int argc, char* argv[])
{
	std::vector<std::string> cmdlineStringArgs(&argv[0], &argv[0 + argc]);
	std::string folder;
	if(argc>1){
		folder = cmdlineStringArgs[1];
	}else{
		folder = "./MultiPhase_test";
	}
	std::cout<<fs::current_path()<<std::endl;
	folder+="/LT.csvs";
	for(auto x : fs::directory_iterator(folder))
	{
		if(((std::string)x.path().filename()).find("new") != std::string::npos){
			continue;
		}
		standardStep(x.path(),1e-7);

	}


}