#include <iostream>
#include <fstream>
#include <circuit.hpp>
#include <filesystem>
#include <getopt.h>

std::string showHelp()
{
    std::string helpMessage =
        "-i\t\t<file>\t\tpath to input netlist\n"
        "-o\t\t<dir>\t\tpath to output directory\n"
        "-f\t\t<format>\tspecify output format, either csv or space\n"
        "-p\t\t<list>\t\tplots output, space separated list specifies columns to plot\n"
        "-s\t\t<path>\t\tsaves graph output as html at specified location, requires -p\n"
        "-c\t\t\t\tshows names of columns in output file, blocks -p and -s i.e. doesn't plot/save result\n"
        "-h\t\t\t\tshows this help information\n\n"
        "Usage: simulator -i file [ -ch ] [ -o dir ] [-p list] [ -s path ] [ -f format ]\n\n"
        "Examples:\n\n"
        "Plot Specific Columns:\n"
        "\tsimulator -i test.net -p 'V(N001) V(N002)'\n\n"
        "Plot All Columns and save result as interactive HTML:\n"
        "\tsimulator -i test.net -p '' -s result.html\n\n"
        "Generate result and check names of columns in netlist:\n"
        "\tsimulator -i test.net -c\n";


    return helpMessage;
}

int main(int argc, char *argv[])
{
    int c;
    std::map<std::string, std::string> stringFlags;
    std::map<std::string, bool> boolFlags;
    while ((c = getopt(argc, argv, "cf:hi:o:p:s:")) != -1)
    {
        switch (c)
        {
        case 'c':
            boolFlags["showColumns"] = true;
            break;
        case 'f':
            stringFlags["outputFormat"] = optarg;
            break;
        case 'h':
            std::cout << showHelp();
            exit(0);
        case 'i':
            stringFlags["inputFilePath"] = optarg;
            break;
        case 'o':
            stringFlags["outputFolderPath"] = optarg;
            break;
        case 'p':
            boolFlags["plotOutput"] = true;
            stringFlags["plotOutput"] = optarg;
            break;
        case 's':
            stringFlags["saveHtmlOutput"] = optarg;
            break;
        default:
            std::cerr << "Unknown Flag: '" << c << "'\n";
            exit(1);
        }
    }

    std::ifstream inputFile;
    if (!stringFlags["inputFilePath"].empty())
    {
        inputFile.open(stringFlags["inputFilePath"]);
    }
    else
    {
        std::cerr << "-i flag required to specify input netlist" << std::endl;
        exit(1);
    }

    if (inputFile.fail())
    {
        std::cerr << "File: " << stringFlags["inputFilePath"] << " was not found!\n";
        exit(1);
    }

    Circuit::Schematic *schem = Circuit::Parser::parse(inputFile);
    inputFile.close();

    if (stringFlags["outputFolderPath"].empty())
    {
        stringFlags["outputFolderPath"] = "out";
    }

    std::filesystem::create_directory(stringFlags["outputFolderPath"]);
    std::ofstream out;
    std::string outputPath;
    Circuit::Simulator::OutputFormat outputFormat;

    if (stringFlags["outputFormat"].empty() || tolower(stringFlags["outputFormat"][0]) == 'c')
    {
        outputFormat = Circuit::Simulator::OutputFormat::CSV;
    }
    else
    {
        outputFormat = Circuit::Simulator::OutputFormat::SPACE;
    }

    for (Circuit::Simulator *sim : schem->sims)
    {
        outputPath = stringFlags["outputFolderPath"] + "/" + schem->title.substr(2) + sim->simulationTypeMap[sim->type];
        if (outputFormat == Circuit::Simulator::OutputFormat::CSV && sim->type != Circuit::Simulator::SimulationType::OP)
        {
            outputPath += ".csv";
        }
        else
        {
            outputPath += ".txt";
        }
        out.open(outputPath);
        sim->run(out, outputFormat);
        out.close();

        std::string systemCall = "simulatorplot ";

        if (outputFormat == Circuit::Simulator::OutputFormat::SPACE)
        {
            systemCall += "-m space ";
        }
        if (boolFlags["showColumns"])
        {
            systemCall += "-c ";
        }
        if(!stringFlags["saveHtmlOutput"].empty())
        {
            systemCall += "-s '" + stringFlags["saveHtmlOutput"] + "' ";
        }
        systemCall += "'" + outputPath + "'";
        if (!stringFlags["plotOutput"].empty())
        {
            systemCall += " '" + stringFlags["plotOutput"] + "'";
        }
        if ((boolFlags["plotOutput"] || boolFlags["showColumns"]) && sim->type != Circuit::Simulator::SimulationType::OP)
        {
            int ret = system(systemCall.c_str());
        }
    }
    delete schem;
    return 0;
}
