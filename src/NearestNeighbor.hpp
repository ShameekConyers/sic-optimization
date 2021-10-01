#pragma once

#include "Env.hpp"
#include "cassert"

class EnvHeuristic;
// Heuristic that simply selects its nearest neighbor
class NearestNeighbor : public EnvHeuristic {
public:
	// takes the env and how far we look ahead
	NearestNeighbor(Env& env, int num_ahead = 0);

	//
	std::vector<int> get_path();

private:
	// recursion driver
	std::pair<int, double> get_next_choice(std::set<int> to_visit, int cur_node, int num_ahead);

	//how far ahead we search
	int m_num_ahead;
};


class FarthestNeighbor : public EnvHeuristic {
public:
	FarthestNeighbor(Env& env);

	std::vector<int> get_path();
};

class RandomNeighbor : public EnvHeuristic {
public:
	RandomNeighbor(Env& env);

	std::vector<int> get_path();
};
