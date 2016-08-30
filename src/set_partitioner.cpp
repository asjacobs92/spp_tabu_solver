/*
SPP Tabu Solver is a Tabu Search solver for the Set Partitioning Problem.
Copyright (C) 2016 Arthur Jacobs

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http:// www.gnu.org/licenses/>.
*/

#include "../include/set_partitioner.h"

#include <list>
#include <ctime>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

SetPartitioner::SetPartitioner() {
    _seed = 515151;
    _time_limit = 900;
    _execution_time = 0;
    _iterations_count = 0;
    _iteration_limit = 100;
    _tabu_list_max_size = 50;
    _tabu_list = list<SubsetSwap>();
}

SetPartitioner::SetPartitioner(int tabu_list_size, int iteration_limit, double time_limit, unsigned int seed) {
    _seed = seed;
    _execution_time = 0;
    _iterations_count = 0;
    _time_limit = time_limit;
    _iteration_limit = iteration_limit;
    _tabu_list_max_size = tabu_list_size;
    _tabu_list = list<SubsetSwap>();
}

SetPartitioner::~SetPartitioner() {
    if (_solution != NULL) {
        delete _solution;
        delete _solution_elements;
    }
}

void SetPartitioner::Solve() {
    _start_time = static_cast<double>(clock() / CLOCKS_PER_SEC);

    int iterations = 0;
    vector<int>* best_solution = new vector<int>();
    vector<int>* best_solution_elements = new vector<int>();

    FindInitialSolution();

    (*_solution) = (*_initial_solution);
    (*_solution_elements) = (*_initial_solution_elements);
    (*best_solution) = (*_initial_solution);
    (*best_solution_elements) = (*_initial_solution_elements);

    while ((iterations < _iteration_limit) && (_execution_time < _time_limit)) {
        _iterations_count++;
        vector<SubsetSwap>* neighborhood = new vector<SubsetSwap>();
        FindNeighborhood(neighborhood);
        int best_candidate = FindBestCandidate((*neighborhood));

        SwapSubsets((*neighborhood)[best_candidate], _solution, _solution_elements);
        if (EvaluateSolution((*_solution_elements)) < EvaluateSolution((*best_solution_elements))) {
            iterations = 0;
            (*best_solution) = (*_solution);
            (*best_solution_elements) = (*_solution_elements);
        } else {
            iterations++;
        }
        AddSwapToTabu((*neighborhood)[best_candidate]);
        delete neighborhood;

        double currentTime = static_cast<double>(clock() / CLOCKS_PER_SEC);
        _execution_time = currentTime - _start_time;
    }

    (*_solution) = (*best_solution);
    (*_solution_elements) = (*best_solution_elements);

    if (IsSolutionFeasible((*_solution_elements))) {
        cout << endl << "FEASIBLE SOLUTION FOUND!" << endl << endl;
    }

    double endTime = static_cast<double>(clock() / CLOCKS_PER_SEC);
    _execution_time = endTime - _start_time;
    cout << "Execution time: " << _execution_time << "sec." << endl;
}

void SetPartitioner::FindInitialSolution() {
    // for each element
    for (int element = 1; element < static_cast<int>((*_initial_solution_elements).size()); element++) {
        // if the element is not part of the solution yet
        if ((*_initial_solution_elements)[element] == 0) {
            // find all the subsets that cover that element
            vector<int> possible_subsets = vector<int>();
            for (int subset = 0; subset < static_cast<int>(_instance.size()); subset++) {
                if (_instance[subset][element] == 1) {
                    possible_subsets.push_back(subset);
                }
            }

            // if there is any subset that contain that element
            if (!possible_subsets.empty()) {
                // search the subset with the lowest weight
                int selected_subset = possible_subsets.front();
                int lower_weight = _instance[possible_subsets.front()][0];
                for (int subset = 0; subset < static_cast<int>(possible_subsets.size()); subset++) {
                    if (_instance[possible_subsets[subset]][0] < lower_weight) {
                        lower_weight = _instance[possible_subsets[subset]][0];
                        selected_subset = possible_subsets[subset];
                    }
                }

                // and then adds it to the solution
                AddSubsetToSolution(selected_subset, _initial_solution, _initial_solution_elements);
            }
        }
    }

    // since the initial solution does not consider elements covered by more than one element
    // it is necessary to run an Heuristic to make less unfeasible (if not feasible)
    HeuristicFeasibilityOperator();
}

void SetPartitioner::HeuristicFeasibilityOperator() {
    srand(_seed);

    // creates an auxiliary array containng the indexes of the subsets that form
    // the current solution
    vector<int> solution_subsets = vector<int>();
    for (int subset = 0; subset < static_cast<int>(_instance.size()); subset++) {
        if ((*_initial_solution)[subset] == 1) {
            solution_subsets.push_back(subset);
        }
    }

    // while the auxiliary subset array has any elements
    while (!solution_subsets.empty()) {
        // a random subset from the array is chosen
        int subset_index = rand() % solution_subsets.size();
        int subset = solution_subsets[subset_index];
        // and removed from the auxiliary array
        solution_subsets.erase(solution_subsets.begin() + subset_index);

        // checks is the subset has any element that is covered by more than one
        // subset
        bool remove_subset = false;
        for (int element = 1; element < static_cast<int>((*_initial_solution_elements).size()); element++) {
            // if the element is present in the subset and it appears more than one
            // time in the solution
            if (_instance[subset][element] == 1 && (*_initial_solution_elements)[element] > 1) {
                // this subset should be removed from the solution
                remove_subset = true;
            }
        }

        if (remove_subset) {
            // removes the subset from the solution, along with all its elements
            RemoveSubsetFromSolution(subset, _initial_solution, _initial_solution_elements);
        }
    }

    // initializes and fills an auxiliary array containing the indexes of the
    // elements that are missing from the current solution
    vector<int> missing_elements = vector<int>();
    for (int element = 1; element < static_cast<int>((*_initial_solution_elements).size()); element++) {
        if ((*_initial_solution_elements)[element] == 0) {
            missing_elements.push_back(element);
        }
    }

    // while the auxiliary array of missing elements still is not empty
    while (!missing_elements.empty()) {
        // one element from the array is chosen ramdomly
        int element_index = rand() % missing_elements.size();
        int element = missing_elements[element_index];
        // and then removed from the missing elements array
        missing_elements.erase(missing_elements.begin() + element_index);

        // for each subset
        vector<int> possible_subsets = vector<int>();
        for (int subset = 0; subset < static_cast<int>(_instance.size()); subset++) {
            // if the subset contains the missing element
            if (_instance[subset][element] == 1) {
                // and does not contain any other element that is already covered in the
                // solution
                bool add_subset = true;
                for (int i = 1; i < static_cast<int>((*_initial_solution_elements).size()); i++) {
                    if (_instance[subset][i] == 1 && (*_initial_solution_elements)[i] == 1) {
                        add_subset = false;
                    }
                }

                // the subset is added to the list of possible subsets to be added
                if (add_subset) {
                    possible_subsets.push_back(subset);
                }
            }
        }

        // if there is any subset that matches that criteria
        if (!possible_subsets.empty()) {
            // chooses the one that has the lowest weight to add to the current
            // solution
            int selected_subset = possible_subsets.front();
            int lower_weight = _instance[possible_subsets.front()][0];
            for (int j = 0; j < static_cast<int>(possible_subsets.size()); j++) {
                if (_instance[possible_subsets[j]][0] < lower_weight) {
                    lower_weight = _instance[possible_subsets[j]][0];
                    selected_subset = possible_subsets[j];
                }
            }

            // then adds the chosen subset
            AddSubsetToSolution(selected_subset, _initial_solution, _initial_solution_elements);
            // and removes all the the other missing elements the selected subset covers
            for (int i = 1; i < static_cast<int>(_instance[selected_subset].size()); i++) {
                if (_instance[selected_subset][i] == 1) {
                    vector<int>::iterator found_element_it = find(missing_elements.begin(), missing_elements.end(), i);
                    if (found_element_it != missing_elements.end()) {
                        missing_elements.erase(found_element_it);
                    }
                }
            }
        }
    }
}

int SetPartitioner::EvaluateSolution(vector<int> covered_elements) {
    int solution_value = 0;
    int scaling_factor = 1;
    for (int i = 1; i < static_cast<int>(covered_elements.size()); i++) {
        if (covered_elements[i] != 1) {
            scaling_factor++;
        }
    }
    solution_value = covered_elements[0] * scaling_factor;
    return solution_value;
}

int SetPartitioner::FindBestCandidate(vector<SubsetSwap> neighborhood) {
    int best_value = 0;
    int best_candidate = -1;
    bool first_candidate = true;
    for (int i = 0; i < static_cast<int>(neighborhood.size()); i++) {
        vector<int>* candidate_solution = new vector<int>;
        vector<int>* candidate_solution_elements = new vector<int>;

        (*candidate_solution) = (*_solution);
        (*candidate_solution_elements) = (*_solution_elements);

        SwapSubsets(neighborhood[i], candidate_solution, candidate_solution_elements);

        int solution_value = EvaluateSolution((*candidate_solution_elements));
        if (first_candidate) {
            first_candidate = false;
            best_candidate = i;
            best_value = solution_value;
        } else if (solution_value < best_value) {
            best_value = solution_value;
            best_candidate = i;
        }

        delete candidate_solution;
        delete candidate_solution_elements;

        double currentTime = static_cast<double>(clock() / CLOCKS_PER_SEC);
        _execution_time = currentTime - _start_time;

        if (_execution_time > _time_limit) {
            return best_candidate;
        }
    }

    return best_candidate;
}

void SetPartitioner::FindNeighborhood(vector<SubsetSwap>* neighborhood) {
    for (int i = 0; i < static_cast<int>(_instance.size()); i++) {
        if ((*_solution)[i] == 1) {
            for (int j = 0; j < static_cast<int>(_instance.size()); j++) {
                if ((*_solution)[j] == 0 && !IsTabu(SubsetSwap(i, j))) {
                    (*neighborhood).push_back(SubsetSwap(i, j));
                }
            }
        }
    }
}

void SetPartitioner::AddSubsetToSolution(int subset_index, vector<int>* solution, vector<int>* covered_elements) {
    // adds the subset to the array of subsets that forms the solution
    (*solution)[subset_index] = 1;
    // adds the subset weight to the solution total weight
    (*covered_elements)[0] += _instance[subset_index][0];

    // adds each element of the subset to the solution array of elements
    for (int i = 1; i < static_cast<int>(_instance[subset_index].size()); i++) {
        if (_instance[subset_index][i] == 1) {
            (*covered_elements)[i] += 1;
        }
    }
}

void SetPartitioner::RemoveSubsetFromSolution(int subset_index, vector<int>* solution, vector<int>* covered_elements) {
    // removes the subset to the array of subsets that forms the solution
    (*solution)[subset_index] = 0;
    // removes the subset weight to the solution total weight
    (*covered_elements)[0] -= _instance[subset_index][0];

    // removes each element of the subset to the solution array of elements
    for (int i = 1; i < static_cast<int>(_instance[subset_index].size()); i++) {
        if (_instance[subset_index][i] == 1) {
            (*covered_elements)[i] -= 1;
        }
    }
}

void SetPartitioner::SwapSubsets(SubsetSwap subsets_to_swap,  vector<int>* solution, vector<int>* covered_elements) {
    int subset_to_add;
    int subset_to_remove;

    // if the first subset is in the solution
    if ((*solution)[subsets_to_swap.subset_index_one] == 1) {
        // then the second subset should be added to the solution
        subset_to_add = subsets_to_swap.subset_index_two;
        // and the first one removed
        subset_to_remove = subsets_to_swap.subset_index_one;
    } else {
        // if the first subset is not in the solution, then the second subset must be
        // then the first subset should be added to the solution
        subset_to_add = subsets_to_swap.subset_index_two;
        // and the second one removed
        subset_to_remove = subsets_to_swap.subset_index_one;
    }

    RemoveSubsetFromSolution(subset_to_remove, solution, covered_elements);
    AddSubsetToSolution(subset_to_add, solution, covered_elements);
}

bool SetPartitioner::IsSolutionFeasible(vector<int> covered_elements) {
    bool solution_feasible = true;
    for (int i = 1; i < static_cast<int>(covered_elements.size()); i++) {
        if (covered_elements[i] == 0 || covered_elements[i] > 1) {
            solution_feasible = false;
        }
    }
    return solution_feasible;
}

bool SetPartitioner::IsTabu(SubsetSwap a) {
    bool tabu = false;
    for (list<SubsetSwap>::iterator it = _tabu_list.begin(); it != _tabu_list.end(); it++) {
        if ((*it) == a) {
            tabu = true;
        }
    }
    return tabu;
}

void SetPartitioner::AddSwapToTabu(SubsetSwap a) {
    if (static_cast<int>(_tabu_list.size()) == _tabu_list_max_size) {
        _tabu_list.pop_front();
    }
    _tabu_list.push_back(a);
}

void SetPartitioner::PrintSolution(vector<int> solution, vector<int> covered_elements) {
    cout << endl << "Solution value: " << covered_elements[0] << endl;
    cout << "Solution elements: ";
    for (int i = 1; i < static_cast<int>(covered_elements.size()); i++) {
        if (covered_elements[i] >= 1) {
            cout << i << " ";
        }
    }
    cout << endl;

    cout << "Solution subsets: ";
    for (int i = 0; i < static_cast<int>(solution.size()); i++) {
        if (solution[i] == 1) {
            cout << i << " ";
        }
    }
    cout << endl << endl;
}

int SetPartitioner::ReadInstance(char* filename) {
    std::ifstream input_file(filename);

    if (!input_file.is_open()) {
        return -1;
    }

    int elements_count = 0;
    int subsets_count = 0;

    // the first number of the first line of the instance file is the number of
    // elements in U
    input_file >> elements_count;

    // the second number of the first line of the instance file is the number of
    // subsets in U
    input_file >> subsets_count;

    for (int i = 0; i < subsets_count; i++) {
        _instance.push_back(vector<int>(elements_count + 1));
        _instance.back().assign(elements_count + 1, 0);

        // the first value of each line is the weight of the subset
        input_file >> _instance.back()[0];

        // the second value of each line is the number of element that are covered
        // by that subset
        int subset_element_count = 0;
        input_file >> subset_element_count;

        // from the second element on, for each element that appears, the
        // corresponding index in the instance array receives 1
        for (int j = 0; j < subset_element_count; j++) {
            int element;
            input_file >> element;
            _instance.back()[element] = 1;
        }
    }

    input_file.close();

    // initializes the binary array that represents which subsets form the
    // solution
    _solution = new vector<int>(subsets_count);
    (*_solution).assign((*_solution).size(), 0);
    // initializes the binary solution array with n + 1 position, where n is the
    // total number of elements
    // (position 0 is the value of the solution)
    _solution_elements = new vector<int>(elements_count + 1);
    (*_solution_elements).assign((*_solution_elements).size(), 0);

    // initializes the binary array that represents which subsets form the initial
    // solution
    _initial_solution = new vector<int>(subsets_count);
    (*_initial_solution).assign((*_initial_solution).size(), 0);
    // initializes the binary initial solution array with n + 1 position, where n
    // is the total number of elements
    // (position 0 is the value of the solution)
    _initial_solution_elements = new vector<int>(elements_count + 1);
    (*_initial_solution_elements).assign((*_initial_solution_elements).size(), 0);

    return 0;
}

int SetPartitioner::WriteResults(char* filename) {
    std::ofstream output_file(filename);

    if (!output_file.is_open()) {
        return -1;
    }

    output_file << "****************************************************" << endl;
    output_file << "**** SET PARTITIONING SOLVER USING TABU SEARCH *****" << endl;
    output_file << "****************************************************" << endl;
    output_file << endl;

    output_file << "RESULTS:" << endl;
    output_file << endl;

    output_file << "Params:" << endl;
    output_file << "     Tabu list size: " << _tabu_list.size() << endl;
    output_file << "     Iteration limit: " << _iteration_limit << endl;
    output_file << "     Time limit: " << _time_limit << endl;
    output_file << "     Radom seed: " << _seed << endl;
    output_file << endl;

    output_file << "Number of iterations: " << _iterations_count << endl;
    output_file << "Execution time: " << _execution_time << " seconds." << endl;
    output_file << endl;

    output_file << "Instance:" << endl;
    output_file << "     Number of elements: " << (*_solution_elements).size() - 1 << endl;
    output_file << "     Number of subsets: " << (*_solution).size() << endl;

    if (IsSolutionFeasible((*_initial_solution_elements))) {
        output_file << endl << "FEASIBLE INITIAL SOLUTION FOUND!" << endl << endl;
    } else {
        output_file << endl
                    << "NOT FEASIBLE INITIAL SOLUTION FOUND!" << endl
                    << endl;
    }

    output_file << "Initial solution:" << endl;
    output_file << "        Value: " << (*_initial_solution_elements)[0] << endl;
    output_file << "        Elements: ";
    for (int element = 1; element < static_cast<int>((*_initial_solution_elements).size()); element++) {
        if ((*_initial_solution_elements)[element] >= 1) {
            output_file << element << " ";
        }
    }
    output_file << endl;

    output_file << "        Subsets: " << endl;
    for (int subset = 0; subset < static_cast<int>((*_initial_solution).size()); subset++) {
        if ((*_initial_solution)[subset] == 1) {
            output_file << "             Number " << subset
                        << " (Value = " << _instance[subset][0] << "): ";
            for (int element = 1; element < static_cast<int>(_instance[subset].size()); element++) {
                if (_instance[subset][element] == 1) {
                    output_file << element << " ";
                }
            }
            output_file << endl;
        }
    }

    if (IsSolutionFeasible((*_solution_elements))) {
        output_file << endl << "FEASIBLE SOLUTION FOUND!" << endl << endl;
    } else {
        output_file << endl << "NO FEASIBLE SOLUTION FOUND!" << endl << endl;
    }

    output_file << "Final solution:" << endl;
    output_file << "        Value: " << (*_solution_elements)[0] << endl;
    output_file << "        Elements: ";
    for (int element = 1; element < static_cast<int>((*_solution_elements).size()); element++) {
        if ((*_solution_elements)[element] >= 1) {
            output_file << element << " ";
        }
    }
    output_file << endl;

    output_file << "        Subsets: " << endl;
    for (int subset = 0; subset < static_cast<int>((*_solution).size()); subset++) {
        if ((*_solution)[subset] == 1) {
            output_file << "             Number " << subset
                        << " (Value = " << _instance[subset][0] << "): ";
            for (int element = 1; element < static_cast<int>(_instance[subset].size()); element++) {
                if (_instance[subset][element] == 1) {
                    output_file << element << " ";
                }
            }
            output_file << endl;
        }
    }
    output_file << endl;
    output_file.close();

    return 0;
}
