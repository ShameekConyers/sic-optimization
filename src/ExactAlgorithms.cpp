#include "ExactAlgorithms.hpp"


BruteForceTSP::BruteForceTSP(Env& env)
	: EnvHeuristic{ env, "BruteForceTSP" }
{
	counter = 0;
}

std::vector<int> BruteForceTSP::get_path()
{
	std::set<int> to_visit;
	std::set<int> check;
	for (int i = 0; i < m_num_nodes; i++) {
		to_visit.insert(i);
	}
	double best_dist = INFINITY;
	std::vector<int> best_path;
	// for (auto node : to_visit) {
	// 	auto path = get_path_procedure(to_visit, node, true);
	// 	double path_dist = m_env->get_path_distance(path);
	// 	if (path_dist < best_dist) {
	// 		best_path = path;
	// 	}
	// }
	// for (auto node : best_path) {
	// 	check.insert(node);
	// }
	best_path = get_path_procedure(to_visit, 0, true);
	// assert(check.size() == best_path.size() - 1);
	// assert(best_path.front() == best_path.back());
	// assert(best_path.size() == to_visit.size() + 1);
	std::cerr << m_num_nodes << ", " << counter << "\n";
	return best_path;
}

std::vector<int> BruteForceTSP::get_path_procedure(
	std::set<int> to_visit, int current_node, bool top_flag
)
{
	counter++;
	if (to_visit.size() == 1) return { current_node };
	double best_distance = INFINITY;
	std::vector<int> best_path;
	std::vector<int> path_tmp;
	std::vector<int> path;


	// Finds the best path from our current node to the rest of the nodes

	to_visit.erase(current_node);
	for (int node : to_visit) {

		path_tmp = get_path_procedure(to_visit, node);

		path = { current_node };
		path.insert(path.end(), path_tmp.begin(), path_tmp.end());
		if (top_flag) {
			path.push_back(current_node);
		}
		double distance = m_env->get_path_distance(path);
		if (distance < best_distance) {
			best_path = path;
			best_distance = distance;
		}
	}
	to_visit.insert(current_node);

	return best_path;
}

BranchBoundTSP::BranchBoundTSP(Env& env)
	: EnvHeuristic{ env,  "BranchBoundTSP" },
	m_initial_guess_heuristic{}
{
	m_initial_guess_heuristic = std::make_unique<NearestNeighbor>(env, 0);
}

std::vector<int> BranchBoundTSP::get_path()
{
	auto [_1, _2, _3] = m_initial_guess_heuristic->get_results();
	double bound_distance = _3.first;
	std::vector<int> best_path;
	std::set<int> check;
	std::set<int> to_visit;
	for (int i = 0; i < m_num_nodes; i++) {
		to_visit.insert(i);
	}
	best_path = get_path_procedure(to_visit, 0, bound_distance);
	for (auto node : best_path) {
		check.insert(node);
	}
	assert(best_path.size() == check.size() + 1);
	assert(check.size() == m_num_nodes);
	return best_path;
}

std::vector<int> BranchBoundTSP::get_path_procedure(
	std::set<int> to_visit,
	int current_node,
	double bound_distance
)
{
	if (bound_distance < 0) return {};
	if (to_visit.size() == 1) return { current_node };

	double best_distance = INFINITY;
	double distance;
	std::vector<int> best_path = {};
	std::vector<int> tmp_path = {};
	std::vector<int> path = {};


	to_visit.erase(current_node);

	// To get the best path out of all possible candidates to visit
	for (int node : to_visit) {
		double next_distance = bound_distance -
			m_env->get_val(current_node, node).first;

		// If unoptimal path continues, else prepares the path vector
		path = { current_node };
		tmp_path = get_path_procedure(to_visit, node, next_distance);
		if (tmp_path.size() == 0) continue;
		path.insert(path.end(), tmp_path.begin(), tmp_path.end());
		if (current_node == 0) path.push_back(current_node);

		// To update our bounded distance and best distance.
		distance = m_env->get_path_distance(path);
		if (distance <= best_distance) {
			best_path = path;
			best_distance = distance;
		}
		if (distance < bound_distance) {
			bound_distance = distance;
		}
	}

	return best_path;
}
