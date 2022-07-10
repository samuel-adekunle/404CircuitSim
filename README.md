```bash
┏━╸┏━┓┏━┓┏━┓┏━┓   ╻ ╻┏━┓╻ ╻   ┏━┓╻┏┳┓╻ ╻╻  ┏━┓╺┳╸┏━┓┏━┓
┣╸ ┣┳┛┣┳┛┃ ┃┣┳┛   ┗━┫┃┃┃┗━┫   ┗━┓┃┃┃┃┃ ┃┃  ┣━┫ ┃ ┃ ┃┣┳┛
┗━╸╹┗╸╹┗╸┗━┛╹┗╸     ╹┗━┛  ╹   ┗━┛╹╹ ╹┗━┛┗━╸╹ ╹ ╹ ┗━┛╹┗╸
```
## Table of Contents

1. [Introduction](#introduction)
2. [Getting Started](#getting-started)
3. [Usage](#usage)
4. [Authors](#authors)

## Introduction

The aim of this project was to fully implement the functionality of LT Spice from a command line interface (CLI). The motivations to build this project originated in irritation of LT Spice's poor cross platform compatibility and interest in developing a deeper understanding in the internal workings of a circuit simulator. This project also seeks to provide stronger support for standard output formats compared to LT Spice and make it simpler and easier to use the output of a simulation in another program. Our in-built plotter also aims to be more customisable and clearer, than the default graph-plotter packaged with LT Spice. Finally the team thought that, of all the tasks offered to us, development of a  circuit simulator covered the greatest number of topics taught this year, making it an excellent opportunity to cement our knowledge. Topics required for the task: C++ programming, linear algebra, analysis of circuits and design of circuits.

## Getting Started

### System Requirements

#### Operating System

 - Linux: Ubuntu 18.04 or higher
 - MacOS: Catalina 10.15 or higher

#### Terminal Shell

 - bash (recommended)
 - sh
 - zsh

### CMake

CMake is used for building and managing dependencies. To install CMake, [click here](https://cmake.org/install/). To confirm if CMake is installed correctly on your machine, run:
```bash
cmake --version
```
The version of CMake installed should come up. CMake 3.16.X is required!

### Python (Only for the graphing tool)

To use the in built graphing tool, Python 3 is required. To install python, [click here](https://docs.python-guide.org/starting/installation/). To confirm if Python 3 is installed correctly on your machine, run:
```bash
python3
```
An interactive shell should come up showing the python version. Python 3.7.X is required!

Once Python 3 has been correctly installed, navigate to the root directory and run:
```bash
python3 -m pip install -r requirements.txt
```
To install the necessary packages required for the graphing tool to work.

### Installing

To install the simulator, navigate to the build directory and run the installation script:
```bash
cd build
./install.sh
```
And follow the on screen instructions.

## Usage
```
-i              <file>          path to input netlist
-o              <dir>           path to output directory
-f              <format>        specify output format, either csv or space
-p              <list>          plots output, space separated list specifies columns to plot
-s              <path>          saves graph output as html at specified location, requires -p
-c                              shows names of columns in output file, blocks -p and -s i.e. doesn't plot/save result
-h                              shows this help information

Usage: simulator -i file [ -ch ] [ -o dir ] [-p list] [ -s path ] [ -f format ]

Examples:

Plot Specific Columns:
    simulator -i test.net -p 'V(N001) V(N002)'

Plot All Columns and save result as interactive HTML:
    simulator -i test.net -p '' -s result.html

Generate result and check names of columns in netlist:
    simulator -i test.net -c
```

## Authors

 - [Jonah Lehner](https://github.com/jjlehner)
 - [Samuel Adekunle](https://github.com/SamtheSaint)
 - [Neel Dugar](https://github.com/neeldug)
