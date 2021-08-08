#include "Env.hpp"
#include <iostream>
#include <cassert>
#include <map>
#include <string>
#include <math.h>
#include <stack>
/*
	Ant:
		std::set<int> m_visited;
		int m_pos;

	Env:
		std::vector<
			std::pair<float, float>> m_ref_graph;
		std::vector<
			std::pair<float, float>> m_graph;
		int m_num_nodes;
		std::vector<Ant> m_ants;
		std::mt19937 m_rng;
		int m_num_ants;
		int m_num_steps;

*/

float PHER_PARAM = 0.9;
float DIST_PARAM = 2.0;

float PHER_DECA = 0.25;




Ant::Ant(int init_pos)
	: m_visited{},
	m_pos{ init_pos }
{

}

Env::Env(int num_nodes, int num_ants, int num_steps)
	: m_ref_graph{},
	m_graph{},
	m_num_nodes{ num_nodes },
	m_ants{},
	m_rng{},
	m_num_steps{ num_steps }
{
	//preliminary rng setup
	std::random_device rd;
	std::uniform_int_distribution rd_dist(0, 10000);
	m_rng.seed(rd_dist(rd)); // random seed, replace with constant if necessary

	std::uniform_real_distribution midpoint_dist(10.0, 20.0);
	std::uniform_real_distribution bounds_dist(0.0, 20.0);


	float lb = fmax(0.0, midpoint_dist(m_rng) - bounds_dist(m_rng));
	float ub = midpoint_dist(m_rng) + bounds_dist(m_rng);
	std::uniform_real_distribution dist(lb, ub);
	m_ref_graph.reserve(m_num_nodes);
	m_graph.reserve(m_num_nodes* m_num_nodes);

	//Init ref graph with random pair of (X, Y)
	for (int i = 0; i < m_num_nodes; i++) {
		float xpos = dist(m_rng);
		float ypos = dist(m_rng);
		m_ref_graph.push_back({ xpos, ypos });

	}

	// calc distances with initial pheromone vals of 1
	for (int i = 0; i < m_num_nodes; i++) {
		for (int j = 0; j < m_num_nodes; j++) {
			auto [ixpos, iypos] = m_ref_graph[i];
			auto [jxpos, jypos] = m_ref_graph[j];
			float xpos = ixpos - jxpos;
			float ypos = iypos - jypos;
			xpos = xpos * xpos;
			ypos = ypos * ypos;


			m_graph.push_back({ sqrtf(xpos + ypos), 1.0 });
		}
	}

	//quick checks
	assert(m_ref_graph.size() == m_num_nodes);
	assert(m_graph.size() == m_num_nodes * m_num_nodes);

	//init ants at random location
	std::uniform_int_distribution row_dist(0, m_num_nodes - 1);
	for (int n = 0; n < num_ants; n++) {
		m_ants.push_back({ row_dist(m_rng) });
	}

}

std::pair<float, float> Env::getVal(int i, int j)
{
	assert(0 <= i && i < m_num_nodes); //!
	assert(0 <= j && j < m_num_nodes); //!
	return m_graph[(i * m_num_nodes) + j];
}
void Env::setVal(int i, int j, std::pair<float, float> val)
{
	assert(0 <= i && i < m_num_nodes); //!
	assert(0 <= j && j < m_num_nodes); //!
	m_graph[(i * m_num_nodes) + j] = val;
}

void Env::runACLoop()
{
	/*
		Runs the loop for ant cololy optimization
	*/
	int counter = 0;
	while (counter < m_num_steps) {
		counter++;
		processACState();
		updateACState();
	}

}

std::vector<std::pair<int, int>> Env::generateACO()
{
	/*
		Returns the best path for the graph.
	*/

	// We generate best path by getting
	//the minimum pheromone for each edge choice
	std::vector<std::pair<int, int>> result;
	std::set<int> to_visit;
	int pos = 0;


	for (int i = 1; i < m_num_nodes; i++) {
		to_visit.insert(i);
	}

	while (!to_visit.empty()) {
		float best = INFINITY;
		int best_node = -1;
		for (auto& node : to_visit) {
			auto [dst, pher] = getVal(pos, node);
			if (pher < best) {
				best = pher;
				best_node = node;
			}
		}
		to_visit.erase(best_node);
		result.push_back({ pos, best_node });
		pos = best_node;
	}
	return result;
}

float Env::getACDist()
{
	/*
		Utilizes the Ant Colony algorithm
	*/
	float result = 0;
	std::set<int> to_visit;
	int pos = 0;


	for (int i = 1; i < m_num_nodes; i++) {
		to_visit.insert(i);
	}

	while (!to_visit.empty()) {
		float best = 0;
		float best_dst = INFINITY;
		int best_node = -1;
		for (auto& node : to_visit) {
			auto [dst, pher] = getVal(pos, node);
			if (pher > best) {
				best = pher;
				best_dst = dst;
				best_node = node;
			}
		}
		to_visit.erase(best_node);
		result += best_dst;
		pos = best_node;
	}
	return result;
}

float Env::getBruteForceDist()
{
	/*
		Simple brute force recursion
	*/
	std::set<int> to_visit;
	int pos = 0;
	float best = 1e10;

	for (int i = 1; i < m_num_nodes; i++) {
		to_visit.insert(i);
	}

	return getBruteForceDistBFS(to_visit, 0);
}

float Env::getBruteForceDistBFS(std::set<int>& to_visit, int cur_node)
{
	/*
		Helper procedure
	*/
	if (to_visit.empty()) return 0.0;
	float best = 1e10;

	for (auto& node : to_visit) {
		std::set<int> next_to_visit = to_visit;
		next_to_visit.erase(node);
		auto [dist, _] = getVal(cur_node, node);
		float rest = getBruteForceDistBFS(next_to_visit, node);
		best = fmin(best, dist + rest);
	}
	return best;
}



float Env::getNNDist(bool complex)
{
	/*
		Utilizes the nearest neighbors to find a solution, iterates over ALL
		possible start points due to the cheapness of computation
	*/

	float best_result = 1e10;
	int bound = m_num_nodes;
	if (!complex) bound = 1;
	for (int i = 0; i < 1; i++) {
		std::set<int> to_visit;
		int pos = 0;
		float result = 0;

		for (int i = 0; i < m_num_nodes; i++) {
			if (i == pos) continue;
			to_visit.insert(i);
		}

		while (!to_visit.empty()) {
			float best = 1e10;
			int best_node = -1;

			for (auto& node : to_visit) {
				auto [dst, _] = getVal(pos, node);
				if (dst < best) {
					best = dst;
					best_node = node;
				}

			}
			to_visit.erase(best_node);
			result += best;
			pos = best_node;
		}

		best_result = fmin(result, best_result);
	}
	return best_result;

}

float Env::getOneAheadNNDist(bool complex)
{
	/*
		Like Nearest Neighbors but also searches one ahead
		and chooses the minimum path

	*/
	float best_result = 1e10;
	int bound = m_num_nodes;
	if (!complex) bound = 1;
	for (int i = 0; i < 1; i++) {
		std::set<int> to_visit;
		int pos = 0;
		float result = 0;

		for (int i = 0; i < m_num_nodes; i++) {
			if (i == pos) continue;
			to_visit.insert(i);
		}

		while (!to_visit.empty()) {
			float stored_ahead = 0.0;
			float best = 1e10;
			int best_node = -1;

			for (auto& node : to_visit) {
				auto [dst, pher] = getVal(pos, node);
				float ahead_best = 1e10;

				for (auto& ahead_node : to_visit) {
					if (ahead_node == node) continue;
					auto [ahead_dst, _] = getVal(node, ahead_node);
					if (ahead_dst < ahead_best) {
						ahead_best = ahead_dst;
					}
				}
				if (to_visit.size() == 1) ahead_best = 0.0;
				if (dst + ahead_best < best) {
					best = dst + ahead_best;
					stored_ahead = ahead_best;
					best_node = node;
				}

			}
			to_visit.erase(best_node);
			result += best - stored_ahead;
			pos = best_node;
		}
		best_result = fmin(result, best_result);
	}
	return best_result;
}

float Env::getBBDist(float upper_bound_hint)
{
	/*
		Utilizes the branch and bound method for searching for an
		optimal soulution

		if an upper_bound hint is provided it will use that
	*/

	float upper_bound = getNNDist(false);
	if (upper_bound_hint > 0.1) upper_bound = upper_bound_hint;
	std::stack<int> cursor;
	int pos = 0;
	float result = 0;

	std::set<int> to_visit;
	for (int i = 1; i < m_num_nodes; i++) {
		to_visit.insert(i);
	}

	float bb_dist = getBBDistDFS(to_visit, upper_bound, 0);
	if (bb_dist < 0) assert(false);
	return bb_dist;
}

float Env::getBBDistDFS(std::set<int>& to_visit, float ub, int cur_node)
{
	/*
		Helper procedure.
	*/
	if (to_visit.empty()) return 0.0;
	if (ub < 0.0) return -1.0;

	float best = 1e10;
	for (auto node : to_visit) {
		std::set<int> next_to_visit = to_visit;
		next_to_visit.erase(node);
		auto [dist, _] = getVal(cur_node, node);
		float rest = getBBDistDFS(next_to_visit, ub - dist, node);
		if (rest < 0.0) continue;
		best = fmin(best, dist + rest);
		ub = fmin(best, ub);
	}

	if (best > 1e9) return -1.0;
	return best;
}


std::vector<std::pair<float, float>> Env::getRefGraph()
{
	return m_ref_graph;
}


void Env::processACState()
{
	// Now we allow the ants to continue randomly choosing next steps
	// We gather the pheromones and reweight them so that they add up to 1

	std::uniform_int_distribution ant_next(0, m_num_nodes - 1);

	for (auto& ant : m_ants) {
		std::set<int> to_visit;
		for (int i = 0; i < m_num_nodes; i++) {
			if (i == ant.m_pos) continue;
			else to_visit.insert(i);
		}

		while (!to_visit.empty()) {
			std::uniform_real_distribution distr(0.0, 1.0);
			std::vector<std::pair<int, float>> probs;
			float total = 0.0;
			float running_sum = 0.0;
			float rand_num = distr(m_rng);
			int chosen_node = -1;

			for (auto& node : to_visit) {
				float val = calculatePred(ant.m_pos, node);
				total += val;
				probs.push_back({ node, val });
			}

			for (auto& prob : probs) {
				prob.second = prob.second / total;
			}
			for (auto& prob : probs) {

				prob.second = prob.second + (float)running_sum;

				if (rand_num <= prob.second) {
					chosen_node = prob.first;
					break;
				};
				running_sum = prob.second;
			}


			if (chosen_node == -1) {
				// std::cout << running_sum << ", " << rand_num << "\t";
				chosen_node = probs[probs.size() - 1].first;
			}

			to_visit.erase(chosen_node);
			std::pair<int, int> edge = { ant.m_pos, chosen_node };
			ant.m_visited.insert(edge);
			ant.m_pos = chosen_node;
		}

		ant.m_pos = ant_next(m_rng);
	}

}

void Env::updateACState()
{
	/*
		updates the pheromones from the prcoessACState() iteration.
	*/

	for (auto& ant : m_ants) {

		// get the total distance of each ant's tour.
		float total_dst = 0.0;
		for (auto& edge : ant.m_visited) {
			auto [dst, prev_val] = getVal(edge.first, edge.second);
			total_dst += dst;
		}


		// local pheromone update
		for (auto& edge : ant.m_visited) {
			auto [dst, prev_val] = getVal(edge.first, edge.second);
			setVal(edge.first, edge.second, { dst, prev_val + (1 / total_dst) });
			setVal(edge.second, edge.first, { dst, prev_val + (1 / total_dst) });
		}

		//reset for next main loop iteration
		ant.m_visited.clear();
	}

	//global uniform decay for all pheromone trails.
	for (int i = 0; i < m_num_nodes; i++) {
		for (int j = 0; j < m_num_nodes; j++) {
			auto [dst, prev_val] = getVal(i, j);
			setVal(i, j, { dst, prev_val * PHER_DECA });
		}
	}
}

float Env::calculatePred(int from, int to)
{
	/*
	calcualtes the desirablity of an edge using Ant Coloy
	*/
	auto [dist, pher] = getVal(from, to);
	dist = powf((1 / dist), DIST_PARAM);
	pher = powf(pher, PHER_PARAM);
	if (isnan(pher)) return 1e-5;
	if (isnan(dist)) return 1e-5;
	return (dist * pher);
}
