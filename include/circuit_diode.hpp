#ifndef GUARD_DIODE_HPP
#define GUARD_DIODE_HPP
#include <cmath>

class Circuit::Diode : public Circuit::Component
{
private:
	std::string modelName = "D";
	double inst_conductance = 0;
	double IS = 1e-14; //also stored in value (Component base class)
	double RS = 0;
	double CJ0 = 1e-14;
	double TT = 0;
	double BV = 100;
	double IBV = 0.1e-10;
	double VJ = 1;
	const double GMIN = 1e-5;
	const double V_T = 25e-3;

public:
	class ParasiticCapacitance : public Circuit::Capacitor
	{
	public:
		ParasiticCapacitance(Schematic *schem)
		{
			this->schem = schem;
		}
		void setDiodeOwner(Diode *d);
		void setCap(double vGuess, const double &CJ0, const double &VJ)
		{
			if (vGuess <= 1.0)
			{
				value = CJ0 / pow(1.0 - vGuess / VJ, 0.5);
			}
			else
			{
				value = 0;
			}

		}
		void setNodes(Node *pos, Node *neg)
		{
			this->nodes.push_back(pos);
			this->nodes.push_back(neg);
		}

		virtual ~ParasiticCapacitance() {}
	};

	ParasiticCapacitance *para_cap;
	Diode() = default;

	Diode(std::string name, std::string nodeA, std::string nodeB, std::string model, Schematic *schem) : Circuit::Component(name, 0.0, schem)
	{
		para_cap = new ParasiticCapacitance(schem);
		schem->setupConnections2Node(this, nodeA, nodeB);
		para_cap->setNodes(this->getPosNode(), this->getNegNode());
	}
	void assignModel(std::vector<std::string> params)
	{
		//NOTE Remember to update component value!
		//REVIEW Maybe allow params to be variable dependent

		assert(params.size() == 6 && "Incorrect number of diode params");
		IS = stod(params[0]);
		RS = stod(params[1]);
		CJ0 = stod(params[2]);
		TT = stod(params[3]);
		BV = stod(params[4]);
		IBV = stod(params[5]);

		value = IS;
	}

	double getCurrentSource(ParamTable *param, double timestep)
	{
		double capacitorCurrent = para_cap->getCurrentSource(param, timestep);
		if (std::isnan(capacitorCurrent))
		{
			capacitorCurrent = 0;
		}
		return capacitorCurrent;
	}

	double getCurrent(ParamTable *param, double time, double timestep, bool useNeg = false) const override
	{
		return getVoltage() * parallelAdd(1.0 / 100.0, (GMIN + inst_conductance))+ para_cap->getCurrent(param, time, timestep, useNeg);
	}
	std::string getModelName()
	{
		return modelName;
	}
	~Diode()
	{
		delete para_cap;
	}
	double parallelAdd(const double &a, const double &b) const
	{
		double result = 1.0 / a + 1.0 / b;
		result = 1 / result;
		if (std::isnan(result))
		{
			return 0;
		}
		if (!std::isfinite(result))
		{
			return 1e30;
		}
		else
		{
			return result;
		}
	}
	double getConductance(ParamTable *param, double timestep) const override
	{
		double vPrev = getVoltage();
		para_cap->setCap(vPrev, this->CJ0, this->VJ);
		double capConductance = para_cap->getConductance(param, timestep);
		return parallelAdd(1.0 / 100.0, (GMIN + inst_conductance)) + capConductance;
	}
	void setConductance(ParamTable *param, double timestep, double vGuess)
	{
		double shockley;
		shockley = IS * (exp(vGuess / (V_T)) - 1);
		if (vGuess != 0 && !std::isnan(shockley))
		{
			i_prev = shockley;
			inst_conductance = shockley / vGuess;
		}
		else
		{
			i_prev = 0;
			inst_conductance = 0;
		}
	}
};
#endif
