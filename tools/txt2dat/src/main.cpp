/*
txt2dat is an SPP problem instance converter from txt to dat file, used by glpk.
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

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <iomanip>

using std::string;
using std::setw;
using std::endl;
using std::vector;
using std::ifstream;
using std::ofstream;

int main() {
    string files[] =  { "sppnw32" };

    vector<string> filenames(files, files + sizeof(files) / sizeof(string));
    for (int file_index = 0; file_index < static_cast<int>(filenames.size()); file_index++) {
        ifstream input_file(("input/" + filenames[file_index] + ".txt").c_str());

        int elements_count = 0;
        int subsets_count = 0;

        vector<vector<int> > _instance = vector<vector<int> >();
        if (!input_file.is_open()) {
            return -1;
        }

        // the first number of the first line of the instance file is the number of elements in U
        input_file >> elements_count;

        // the second number of the first line of the instance file is the number of subsets in U
        input_file >> subsets_count;

        // each of the remaining lines of the file corresponds to one subset
        for (int i = 0; i < subsets_count; i++) {
            _instance.push_back(vector<int>(elements_count + 1));
            _instance.back().assign(elements_count + 1, 0);

            // the first value of each line is the weight of the subset
            input_file >> _instance.back()[0];

            // the second value of each line is the number of element that are covered by that subset
            int subset_element_count = 0;
            input_file >> subset_element_count;

            // from the second element on, for each element that appears,
            // the corresponding index in the instance array receives 1
            for (int j = 0; j < subset_element_count; j++) {
                int element;
                input_file >> element;
                _instance.back()[element] = 1;
            }
        }
        input_file.close();

        ofstream output_file(("output/" + filenames[file_index] + ".dat").c_str());
        if (!output_file.is_open()) {
            return -1;
        }

        output_file << "data;" << endl;
        output_file << "param elements_count := " << elements_count << ";" << endl;
        output_file << "param subsets_count := " << subsets_count << ";" << endl;
        output_file << "param instance : ";

        for (int i = 0; i <= elements_count; i++) {
            output_file << setw(6) << i << " ";
        }
        output_file << " := " << endl;

        for (int i = 0; i < subsets_count; i++) {
            output_file << "          " << setw(6) << i + 1 << " ";
            for (int j = 0; j <= elements_count; j++) {
                output_file << setw(6) << _instance[i][j] << " ";
            }
            output_file << endl;
        }

        output_file << ";" << endl;
        output_file << "end;";
        output_file.close();
    }

    return 0;
}
