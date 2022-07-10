#ifndef GUARD_CIRCUIT_SOURCE_HPP
#define GUARD_CIRCUIT_SOURCE_HPP
#include <cmath>
class Circuit::Source : public Circuit::Component
{
protected:
	double DC = 0;
	double smallSignalAmp = 0;

	double SINE_DC_offset = 0;
	double SINE_amplitude = 0;
	double SINE_frequency = 0;
	Source() = default;
	Source(const std::string &name, double DC, double smallSignalAmp, double SINE_DC_offset, double SINE_amplitude, double SINE_frequency, Schematic *schem) : Component(name, DC, schem)
	{

		this->name = name;
		this->DC = DC;
		this->smallSignalAmp = smallSignalAmp;
		this->SINE_DC_offset = SINE_DC_offset;
		this->SINE_amplitude = SINE_amplitude;
		this->SINE_frequency = SINE_frequency;
	}
	Source(const std::string &name, double value, const Circuit::Node *pos, const Circuit::Node *neg, Schematic *schem) : Source(name, value, 0, 0, 0, 0, schem)
	{
	}

public:
	double getSourceOutput(ParamTable *param, double t) const
	{
		return (DC + SINE_DC_offset + (SINE_amplitude)*std::sin(2.0 * M_PI * SINE_frequency * t));
	}
	bool isSource() const override
	{
		return true;
	}
	virtual bool isCurrent() const = 0;
};

class Circuit::Current : public Circuit::Source
{
	friend class Capacitor;

private:
	//Note to be used by
	Current(Schematic *schem)
	{
		this->DC = 0;
		this->schem = schem;
	}

public:
	Current(const std::string &name, double DC, const std::string &nodePos, const std::string &nodeNeg, double smallSignalAmp, double SINE_DC_offset, double SINE_amplitude, double SINE_frequency, Schematic *schem) : Source(name, DC, smallSignalAmp, SINE_DC_offset, SINE_amplitude, SINE_frequency, schem)
	{
		//NOTE for some reason swapped for current sources order - because of error in specification file
		schem->setupConnections2Node(this, nodeNeg, nodePos);
	}
	Current(const std::string &name, double DC, const std::string &nodePos, const std::string &nodeNeg, Schematic *schem) : Current(name, DC, nodePos, nodeNeg, 0, 0, 0, 0, schem)
	{
	}
	bool isCurrent() const override
	{
		return true;
	}
	double getCurrent(ParamTable *param, double t, double timestep = 0, bool useNeg = false) const override
	{
		return getSourceOutput(param, t);
	}
};

class Circuit::Voltage : public Circuit::Source
{
	friend class Inductor;

private:
	Voltage(Schematic *schem)
	{
		DC = 0;
		this->schem = schem;
	}

public:
	Voltage(const std::string &name, double DC, const std::string &nodePos, const std::string &nodeNeg, double smallSignalAmp, double SINE_DC_offset, double SINE_amplitude, double SINE_frequency, Schematic *schem) : Source(name, DC, smallSignalAmp, SINE_DC_offset, SINE_amplitude, SINE_frequency, schem)
	{
		schem->setupConnections2Node(this, nodePos, nodeNeg);
	}
	Voltage(const std::string &name, double DC, const std::string &nodePos, const std::string &nodeNeg, Schematic *schem) : Voltage(name, DC, nodePos, nodeNeg, 0, 0, 0, 0, schem)
	{
	}
	bool isCurrent() const override
	{
		return false;
	}
	double getCurrent(ParamTable *param, double t, double timestep = 0, bool useNeg = false) const override
	{
		double current = 0;
		double subCurrent = 0;
		Node *refNode = getPosNode()->getId() != -1 ? getPosNode() : getNegNode();
		if (useNeg)
		{
			if (refNode == getPosNode() && getNegNode()->getId() != -1)
			{
				refNode = getNegNode();
			}
		}

		Node *otherNode = *std::find_if(nodes.begin(), nodes.end(), [&](Node *n) {
			return n != refNode;
		});

		for (Component *comp : refNode->comps)
		{
			if (comp != this)
			{
				if (Voltage *source = dynamic_cast<Voltage *>(comp))
				{
					if (source->getPosNode() == refNode)
					{
						if (source->getNegNode() == otherNode)
						{
							std::cerr << "connecting voltage sources in parralel leads to a overdefined matrix. abort!!!" << std::endl;
							exit(1);
						}
						subCurrent = comp->getCurrent(param, t,timestep, true);
					}
					else
					{
						if (source->getPosNode() == otherNode)
						{
							std::cerr << "connecting voltage sources in parralel leads to a overdefined matrix. abort!!!" << std::endl;
							exit(1);
						}
						subCurrent = comp->getCurrent(param, t, timestep);
					}
				}
				else
				{
					subCurrent = comp->getCurrent(param, t, timestep);
				}

				if (refNode == comp->getPosNode())
				{
					current -= subCurrent;
				}
				else
				{
					current += subCurrent;
				}
			}
		}
		return current;
	}
};

#endif
