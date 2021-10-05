#include "SimulatedAnnealing.hpp"
#include "cassert"
#include "cmath"

SimulatedAnnealing::SimulatedAnnealing(
	Env& env
)
	: EnvHeuristic{ env, "SimulatedAnnealing" },
	m_inital_guess_heuristic{}
{
	m_inital_guess_heuristic = std::make_unique<NearestNeighbor>(env, 0);
}

std::vector<int> SimulatedAnnealing::get_path()
{
	std::vector<int> path = m_inital_guess_heuristic->get_path();
	std::vector<int> candidate = path;
	std::vector<int> best_candidate = path;
	double path_distance = m_env->get_path_distance(path);
	double best_candidate_distance;
	double candidate_distance;
	double temperature = 1.0;
	double temp_decay_param = 0.95;
	double minimum_temperature = 1e-5;
	int number_of_candidates = 100 * m_env->get_num_nodes();

	while (temperature > minimum_temperature) {
		best_candidate_distance = INFINITY;

		for (int i = 0; i < number_of_candidates; i++) {
			populate_candidate_path_(candidate, path);
			candidate_distance = m_env->get_path_distance(candidate);
			if (candidate_distance < best_candidate_distance) {
				best_candidate = candidate;
				best_candidate_distance = candidate_distance;
			}
		}

		if (best_candidate_distance < path_distance) {
			path = best_candidate;
		}
		else {
			process_step_(
				best_candidate, path, temperature, best_candidate_distance, path_distance
			);
		}

		temperature = temperature * temp_decay_param;
	}
	for (auto node : path) {
		std::cerr << node << ", ";
	}
	std::cerr << "\n";
	return path;
}

void SimulatedAnnealing::process_step_(
	std::vector<int>& candidate,
	std::vector<int>& path,
	double temperature,
	double candidate_distance,
	double path_distance
)
{
	double loss = (candidate_distance - path_distance) / path_distance;
	loss = exp(-loss / temperature);
	assert(!isnan(loss));
	std::uniform_real_distribution distribution(0.0, 1.0);
	double threshold = distribution(m_env->get_rng());

	if (loss > threshold) {
		path = candidate;
	}
}

void SimulatedAnnealing::populate_candidate_path_(
	std::vector<int>& candidate_path,
	std::vector<int>& original_path
)
{
	// initial condition
	candidate_path.clear();
	candidate_path.insert(
		candidate_path.end(), original_path.begin(), original_path.end() - 1);

	// To populate a segment
	std::vector<int> segment;
	int num_nodes = m_env->get_num_nodes();

	std::uniform_int_distribution start_node_distr(0, num_nodes - 1);
	int start_node = start_node_distr(m_env->get_rng());

	// Note end_node is open (not included)
	std::uniform_int_distribution end_node_distr(1, num_nodes - start_node);
	int end_node = start_node + end_node_distr(m_env->get_rng());

	segment.insert(
		segment.end(),
		original_path.begin() + start_node,
		original_path.begin() + end_node
	);

	std::uniform_int_distribution distribution(0, 1);
	int coin_toss = distribution(m_env->get_rng());
	if (coin_toss == 0) {
		reverse_segment_(candidate_path, start_node, end_node);
	}
	else {
		transport_segment_(segment, candidate_path, start_node, end_node);
	}

	candidate_path.push_back(candidate_path.front());
	assert(candidate_path.size() == original_path.size());
}


void SimulatedAnnealing::reverse_segment_(
	std::vector<int>& path,
	int segment_begin,
	int segment_end
)
{
	std::reverse(path.begin() + segment_begin, path.begin() + segment_end);
}

void SimulatedAnnealing::transport_segment_(
	std::vector<int>& segment,
	std::vector<int>& path,
	size_t segment_begin,
	size_t segment_end
)
{
	std::vector<int> new_path;
	int upper_bound = path.size() - segment.size();
	std::uniform_int_distribution index_distr(0, upper_bound);
	size_t new_start = index_distr(m_env->get_rng());
	if (new_start < segment_begin) {
		new_path.insert(
			new_path.end(), path.begin(), path.begin() + new_start);
		new_path.insert(
			new_path.end(), segment.begin(), segment.end());
		new_path.insert(
			new_path.end(), path.begin() + new_start, path.begin() + segment_begin);
		new_path.insert(
			new_path.end(), path.begin() + segment_end, path.end());

	}
	else if (new_start == segment_begin) {
		return;
	}
	else {
		new_start = segment.size() + new_start;
		new_path.insert(
			new_path.end(), path.begin(), path.begin() + segment_begin);
		new_path.insert(
			new_path.end(), path.begin() + segment_end, path.begin() + new_start);
		new_path.insert(
			new_path.end(), segment.begin(), segment.end());
		new_path.insert(
			new_path.end(), path.begin() + new_start, path.end());
	}
	// std::cerr << new_path.size() << ", " << path.size() << "\n";
	assert(new_path.size() == path.size());
	path = new_path;
}
