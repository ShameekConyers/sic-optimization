#include "AntSystem.hpp"

#include <map>
#include <cassert>
#include <iostream>



/*
	Ant:
		std::set < std::pair<int, int>> m_visited;
		int m_position;

	AntSystem:
		Env* m_env;
		int m_num_ants;
		std::vector<Ants> m_ants;
*/

Ant::Ant(int initial_position)
	: m_position{ initial_position },
	m_history{} {
}

AntSystem::AntSystem(
	Env& env,
	double pheromone_parameter,
	double pheromone_decay,
	double distance_parameter)
	: EnvHeuristic{ env, "AntSystem" },
	m_num_ants{},
	m_ants{},
	m_pheromone_parameter{ pheromone_parameter },
	m_pheromone_decay{ pheromone_decay },
	m_distance_parameter{ distance_parameter } {

	// we linearly distribute the ants along nodes
	m_num_ants = m_num_nodes * 20;
	m_ants.reserve(m_num_ants);
	for (int i = 0; i < m_num_ants; i++) {
		m_ants.push_back({ i % m_num_nodes });
	}
}

void AntSystem::run_loop(
	int num_steps
)
{
	if (num_steps <= 0) {
		num_steps = 50;
	}
	for (int i = 0; i < num_steps; i++) {
		process_step();
		update_heuristic();
	}
}

std::vector<int> AntSystem::get_path()
{
	// Constructs a single Ant heavily weighted toward pheromones and returns
	//it's path


	run_loop();

	Ant ant{ 0 };
	std::vector<int> best_path = { 0 };
	std::set<int> check;

	std::set<int> to_visit{};
	for (int i = 0; i < m_num_nodes; i++) {
		check.insert(i);
		if (i == ant.m_position) continue;
		to_visit.insert(i);
	}

	// While we still have nodes to visit we continually generate the
	while (!to_visit.empty()) {
		double best_heuristic = INFINITY;
		int best_node;

		for (auto& node : to_visit) {
			double heuristic = get_heuristic(ant.m_position, node);
			if (heuristic <= best_heuristic) {
				best_node = node;
				best_heuristic = heuristic;
			}
		}

		// Update tour and take out chosen node from map
		best_path.push_back(best_node);
		ant.m_position = best_node;
		to_visit.erase(best_node);

	}
	best_path.push_back(best_path.front());
	assert(check.size() + 1 == best_path.size());
	// std::cerr << "\nAnt\n\t";
	// for (auto node : best_path) {
	// 	std::cerr << node << ", ";
	// }
	return best_path;
}


void AntSystem::process_step()

{


	// We construct a to-visit set of nodes we need to visit for each ant

	for (Ant& ant : m_ants) {

		int inital_position = ant.m_position;
		ant.m_history = { inital_position };
		std::map<int, double> to_visit{}; // (node, heuristic_probability)
		std::set<int> check;

		for (int i = 0; i < m_num_nodes; i++) {
			check.insert(i);
			if (i == inital_position) continue;
			to_visit[i] = 0.0;
		}

		// While we still have nodes to visit we continually generate the
		// probability of choosing edge e in the set E
		// Note for every iteration we remove one node and must update


		std::uniform_real_distribution distr(0.0, 1.0 - 1e-10);

		while (!to_visit.empty()) {
			double probability_threshold = distr(m_env->get_rng());
			int chosen_node = -1;
			double chosen_dist = -1;
			double total_heuristic = 0.0;
			double cumulative_probability = 0.0;


			// First pass to get total heuristic values
			for (auto& node : to_visit) {
				node.second = 1.0 / get_heuristic(
					ant.m_position,
					node.first
				);
				total_heuristic += node.second;
			}

			// Second pass to select node
			for (auto& node : to_visit) {
				node.second = node.second / total_heuristic + cumulative_probability;


				if (node.second >= probability_threshold) {
					chosen_node = node.first;
					break;
				}
				else {
					cumulative_probability = node.second;
				}
			}

			assert(chosen_node != -1);
			// Update tour and take out chosen node from map
			ant.m_history.push_back(chosen_node);
			ant.m_position = chosen_node;
			to_visit.erase(chosen_node);
		}


		//finally let the ant return to its original node
		ant.m_history.push_back(inital_position);
		ant.m_position = inital_position;
		assert(check.size() + 1 <= ant.m_history.size());
	}
}

void AntSystem::update_heuristic()
{
	// catalog all necessary changes of the heurstic field data of the env
	std::map<std::pair<int, int>, double> new_edges;
	for (Ant& ant : m_ants) {
		double ant_tour_distance = m_env->get_path_distance(ant.m_history);

		for (int node = 1; node < ant.m_history.size(); node++) {
			std::pair<int, int> edge = {
				ant.m_history[node - 1], ant.m_history[node] };

			if (new_edges.count(edge)) {
				new_edges[edge] += 1.0 / ant_tour_distance;
			}
			else {
				new_edges[edge] = 1.0 / ant_tour_distance;

			}
		}

		//clean up
		ant.m_history.clear();
		ant.m_position = (ant.m_position + 1) % m_num_nodes;
	}

	//execute changes
	for (int i = 0; i < m_num_nodes; i++) {
		for (int j = 0; i < m_num_nodes; i++) {
			double old_pheromone = m_env->get_heuristic(i, j);
			m_env->set_heuristic(i, j, old_pheromone * m_pheromone_decay);
		}
	}
	for (auto& weighted_edge : new_edges) {
		int i = weighted_edge.first.first;
		int j = weighted_edge.first.second;
		double old_pheromone = m_env->get_heuristic(i, j);
		double new_pheromone = old_pheromone + weighted_edge.second;

		m_env->set_heuristic(i, j, new_pheromone);
	}
}

double AntSystem::get_heuristic(
	int from,
	int to
)
{
	// Smaller is better
	if (from == to) return 0.0;
	auto [distance, pheromone] = m_env->get_val(from, to);
	distance = pow(distance, m_distance_parameter);
	pheromone = std::max(pheromone, 1e-7);
	double heuristic = pow(1.0 / pheromone, m_pheromone_parameter);
	return distance * heuristic;

}
