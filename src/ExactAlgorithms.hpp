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

	void generate_path_procedure(
		std::vector<int> current_path,
		std::set<int> to_visit,
		int next_node
	);

	std::vector<int> m_best_path;
	double m_best_distance;
};

class BranchBoundTSP : public EnvHeuristic {
public:
	BranchBoundTSP(
		Env& env
	);

	std::vector<int> get_path();

private:
	void generate_path_procedure(
		std::vector<int> current_path,
		std::set<int> to_visit,
		int next_node,
		double bound_distance
	);

	std::unique_ptr<EnvHeuristic> m_initial_guess_heuristic;

	std::vector<int> m_best_path;
	double m_best_distance;
};
