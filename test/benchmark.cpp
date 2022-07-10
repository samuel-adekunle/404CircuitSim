#include <hayai.hpp>
#include <circuit.hpp>
#include <vector>
#include <fstream>

size_t i;

class SimulatorFixture
    : public ::hayai::Fixture
{

public:
    SimulatorFixture()
    {
    }
    virtual void SetUp()
    {
        netlist.open("benchmarking/netlist" + std::to_string(i) + ".cir");
        this->schem = Circuit::Parser::parse(netlist);
    }

    virtual void TearDown()
    {
        netlist.close();
        delete this->schem;
    }

    std::ifstream netlist;
    std::stringstream ss;
    Circuit::Schematic *schem;
    static const size_t iterations = 1;
    static const size_t runs = 50;
};

BENCHMARK_F(SimulatorFixture, run, runs, iterations)
{
    for (Circuit::Simulator *sim : schem->sims)
    {
        sim->run(ss, Circuit::Simulator::OutputFormat::CSV);
    }
}

int main(int argc, char const *argv[])
{
    hayai::ConsoleOutputter consoleOutputter;
    hayai::Benchmarker::AddOutputter(consoleOutputter);
    for (i = 50; i <= 1000; i += 50)
    {
        std::cout << i << std::endl;
        std::cerr << i << std::endl;
        hayai::Benchmarker::RunAllTests();
    }
    return 0;
}
