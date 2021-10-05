#pragma once

#include "Env.hpp"
#include "NearestNeighbor.hpp"

class EnvHeuristic;
class NearestNeighbor;

class BruteForceTSP : public EnvHeuristic {
public:
	BruteForceTSP(
		Env& env
	);

	// returns the optimal path.
	std::vector<int> get_path();

private:

	std::vector<int> get_path_procedure(
		std::set<int> to_visit,
		int current_node,
		bool top_flag = false
	);

	int counter;
};

class BranchBoundTSP : public EnvHeuristic {
public:
	BranchBoundTSP(
		Env& env
	);

	std::vector<int> get_path();

private:
	std::vector<int> get_path_procedure(
		std::set<int> to_visit,
		int current_node,
		double bound_distance
	);

	std::unique_ptr<EnvHeuristic> m_initial_guess_heuristic;

};
