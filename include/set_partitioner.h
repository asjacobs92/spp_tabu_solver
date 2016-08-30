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

#ifndef SET_PARTITIONER_H
#define SET_PARTITIONER_H

#include <list>
#include <vector>
#include <iostream>

using std::cout;
using std::endl;
using std::list;
using std::vector;

struct SubsetSwap {
	int subset_index_one;
	int subset_index_two;

	SubsetSwap() {
		subset_index_one = -1;
		subset_index_two = -1;
	}

	SubsetSwap(int subset_one, int subset_two) {
		subset_index_one = subset_one;
		subset_index_two = subset_two;
	};

	inline bool operator==(const SubsetSwap &a) {
		if (subset_index_one == a.subset_index_one && subset_index_two == a.subset_index_two)
		{
			return true;
		}

		if (subset_index_one == a.subset_index_two && subset_index_two == a.subset_index_one)
		{
			return true;
		}

		return false;
	};
};

class SetPartitioner  {
	public:
		//-------------------------------------//
		//-----------Public-methods------------//
		//-------------------------------------//

		SetPartitioner();

		SetPartitioner(int tabu_list_size, int iteration_limit, double time_limit, unsigned int seed);

		~SetPartitioner();

		void Solve();

		int ReadInstance(char *filename);

		int WriteResults(char *filename);

	private:
		//------------------------------------//
		//----------Private-methods-----------//
		//------------------------------------//

		bool IsTabu(SubsetSwap a);

		void AddSwapToTabu(SubsetSwap a);

		void FindInitialSolution();

		void HeuristicFeasibilityOperator();

		int EvaluateSolution(vector<int> covered_elements);

		bool IsSolutionFeasible(vector<int> covered_elements);

		int FindBestCandidate(vector<SubsetSwap> neighborhood);

		void FindNeighborhood(vector<SubsetSwap> *neighborhood);

		void AddSubsetToSolution(int subset_index, vector<int> *solution, vector<int> *covered_elements);

		void SwapSubsets(SubsetSwap subsets_to_swap, vector<int> *solution, vector<int> *covered_elements);

		void RemoveSubsetFromSolution(int subset_index, vector<int> *solution, vector<int> *covered_elements);


		//debug only
		void PrintSolution(vector<int> solution, vector<int> covered_elements);

	private:
		//------------------------------------//
		//---------Private-variables----------//
		//------------------------------------//

		unsigned int _seed;

		int _iteration_limit;

		int _iterations_count;

		int _tabu_list_max_size;

		double _time_limit;

		double _start_time;

		double _execution_time;

		vector<int> *_solution;

		vector<int> *_solution_elements;

		vector<int> *_initial_solution;

		vector<int> *_initial_solution_elements;

		list<SubsetSwap> _tabu_list;

		vector<vector<int> > _instance;
};

#endif
