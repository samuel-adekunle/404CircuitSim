// TODO - Implement Circuit Parser
// REVIEW - Maybe at some point make this all just a constructor of Schematic
#ifndef GUARD_CIRCUIT_PARSER_HPP
#define GUARD_CIRCUIT_PARSER_HPP

#include <iostream>
#include <set>
#include <cassert>
#include <sstream>
#include <algorithm>
#include <limits>
#include <cmath>
#include <regex>

class Circuit::Parser{
private:

	std::set<std::string> acceptedCommands{
			".step",
			".tran",
			".dc",
			".op"
			".model"
	};

	static double parseVal(const std::string &value ){
		std::size_t suffixPos =  value.find_first_not_of("0.123456789");
		if( suffixPos == std::string::npos ){
			return	std::stod( value );
		}
		std::string unitSuffix = value.substr(suffixPos, std::string::npos);
		int mult;
		// std::cerr<<(int)unitSuffix[0]<<" "<<unitSuffix<<std::endl;

		if (unitSuffix == "p"){
			mult = -12;
		}
		else if (unitSuffix == "n"){
			mult = -9;
		}
		else if (unitSuffix == "u" || unitSuffix == "µ"){
			mult = -6;
		}
		else if (unitSuffix == "m"){
			mult = -3;
		}
		else if (unitSuffix == "k"){
			mult = 3;
		}
		else if (unitSuffix == "Meg"){
			mult = 6;
		}
		else if (unitSuffix == "G"){
			mult = 9;
		}
		else {
			std::cerr << "Invalid Unit Suffix" << '\n';
			assert(0);
		}
		std::string num = value.substr(0, suffixPos);
		return std::stod(num) * pow(10, mult);
	}

	template <class SourceType>
	static SourceType* sourceFactory( const std::string& line, Schematic *schem ) {
		static_assert(std::is_base_of<Circuit::Source, SourceType>::value, "Only derivates of source type maybe passed into this function");

		std::regex nameNodes(R"(^(\w+) (\w+) (\w+))");
		std::smatch nameNodesM;

		std::string name;
		std::string nodePos;
		std::string nodeNeg;
		if( regex_search( line, nameNodesM, nameNodes ) ){
			name = nameNodesM.str(1);
			nodePos = nameNodesM.str(2);
			nodeNeg = nameNodesM.str(3);
		}

		double DC = 0;
		double smallSignalAmp = 0;

		double SINE_DC_offset = 0;
		double SINE_amplitude = 0;
		double SINE_frequency = 0;

		//NOTE Small Signal value
		std::regex ac("AC ([a-zA-Z.\\d]+)");
		std::smatch acM;
		if( regex_search( line, acM, ac ) ){
			smallSignalAmp = parseVal( acM.str(1) );
		}

		//DC value already variable safe although not implemented
		std::regex dc("(?:^(?:(?:[a-zA-Z.\\d]+ ){3}))([a-zA-Z.\\d]+)");
		std::smatch dcM;
		if( regex_search( line, dcM, dc ) ){
			if( std::isdigit(dcM.str(1)[0])){
				DC = parseVal( dcM.str(1) );
			}
		}

		//sine function variable safe although not implemented
		std::regex sine(R"(^(?:(?:\w+ ?){3}) (?:sine\s?\(|SINE\s?\()(?:\s?)([a-z.A-Z\d]+) ([a-z.A-Z\d]+) ([a-z.A-Z\d]+))");
		std::smatch sineFunc;
		if( regex_search( line, sineFunc, sine ) ){
			if( std::isdigit(sineFunc.str(1)[0])){
				SINE_DC_offset = parseVal( sineFunc.str(1) );
			}
			if( std::isdigit(sineFunc.str(2)[0])){
				SINE_amplitude = parseVal( sineFunc.str(2) );
			}
			if( std::isdigit(sineFunc.str(3)[0])){
				std::string val = sineFunc.str(3);
				SINE_frequency = parseVal( sineFunc.str(3) );
			}
		}

		return new SourceType(name, DC, nodePos, nodeNeg, smallSignalAmp, SINE_DC_offset , SINE_amplitude,  SINE_frequency, schem );
	}

	static void addComponent( const std::string& comp, Circuit::Schematic* schem ){

		std::stringstream ss( comp );
		std::vector<std::string> params;
		std::string param;
		while(ss>>param){
			params.push_back(param);
		}
		if(params.size() == 0 ){
			return;
		}
		std::string name = params[0];
		//NOTE has to be int for switch but basically comparing chars
		int component = (int) std::tolower( name[0] );


		switch( component ){
			case (int) 'r' : {
				//TODO Would be helpful to say which resistor has broken syntax rules
				assert( params.size() >= 4 && "Resistor - too few params" );
				std::string nodeA = params[1];
				std::string nodeB = params[2];
				if(params[3][0] == '{'){
					//Variable defined
					std::string variableName = params[3];
					variableName.pop_back();
					variableName = variableName.substr(1,variableName.size()-1);
					new Circuit::Resistor( name, variableName, nodeA, nodeB, schem );
				}
				else{
					double value = parseVal( params[3] );
					new Circuit::Resistor( name, value, nodeA, nodeB, schem );
				}

				break;
			}
			case (int) 'c' : {
				//TODO Would be helpful to say which capacitor has broken syntax rules
				assert( params.size() >= 4 && "Capacitor - too few params");
				std::string nodeA = params[1];
				std::string nodeB = params[2];
				double V_init = 0.0;
				if( params.size() >= 5 ){
					V_init = parseVal( params[4] );
				}
				if(params[3][0] == '{'){
					//Variable defined
					std::string variableName = params[3];
					variableName.pop_back();
					variableName = variableName.substr(1,variableName.size()-1);
					new Circuit::Capacitor( name, variableName, nodeA, nodeB, schem, V_init );
				}
				else{
					double value = parseVal( params[3] );
					new Circuit::Capacitor( name, value, nodeA, nodeB, schem, V_init );
				}

				break;
			}
			case (int) 'l' : {
				//TODO Would be helpful to say which indcutor has broken syntax rules
				assert( params.size() >= 4 && "Inductor - too few params");
				std::string nodeA = params[1];
				std::string nodeB = params[2];
				double value = parseVal( params[3] );
				double I_init = 0.0;
				if( params.size() >= 5 ){
					I_init = parseVal( params[4] );
				}
				if(params[3][0] == '{'){
					//Variable defined
					std::string variableName = params[3];
					variableName.pop_back();
					variableName = variableName.substr(1,variableName.size()-1);
					new Circuit::Inductor( name, variableName, nodeA, nodeB, schem, I_init );

				}
				else{
					new Circuit::Inductor( name, value, nodeA, nodeB, schem, I_init );
				}

				break;
			}
			case (int) 'v' : {
				assert( params.size() >= 4 && "Voltage - too few params" );
				sourceFactory<Circuit::Voltage>( comp, schem );


				break;
			}
			case (int) 'i' : {
				assert( params.size() >= 4 && "Voltage - too few params" );
				sourceFactory<Circuit::Current>( comp, schem );

				break;
			}
			case (int) 'd' : {
				assert( params.size() >= 4 && "Diode - too few params" );
				std::string nodeA = params[1];
				std::string nodeB = params[2];
				std::string modName = params[3];

				Circuit::Diode *diode = new Circuit::Diode( name, nodeA, nodeB, modName, schem );
				schem->containsNonLinearComponents();
				schem->nonLinearComps.push_back(diode);
				break;
			}
			case (int) 'q' : {
				// Qname C B E BJT_modelName

				assert( params.size() >= 5 );
				std::string nodeCollector = params[1];
				std::string nodeBase = params[2];
				std::string nodeEmitter = params[3];
				std::string modelName = params[4];

				new Circuit::Transistor( name, nodeCollector, nodeBase, nodeEmitter, modelName, schem );
				schem->containsNonLinearComponents();
				break;
			}
			case (int) 'm' : {
				std::cout<<"not implemented yet"<<std::endl;
				break;
			}
			case (int) 'e' :{
				std::cout<<"not implemented yet"<<std::endl;
			}
			case (int) 'g' : {
				std::cout<<"not implemented yet"<<std::endl;
				break;
			}
			case (int) 'h' : {
				std::cout<<"not implemented yet"<<std::endl;
				break;
			}
			case (int) 'f' : {
				std::cout<<"not implemented yet"<<std::endl;
				break;
			}
			default : {
				break;
			}
		}

	}

	static void parseCommand( const std::string& cmd, Circuit::Schematic* schem, std::map<std::string, std::vector<double>> *tableGenerator, bool& stepped ){
		schem->simulationCommands.push_back( cmd );
		std::stringstream ss( cmd );
		std::vector<std::string> params;
		std::string param;
		while(ss>>param){
			params.push_back(param);
		}

		std::transform(params[0].begin()+1, params[0].end(), params[0].begin()+1, ::toupper);
		if( params[0] == ".STEP" ){
			stepped = true;
			if(params[1] == "oct" ){
				std::string variableName = params[3];
				double startVal = parseVal(params[4]);
				double endVal = parseVal(params[5]);
				double pointsPerOctave = parseVal(params[6]);
				double num_octaves = (log2((endVal/startVal)));
				std::vector<double> v(pointsPerOctave * num_octaves);
				std::generate(v.begin(), v.end(), [startVal, pointsPerOctave, n=0] () mutable {
					return startVal*pow(2,n++/pointsPerOctave);
				});
				(*tableGenerator)[variableName] = v;
			}
			else if(params[1] == "dec" ){
				std::string variableName = params[3];
				double startVal = parseVal(params[4]);
				double endVal = parseVal(params[5]);
				double pointsPerDecade = parseVal(params[6]);
				double num_decades = (log10((endVal/startVal)));
				std::vector<double> v(pointsPerDecade * num_decades);
				std::generate(v.begin(), v.end(), [startVal, pointsPerDecade, n=0] () mutable {
					return startVal*pow(10,n++/pointsPerDecade);
				});
				(*tableGenerator)[variableName] = v;
			}
			else if(params[3] == "list" ){
				std::string variableName = params[2];
				std::vector<double> v;
				std::for_each(params.begin()+4, params.end(),[&v](const std::string &a){
					double val = parseVal(a);
					v.push_back(val);
				});
				(*tableGenerator)[variableName] = v;
			}
			else{
				std::string variableName = params[2];
				double startVal = parseVal(params[3]);
				double endVal = parseVal(params[4]);
				double increment = parseVal(params[5]);
				assert(endVal > startVal && "End value smaller than start value");
				std::vector<double> v((endVal - startVal)/increment);
				std::generate(v.begin(), v.end(), [startVal, increment, n=0] () mutable {
					return startVal+increment*n++;
				});
				(*tableGenerator)[variableName] = v;
			}
		}
		else if( params[0] == ".TRAN"){
			// std::cerr<<"tran"<<std::endl;
			assert(params.size() == 5 && "Incorrect number of parameters in transient command");
			double stop = parseVal( params[2] );
			double start = parseVal( params[3] );
			double step = parseVal( params[4] );

			// std::cerr<<stop<<" "<<start<<" "<<step<<std::endl;
			schem->sims.push_back(new Simulator(schem, Circuit::Simulator::SimulationType::TRAN, stop, start, step));
			schem->simulationCommands.push_back( cmd );
		}
		else if( params[0] == ".OP"){
			schem->sims.push_back(new Simulator(schem, Circuit::Simulator::SimulationType::OP));
		}

	}
	using TableIt = std::map<std::string, std::vector<double>>::const_iterator;
	static std::vector<ParamTable *> buildParam(TableIt it,const TableIt end){
		std::vector<ParamTable *> tables;
		//REVIEW Need to change auto
		std::for_each( it->second.begin(), it->second.end(), [&tables, &it, &end](const auto& x){
			if(it==std::prev(end,1)){
				ParamTable* p = new ParamTable();
				p->lookup[it->first] = x;
				tables.push_back(p);
			}
			else{
				std::vector<ParamTable *> toAdd = buildParam(std::next(it,1),end);
				std::for_each( toAdd.begin(), toAdd.end(), [&it, &x](auto& a){
					a->lookup[it->first] = x;
				});
				tables.insert(tables.end(), toAdd.begin(), toAdd.end());
			}
		});
		return tables;

	}
	static std::vector<ParamTable *> paramGenerator(std::map<std::string, std::vector<double>> values){
		TableIt varOne = values.begin();
		const TableIt varEnd = values.end();
		std::vector<ParamTable *> tables;

		std::vector<ParamTable *> recursiveAdd = buildParam(varOne, varEnd);
		tables.insert(tables.end(), recursiveAdd.begin(), recursiveAdd.end());

		return tables;
	}
public:

	static Circuit::Schematic* parse( std::istream& inputStream ){
		//NOTE
		//Refer to
		//https://web.stanford.edu/class/ee133/handouts/general/spice_ref.pdf for
		//sytnax of SPICE files


		std::string inputLine;
		std::map<std::string, std::vector<double>> tableGenerator;
		Circuit::Schematic *schem = new Schematic();
		if( std::getline( inputStream, inputLine ) ){
			schem->title = inputLine;
		}
		bool endStatement = false;
		bool stepped = false;
		while( std::getline( inputStream, inputLine )){
			if( inputLine == ".END" || inputLine == ".end" ){
				endStatement = true;
				break;
			}
			if( inputLine[0] != '*' || inputLine[0] != '.'){
				addComponent( inputLine, schem );
			}
			if( inputLine[0] == '.'){
				parseCommand(inputLine, schem, &tableGenerator, stepped);
			}
		}
		if(stepped){
			schem->tables = paramGenerator(tableGenerator);
		}
		else{
			ParamTable *param = new ParamTable();
			schem->tables.push_back( param );
		}
		assert( endStatement && "No end statement present in netlist");
		return schem;
	}
};


#endif
