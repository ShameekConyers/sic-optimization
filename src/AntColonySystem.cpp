#include "AntColonySystem.hpp"
#include "AntSystem.hpp"
#include <map>
#include <utility>
#include <random>

AntColonySystem::AntColonySystem(
	Env& env,
	double pheromone_parameter,
	double pheromone_decay,
	double distance_parameter
)
	:EnvHeuristic{ env, "AntColonySystem" },
	m_num_ants{},
	m_ants{},
	m_global_best_distance{ INFINITY },
	m_pheromone_parameter{},
	m_pheromone_decay{},
	m_distance_parameter{}
{
	// Initialize lower and upper pheromone bounds
	// Maximum chosen s.t. 95% is the maximum chance of being picked
	m_minimum_pheromone = 1e-4;
	m_maximum_pheromone = m_minimum_pheromone * m_num_nodes * 1000;


	// Linearly distribute the ant along all nodes
	m_num_ants = m_num_nodes * 20;
	m_ants.reserve(m_num_ants);
	for (int i = 0; i < m_num_ants; i++) {
		m_ants.push_back({ i % m_num_nodes });
	}

	initialize_heuristic();
}

void AntColonySystem::run_loop(
	int num_steps
)
{
	if (num_steps <= 0) {
		num_steps = 50;
	}
	for (int i = 0; i < num_steps; i++) {
		process_step();
		update_heuristic_global();
		m_global_best_path = find_best_heuristic_path();
		m_global_best_distance = m_env->get_path_distance(m_global_best_path);
	}
}

std::vector<int> AntColonySystem::get_path()
{
	run_loop();
	return find_best_heuristic_path();
}

std::vector<int> AntColonySystem::find_best_heuristic_path()
{
	assert(m_global_best_path.size() == m_num_nodes + 1);

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

	double best_path_distance = m_env->get_path_distance(best_path);
	if (best_path_distance < m_global_best_distance) return best_path;
	return m_global_best_path;

}

void AntColonySystem::process_step()
{
	for (Ant& ant : m_ants) {
		int inital_position = ant.m_position;
		ant.m_history = { inital_position };
		std::map<int, double> to_visit{}; // (node, heuristic criterion)
		std::set<int> validator{};

		for (int i = 0; i < m_num_nodes; i++) {
			validator.insert(i);
			if (i == inital_position) continue;
			to_visit[i] = 0.0;
		}

		std::uniform_real_distribution distribution(0.0, 1.0 - 1e-10);

		while (!to_visit.empty()) {
			double probability_threshold = distribution(m_env->get_rng());
			int chosen_node = -1; // invalid
			double chosen_distance = INFINITY;
			double total_heuristic = 0.0;
			double cumulative_probability = 0.0;


			// first pass to get total heuristic values
			for (auto& node : to_visit) {
				node.second = 1.0 / get_heuristic(
					ant.m_position,
					node.first
				);
				total_heuristic += node.second;
			}

			// second pass to select node;
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
			// update tour with our chosen node
			ant.m_history.push_back(chosen_node);
			ant.m_position = chosen_node;
			to_visit.erase(chosen_node);
		}

		// Let ant return to original node and update pheromones
		ant.m_history.push_back(inital_position);
		ant.m_position = inital_position;
		assert(validator.size() + 1 == ant.m_history.size());
		double tour_distance = m_env->get_path_distance(ant.m_history);
		if (tour_distance < m_global_best_distance) {
			m_global_best_distance = tour_distance;
			m_global_best_path = ant.m_history;
		}
		update_heuristic_from_path(ant.m_history, tour_distance);
		update_heuristic_from_path(m_global_best_path, m_global_best_distance);
	}
}

void AntColonySystem::initialize_heuristic()
{
	for (int i = 0; i < m_num_nodes; i++) {
		for (int j = 0; j < m_num_nodes; j++) {
			m_env->set_heuristic(i, j, m_maximum_pheromone / 2.0);
		}
	}
}



void AntColonySystem::update_heuristic_global()
{
	for (int i = 0; i < m_num_nodes; i++) {
		for (int j = 0; j < m_num_nodes; j++) {
			double old_pheromone = m_env->get_heuristic(i, j);
			double new_pheromone =
				old_pheromone -
				(abs(old_pheromone - m_minimum_pheromone) * (1 - m_pheromone_decay));
			m_env->set_heuristic(i, j, new_pheromone);
		}
	}
}

void AntColonySystem::update_heuristic_from_path(
	std::vector<int>& path,
	double path_distance
)
{
	for (int node = 1; node < path.size(); node++) {
		double old_pheromone = m_env->get_heuristic(
			path[node - 1], path[node]
		);
		double new_pheromone =
			old_pheromone + (1.0 / pow(path_distance, 2.0));
		new_pheromone = std::min(new_pheromone, m_maximum_pheromone);
		m_env->set_heuristic(
			path[node - 1],
			path[node],
			new_pheromone
		);
	}
}

double AntColonySystem::get_heuristic(
	int from,
	int to
)
{
	// Smaller is better
	if (from == to) return 0.0;
	auto [distance, pheromone] = m_env->get_val(from, to);
	distance = pow(distance, m_distance_parameter);
	double heuristic = pow(1.0 / pheromone, m_pheromone_parameter);
	return distance * heuristic;
}
