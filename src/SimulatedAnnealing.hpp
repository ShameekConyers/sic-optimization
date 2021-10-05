#pragma once

#include "Env.hpp"
#include "NearestNeighbor.hpp"

class NearestNeighbor;

class SimulatedAnnealing : public EnvHeuristic {
public:
	SimulatedAnnealing(
		Env& env
	);

	std::vector<int> get_path();
private:

	void process_step_(
		std::vector<int>& candidate,
		std::vector<int>& path,
		double temperature,
		double candidate_distance,
		double path_distance
	);

	void populate_candidate_path_(
		std::vector<int>& candidate_path,
		std::vector<int>& original_path
	);


	void reverse_segment_(
		std::vector<int>& path,
		int segment_begin,
		int segment_end
	);

	void transport_segment_(
		std::vector<int>& segment,
		std::vector<int>& path,
		size_t segment_begin,
		size_t segment_end
	);

	std::unique_ptr<EnvHeuristic> m_inital_guess_heuristic;
};
