#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <map>
#include <regex>
#include <cassert>
namespace fs = std::filesystem;


void writeFile( const std::string& folder, const std::stringstream &save, const std::vector<std::string> &variables )
{
	std::string fileName = folder;
	for(auto x: variables){
		fileName += x + ",";
	}
	std::cout<<fileName<<std::endl;

	fileName+=".csv";
	std::ofstream outFile;
	outFile.open(fileName);
	outFile<<save.str();
	outFile.close();
}
bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}
void separate(const std::string &folder, const std::string &fileName){
	fs::create_directory(folder+"/"+fileName+"s");
	std::fstream e404(folder+"/"+fileName);

	std::string line;
	std::string header;
	
	std::vector<std::string> variableNames;
	std::stringstream currentSave;

	bool firstLoop = true;
	std::getline(e404, header);
	while(replace(header,"\t",","));
	while(std::getline(e404, line))
	{
		if( line.find("Step Information") !=std::string::npos )
		{
			if(!firstLoop){
				writeFile( folder+"/"+fileName+"s/", currentSave, variableNames );
			}
			variableNames.clear();
			std::regex vars(R"((\w+)(?:=)(\d+))");
			std::sregex_iterator it(line.begin(), line.end(), vars);
			
			while(it!=std::sregex_iterator())
			{
				variableNames.push_back(it->str());
				it++;
			}
			currentSave.str(std::string());
			currentSave<<header<<"\n";
		}
		else{
			while(replace(line,"\t",","));
			currentSave<<line<<"\n";
			firstLoop = false;
		}
	}
	writeFile(folder+"/"+fileName+"s/" ,currentSave, variableNames);
}
int main(int argc, char* argv[])
{
	std::vector<std::string> cmdlineStringArgs(&argv[0], &argv[0 + argc]);
	std::string folder = cmdlineStringArgs[1];
	
	separate(folder, "LT.csv");
	separate(folder, "out.csv");

}