# AnyStreamProcess
Tools prof investigating the systems with non-poisson bitrth and death processes.

Software can investigate following systems:
* Full Availability Group without buffer
* Limited Availability Group without buffer
* FAG and LAG with continuos or discrette buffer

Time intervalls between the events can be described by following distributions:
* Poisson
* Uniform
* Gamma
* Pareto
* Normal (negative values are truncated)

There are to type of events:
* New object arrival (call or task)
* End of service

The arrival proces can be:
* state independent (intensity is constant - one super source)
* state dependent - decreasing (like in Engset model: intensity is decreasing with the number of the objects in the system - limited number of the sourcess)
* state dependent - increasing (like in Pascal model: intensity is increasing with the number of the objects in the system - limited number ot the sourcess)

Each class of objects (calls or taksk) reqiures different number or resourcess (e.g. CPUs, Lambdas, Slots).

Software consists of:
* simulators
* analytical algorithms implementation
* visualization module (model of the system and charts)
* storege module (export to xlsx or gnuplot files)

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

* Qt5
* Library for generating XLSX files: (qtbase5-private-dev, git clone https://github.com/dbzhang800/QtXlsxWriter)

### Installing
git clone https://github.com/adamkaliszan/AnyStreamProcess

## License

This project is licensed under the GPLv2 License - see the [LICENSE.md](LICENSE.md) file for details



