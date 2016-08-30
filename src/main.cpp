/*
SPP Tabu Solver is a Tabu Search solver for the Set Partitioning Problem.
Copyright (C) 2016  Arthur Jacobs

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>

#include "../include/set_partitioner.h"

using std::cout;

int main(int argc, char *argv[]) {
    char *input_filename;
    char *output_filename;

    int tabu_list_size = 30;
    int iteration_limit = 200;
    double time_limit = 900;
    unsigned int seed = 515151;

    if (argc <= 2) {
        cout << "To use this program, you should type the following command line:" << endl;
        cout << endl <<"    spp_tabu_solver <output_file> <input_file> <optional_params>" << endl;
        cout << endl << "Params list:" << endl;
        cout << "      -t <tabu_list_size>; (default = 30)" << endl;
        cout << "         Number of moves that will be kept in Tabu List." << endl;
        cout << "      -i <iteration_limit>; (default = 200)" << endl;
        cout << "         Maximum number of moves without improvement before the solver stops;" << endl;
        cout << "      -l <time_limit>; (default = 15m)" << endl;
        cout << "         Maximum amount of time the tabu search will run (in minutes)" << endl;
        cout << "      -s <random_seed>; (default = 515151)" << endl;
        cout << "         Seed for the randomized feasibility heuristic used in the initial solution;" << endl;
        cout << "\nIn case you want to keep the params dafault values, just do not use them.";
        getchar();
        return -1;
    }

    // verifies if it is possible to create the output file.
    std::ofstream output_file(argv[1]);
    if (!output_file.is_open()) {
        cout << endl << "ERROR: It was impossible to create the output file: " << argv[1] << endl;
        getchar();
        return 1;
    }
    output_file.close();

    output_filename = argv[1];

    // verifies if it is possible to open the input file, containing the instance.
    std::ifstream input_file(argv[2]);
    if (!input_file.is_open()) {
        cout << endl << "ERROR: It was impossible to open the input file: " << argv[2] << endl;
        getchar();
        return 2;
    }
    input_file.close();
    input_filename = argv[2];

    // reads optional params
    for (int i = 3; i < argc; i++) {
        // verifies if there is a value after the chosen parameter and if the parameter value is valid
        if (i + 1 > argc || atoi(argv[i + 1]) < 0) {
            cout << endl << "ERROR: Please type a positive integer value after the chosen parameter."<< endl;
            cout << endl << "WARNING: Default value will be used." << endl;
            getchar();
        }
        // verifies if the param is "-t"
        if (strcmp(argv[i], "-t") == 0) {
            tabu_list_size = atoi(argv[i + 1]);
        }

        // verifies it the param is "-l"
        if (strcmp(argv[i], "-l") == 0) {
            time_limit = atoi(argv[i + 1]) * 60;
        }

        // verifies it the param is "-i"
        if (strcmp(argv[i], "-i") == 0) {
            iteration_limit = atoi(argv[i + 1]);
        }

        // verifies it the param is "-l"
        if (strcmp(argv[i], "-s") == 0) {
            seed = atoi(argv[i + 1]);
        }
    }


    SetPartitioner sp = SetPartitioner(tabu_list_size, iteration_limit, time_limit, seed);

    cout << "Reading instance..." << endl;
    if (sp.ReadInstance(input_filename) != 0) {
        cout << endl << "ERROR: Could not read from input file." << endl;
        getchar();
        return -1;
    }

    cout << "Solving..." << endl;
    sp.Solve();

    cout << "Writing results..." << endl;
    if (sp.WriteResults(output_filename) != 0) {
        cout << endl << "ERROR: Could not write output file." << endl;
        getchar();
        return -1;
    }

    return 0;
}
