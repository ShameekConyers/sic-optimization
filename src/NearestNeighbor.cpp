#include "NearestNeighbor.hpp"
#include <cassert>
#include <iostream>

NearestNeighbor::NearestNeighbor(
	Env& env, int num_ahead
)
	: EnvHeuristic{ env, "NearestNeighbor_" + std::to_string(num_ahead) + "_Ahead" },
	m_num_ahead{ num_ahead }
{

}

std::vector<int> NearestNeighbor::get_path()
{


	std::vector<int> best_path;
	double best_path_distance = INFINITY;

	std::set<int> check;
	for (int i = 0; i < m_num_nodes; i++) {
		check.insert(i);
	}

	for (int start_node = 0; start_node < 1; start_node++) {



		double working_path_distance;
		int working_node = start_node;
		std::vector<int> working_path = { working_node };

		std::set<int> to_visit;
		for (int i = 0; i < m_num_nodes; i++) {
			to_visit.insert(i);
		}


		while (!to_visit.empty()) {

			auto [node, distance] = get_next_choice(
				to_visit, working_node, m_num_ahead);

			to_visit.erase(working_node);
			if (node != working_node) working_path.push_back(node);
			working_node = node;
		}

		working_path.push_back(working_path.front());
		working_path_distance = m_env->get_path_distance(working_path);
		if (working_path_distance < best_path_distance) {
			best_path = working_path;
			best_path_distance = working_path_distance;
		}

	}

	// checks here
	assert(check.size() + 1 == best_path.size());
	check.clear();
	for (auto node : best_path) {
		check.insert(node);
	}
	assert(check.size() + 1 == best_path.size());
	assert(best_path.front() == best_path.back());

	return best_path;
}

std::pair<int, double> NearestNeighbor::get_next_choice(std::set<int> to_visit, int current_node, int num_ahead)
{
	if (num_ahead < 0 || to_visit.size() <= 1) return { current_node, 0.0 };
	to_visit.erase(current_node);
	int best_node = -1;
	double best_distance = INFINITY;

	/*
		Below finds the next best choice for our nearest neighbor algorithm
		depending on how far ahead we have to look.
	*/
	for (int node : to_visit) {
		auto [_1, next_distance] = get_next_choice(
			to_visit, node, num_ahead - 1);

		auto [distance, _2] = m_env->get_val(current_node, node);

		distance = distance + next_distance;

		if (distance <= best_distance) {
			best_node = node;
			best_distance = distance;
		}
	}


	return { best_node, best_distance };
}

FarthestNeighbor::FarthestNeighbor(Env& env)
	: EnvHeuristic{ env, "FarthestNeighbor" }
{

}

std::vector<int> FarthestNeighbor::get_path()
{
	std::set<int> to_visit;
	std::set<int> validator;
	std::vector<int> path = { 0 };
	int current_node = 0;
	for (int i = 1; i < m_num_nodes; i++) {
		validator.insert(i);
		to_visit.insert(i);
	}

	while (!to_visit.empty()) {

		double best_distance = -1.0;
		int best_node = -1;

		for (auto& node : to_visit) {
			auto [distance, _1] = m_env->get_val(current_node, node);
			if (distance >= best_distance) {
				best_distance = distance;
				best_node = node;
			}
		}

		path.push_back(best_node);
		current_node = best_node;
		to_visit.erase(current_node);
	}
	path.push_back(path.front());
	assert(path.size() == validator.size() + 2);
	return path;
}

RandomNeighbor::RandomNeighbor(Env& env)
	: EnvHeuristic{ env, "RandomNeighbor" }
{

}

std::vector<int> RandomNeighbor::get_path()
{
	std::uniform_real_distribution distribution{ 0.0, 1.0 - 1e-10 };
	std::set<int> to_visit;
	std::set<int> validator = { 0 };
	std::vector<int> path = { 0 };
	for (int i = 1; i < m_num_nodes; i++) {
		to_visit.insert(i);
		validator.insert(i);
	}

	while (!to_visit.empty()) {
		double probability_threshold = distribution(m_env->get_rng());
		int ticks = to_visit.size();
		double bin_stride = 1.0 / (double)ticks;
		double current_bin = 0.0;
		int chosen_node = -1;

		for (int node : to_visit) {
			current_bin += bin_stride;
			if (probability_threshold < current_bin) {
				chosen_node = node;
				break;
			}
		}
		assert(chosen_node != -1);

		path.push_back(chosen_node);
		to_visit.erase(chosen_node);
	}

	path.push_back(path.front());
	assert(path.size() == validator.size() + 1);
	return path;
}
