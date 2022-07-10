#ifndef GUARD_CIRCUIT_STRUCTURE_HPP
#define GUARD_CIRCUIT_STRUCTURE_HPP

#include <vector>
#include <map>
#include <string>
#include <cassert>
#include <iostream>
#include <functional>
#include <algorithm>
#include <typeinfo>

namespace Circuit
{
	class Component;
	class Capacitor;
	class Inductor;
	class Resistor;
	class Transistor;
	class Mosfet;
	class Schematic;
	class Node;
	class Source;
	class Current;
	class Voltage;
	class Parser;
	class Simulator;
	class Math;
	class LC;
	class Diode;
	struct ParamTable;
} // namespace Circuit

struct Circuit::ParamTable
{
	std::map<std::string, double> lookup;
};

class Circuit::Schematic
{
	friend class Simulator;

public:
	enum IterationType
	{
		Newton,
		Levenberg
	};

private:
	IterationType itType = Levenberg;
	bool nonLinear = false;
	std::function<int()> createIDGenerator(int &start) const
	{
		return [&]() {
			return start++;
		};
	}
	int start = -1;
	void setupConnectionNode(Circuit::Component *linear, const std::string &node);

public:
	Schematic();
	std::vector<ParamTable *> tables;
	std::function<int()> id;
	std::string title;
	std::map<std::string, Node *> nodes;
	std::map<std::string, Component *> comps;
	std::vector<std::string> commands;
	std::vector<std::string> simulationCommands;
	std::vector<Simulator *> sims;
	std::vector<Diode *> nonLinearComps;
	void containsNonLinearComponents()
	{
		nonLinear = true;
	}
	void setupConnections2Node(Circuit::Component *linear, const std::string &nodeA, const std::string &nodeB);
	void setupConnections3Node(Circuit::Component *linear, const std::string &nodeA, const std::string &nodeB, const std::string &nodeC);
	~Schematic();
};

class Circuit::Node
{
private:
	Schematic *schem;
	std::string name;
	int id;

public:
	double voltage;
	std::vector<Component *> comps;
	Node(const std::string &name, Schematic *schem) : name(name)
	{
		id = schem->id();
		voltage = 0.0;
		this->schem = schem;
	}
	std::string getName() const
	{
		return name;
	}
	int getId() const
	{
		return id;
	}

	~Node()
	{
		assert(comps.size() == 0 && "Can't delete a Node with components");
		schem->nodes.erase(name);
	}
};

class Circuit::Component
{
protected:
	double i_prev = 0.0; //important for opreplace to have initial value of i_prev
	double value;
	Schematic *schem;
	Component() = default;
	Component(const std::string &name, std::string variableName, Schematic *schem) : schem(schem), variableDefined(true), variableName(variableName), name(name) {}
	Component(const std::string &name, double value, Schematic *schem) : value(value), schem(schem), name(name) {}
	bool variableDefined = false;
	std::string variableName;

public:
	std::string name;
	std::vector<Node *> nodes;
	virtual double getConductance(ParamTable *param, double timestep) const
	{
		assert(false && "Calling base class conductance, overload in specific component");
		return 0.0;
	};

	Node *getPosNode() const
	{
		return nodes[0];
	}
	Node *getNegNode() const
	{
		return nodes[1];
	}
	virtual double getVoltage() const
	{
		return getPosNode()->voltage - getNegNode()->voltage;
	}
	virtual double getCurrent(ParamTable *param, double time = 0, double timestep = 0, bool useNeg = false) const
	{
		return getVoltage() * getConductance(param, time);
	}

	virtual ~Component()
	{
		if (this->schem->comps.find(name) != this->schem->comps.end())
		{
			this->schem->comps.erase(name);
		}

		for (Node *n : nodes)
		{
			std::vector<Circuit::Component *>::iterator it = std::find(n->comps.begin(), n->comps.end(), this);
			if (it != n->comps.end())
			{
				n->comps.erase(it);
			}
			if (n->comps.size() == 0)
			{
				delete n;
			}
		}
	}

	virtual double getValue(ParamTable *param) const
	{
		if (variableDefined)
		{
			return param->lookup[variableName];
		}
		else
		{
			return value;
		}
	}
	virtual void setValue(ParamTable *param, double value)
	{
		if (!variableDefined)
		{
			this->value = value;
		}
		else
		{
			param->lookup[variableName] = value;
		}
	}
	virtual bool isSource() const
	{
		return false;
	}
};

void Circuit::Schematic::setupConnectionNode(Circuit::Component *linear, const std::string &node)
{

	std::map<std::string, Circuit::Node *>::iterator it = this->nodes.find(node);

	if (it == this->nodes.end())
	{
		Circuit::Node *a = new Circuit::Node(node, this);
		it = this->nodes.insert(std::pair<std::string, Circuit::Node *>(node, a)).first;
	}

	Circuit::Node *a = (*it).second;

	// into the node
	a->comps.push_back(linear);

	// this goes into the schematic
	this->comps.insert(std::pair<std::string, Circuit::Component *>(linear->name, linear));
	linear->nodes.push_back(a);
}

void Circuit::Schematic::setupConnections2Node(Circuit::Component *linear, const std::string &nodeA, const std::string &nodeB)
{
	setupConnectionNode(linear, nodeA);
	setupConnectionNode(linear, nodeB);
}
void Circuit::Schematic::setupConnections3Node(Circuit::Component *linear, const std::string &nodeA, const std::string &nodeB, const std::string &nodeC)
{
	setupConnectionNode(linear, nodeA);
	setupConnectionNode(linear, nodeB);
	setupConnectionNode(linear, nodeC);
}

Circuit::Schematic::Schematic() : id(createIDGenerator(start))
{
	Node *ground = new Node("0", this);
	nodes.insert(std::pair<std::string, Node *>(ground->getName(), ground));
}

Circuit::Schematic::~Schematic()
{
	std::for_each(tables.begin(), tables.end(),
				  [](ParamTable *&t) {
					  delete t;
				  });

	while (comps.size() != 0)
	{
		delete comps.begin()->second;
	}

	std::for_each(sims.begin(), sims.end(), [](auto kv) {
		delete kv;
	});
}
#endif
