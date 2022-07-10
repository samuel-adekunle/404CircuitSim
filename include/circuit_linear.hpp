#ifndef GUARD_CIRCUIT_LINEAR_HPP
#define GUARD_CIRCUIT_LINEAR_HPP

#include <limits>

class Circuit::LC : public Circuit::Component
{
protected:
    LC() = default;
    LC(const std::string &name, std::string variableName, Circuit::Schematic *schem) : Component(name, variableName, schem) {}
    LC(const std::string &name, double value, Circuit::Schematic *schem) : Component(name, value, schem) {}

public:
    virtual double getCurrentSource(ParamTable *param, double timestep) = 0;

    double getCurrent(ParamTable *param, double time, double timestep, bool useNeg = false) const override
    {
        return (getVoltage()) * getConductance(param, timestep) - i_prev;
    }
    virtual ~LC(){};
};

class Circuit::Capacitor : public Circuit::LC
{
protected:
    Capacitor() = default;
    double DC_init;
    Current *opReplace = nullptr;

public:
    Capacitor(const std::string &name, std::string variableName, const std::string &nodeA, const std::string &nodeB, Schematic *schem, double DC_init = 0) : LC(name, variableName, schem)
    {
        schem->setupConnections2Node(this, nodeA, nodeB);
        opReplace = new Current(schem);
        this->DC_init = DC_init;
    }
    Capacitor(const std::string &name, double value, const std::string &nodeA, const std::string &nodeB, Schematic *schem, double DC_init = 0) : LC(name, value, schem)
    {
        schem->setupConnections2Node(this, nodeA, nodeB);
        opReplace = new Current(schem);
        this->DC_init = DC_init;
    }
    Current *getOpReplace()
    {
        return opReplace;
    }
    virtual double getConductance(ParamTable *param, double timestep) const override
    {
        if (timestep <= 0)
        {
            return 1e13; //Max conductance
        }
        return value / timestep;
    }

    virtual double getCurrentSource(ParamTable *param, double timestep) override
    {
        double i_pres = getConductance(param, timestep) * getVoltage();
        i_prev = i_pres;
        return i_pres;
    }
    virtual ~Capacitor()
    {
        if(opReplace){
            delete opReplace;
        }
    }
};

class Circuit::Inductor : public Circuit::LC
{
protected:
    double I_init;
    Voltage *opReplace = nullptr;

public:
    Inductor(const std::string &name, std::string variableName, const std::string &nodeA, const std::string &nodeB, Schematic *schem, double I_init = 0) : LC(name, variableName, schem)
    {
        schem->setupConnections2Node(this, nodeA, nodeB);
        opReplace = new Voltage(schem);
    }
    Inductor(const std::string &name, double value, const std::string &nodeA, const std::string &nodeB, Schematic *schem, double I_init = 0) : LC(name, value, schem)
    {
        schem->setupConnections2Node(this, nodeA, nodeB);
        opReplace = new Voltage(schem);
        this->I_init = I_init;
    }
    Voltage *getOpReplace()
    {
        return opReplace;
    }
    double getConductance(ParamTable *param, double timestep) const override
    {
        if (timestep <= 0)
        {
            return 1e-13; //Min Conductance
        }
        return timestep / value;
    }

    double getCurrentSource(ParamTable *param, double timestep) override
    {
        double i_pres = i_prev - getConductance(param, timestep) * getVoltage();
        i_prev = i_pres;
        return i_pres;
    }
    virtual ~Inductor()
    {
        delete opReplace;
    };
};

class Circuit::Resistor : public Component
{
public:
    Resistor(const std::string &name, std::string variableName, const std::string &nodeA, const std::string &nodeB, Schematic *schem) : Component(name, variableName, schem)
    {
        schem->setupConnections2Node(this, nodeA, nodeB);
    }
    Resistor(const std::string &name, double value, const std::string &nodeA, const std::string &nodeB, Schematic *schem) : Component(name, value, schem)
    {
        schem->setupConnections2Node(this, nodeA, nodeB);
    }
    double getConductance(ParamTable *param, double time = 0) const override
    {
        return 1.0 / getValue(param);
    }
};

#endif
