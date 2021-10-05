#include "ExactAlgorithms.hpp"


BruteForceTSP::BruteForceTSP(Env& env)
	: EnvHeuristic{ env, "BruteForceTSP" }
{
	m_best_distance = INFINITY;
	m_best_path = {};
}

std::vector<int> BruteForceTSP::get_path()
{
	std::set<int> to_visit;
	std::set<int> check;
	for (int i = 0; i < m_num_nodes; i++) {
		to_visit.insert(i);
	}

	generate_path_procedure({}, to_visit, 0);

	// auto path = m_best_path;
	// std::cerr << "\n";
	// for (auto node : path) {
	// 	std::cerr << node << ", ";
	// }
	// std::cerr << "\n";
	// for (int i = 1; i < path.size(); i++) {
	// 	std::cerr << m_env->get_val(path[i - 1], path[i]).first << ", ";
	// }
	return m_best_path;
}

void BruteForceTSP::generate_path_procedure(
	std::vector<int> current_path, std::set<int> to_visit, int next_node
)
{
	current_path.push_back(next_node);
	to_visit.erase(current_path.back());
	if (to_visit.size() == 0) {
		current_path.push_back(current_path.front());
		double distance = m_env->get_path_distance(current_path);
		if (distance < m_best_distance) {
			m_best_path = current_path;
			m_best_distance = distance;
		}
		return;
	}
	else {
		for (int node : to_visit) {
			generate_path_procedure(current_path, to_visit, node);
		}
	}
}

BranchBoundTSP::BranchBoundTSP(Env& env)
	: EnvHeuristic{ env,  "BranchBoundTSP" },
	m_initial_guess_heuristic{}
{
	m_initial_guess_heuristic = std::make_unique<NearestNeighbor>(env, 0);
	m_best_distance = INFINITY;
	m_best_path = {};
}

std::vector<int> BranchBoundTSP::get_path()
{
	std::vector<int> bound_path = m_initial_guess_heuristic->get_path();
	double bound_distance = m_env->get_path_distance(bound_path);
	m_best_distance = bound_distance;
	m_best_path = bound_path;

	std::set<int> check;
	std::set<int> to_visit;
	for (int i = 0; i < m_num_nodes; i++) {
		to_visit.insert(i);
	}
	generate_path_procedure({}, to_visit, 0, 0);
	for (auto node : m_best_path) {
		check.insert(node);
	}
	assert(m_best_path.size() == check.size() + 1);
	assert(check.size() == m_num_nodes);


	return m_best_path;
}

void BranchBoundTSP::generate_path_procedure(
	std::vector<int> current_path,
	std::set<int> to_visit,
	int next_node,
	double path_distance
)
{
	if (path_distance > m_best_distance) return;

	current_path.push_back(next_node);
	to_visit.erase(current_path.back());
	if (to_visit.size() == 0) {
		current_path.push_back(current_path.front());
		double distance = m_env->get_path_distance(current_path);
		if (distance < m_best_distance) {
			m_best_path = current_path;
			m_best_distance = distance;
		}
		return;
	}
	else {
		for (int node : to_visit) {
			double next_path_distance =
				path_distance + m_env->get_val(next_node, node).first;
			generate_path_procedure(current_path, to_visit, node, next_path_distance);
		}
	}
}
