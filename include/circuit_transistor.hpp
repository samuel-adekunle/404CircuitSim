#ifndef GUARD_CIRCUIT_TRANSISTOR_HPP
#define GUARD_CIRCUIT_TRANSISTOR_HPP

class Circuit::Transistor : public Component {
private:
	std::string modelName;
	enum TType{
		NPN, PNP
	};

public:
	//REVIEW will probably have to make these doubles
	double BF=100;
	double IS=1e-16;
	double VAF=std::numeric_limits<double>::max();
	bool model_CE_resistance = false;


	TType transistorType;
	Transistor( std::string name, std::string nodeCollector, std::string nodeBase, std::string nodeEmitter, std::string model, Schematic* schem) : Component( name, 0.0, schem ){
		if( model == "NPN" ){
			transistorType = NPN;
		}
		if( model == "PNP" ){
			transistorType = NPN;
		}
		modelName = model;
		schem->setupConnections3Node( this, nodeCollector, nodeBase, nodeEmitter );
	}

	//REVIEW Model not setup
	void assignModel( std::vector<std::string> params ){
		//NOTE Remember to update component value!
		//REVIEW Maybe allow params to be variable dependent

		assert( params.size() == 6 && "Incorrect number of diode params" );
		value = IS;
	}

	std::string getModelName(){
		return modelName;
	}

	virtual double getValue(ParamTable * param) const override{
		assert( false && "No value associated with transistor");
		return 0;
	}
};

#endif
