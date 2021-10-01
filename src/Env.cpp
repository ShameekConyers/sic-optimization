#include "Env.hpp"
#include <cassert>

#include <iostream>
/*
	flat vector which holds the original (X, Y) graph with stride 2
	std::vector<double> m_ref_graph;

	flat vector which holds the (obj dist, heur dist) graph, has stride 2
	std::vector<double> m_graph;

	number of nodes in fully connected graph
	int m_num_nodes;

	number of times you need to move until new node, const for now
	const int m_stride = 2;

	utility
	std::mt19937 m_rng;

*/


Env::Env()
	: m_ref_graph{}, m_graph{}, m_num_nodes{ 0 }, m_stride{ 2 }, m_rng{}

{

}

void Env::initialize_env(int num_nodes, int seed, int num_orders)
{
	m_num_nodes = num_nodes;
	if (seed == -1) {
		// gives random seed if one isn't provided
		std::random_device rd_seed;
		std::uniform_int_distribution rd_seed_distr(0, 100'000'000);
		seed = rd_seed_distr(rd_seed);

	}
	m_rng.seed(seed);
	m_ref_graph.reserve(m_stride * num_nodes);
	m_graph.reserve(m_stride * num_nodes * num_nodes);


	//constructs random coordinates for our graph, assume uniform distribution
	//note we are using a flat vector with stride for our graph
	std::uniform_real_distribution x_distr(0.0, 100.0);
	std::uniform_real_distribution y_distr(0.0, 100.0);

	for (int i = 0; i < m_num_nodes; i++) {
		m_ref_graph.push_back(x_distr(m_rng));
		m_ref_graph.push_back(y_distr(m_rng));
	}


	//uses the ref graph to construct a graph with distances
	for (int i = 0; i < m_num_nodes; i++) {
		double x1 = m_ref_graph[i * m_stride];
		double y1 = m_ref_graph[i * m_stride + 1];

		for (int j = 0; j < m_num_nodes; j++) {

			double x2 = m_ref_graph[j * m_stride];
			double y2 = m_ref_graph[j * m_stride + 1];

			double dist = sqrt(
				pow(x1 - x2, 2.0)
				+ pow(y1 - y2, 2.0)
			);

			m_graph.push_back(dist);
			m_graph.push_back(0.0); // inits (dist, heuristic)
		}
	}

	assert(m_graph.size() == (m_stride * num_nodes * num_nodes));


	//constructs a random set of orders, if none given we simply assign node 0
	//as the pickup and destination as all others (TSP problem)
	//note std::distr is inclusive hence we subtract 1
	std::uniform_int_distribution order_distr(0, m_num_nodes - 1);
	if (num_orders == -1) {
		for (int i = 1; i < m_num_nodes; i++) {
			m_orders.push_back({ 0, i });
		}
	}
	else {
		for (int i = 0; i < num_orders; i++) {
			int from = order_distr(m_rng);
			int to = from;
			while (to == from) {
				to = order_distr(m_rng);
			}
			m_orders.push_back({ from, to });
		}
	}

}

void Env::run_loop(int num_steps)
{
	// Not used...
	// currently due to the variable nature of of heuristics it makes more sense
	// to let the actor mutate the heuristic and then clear it
	for (int i = 0; i < num_steps; i++) {
		step_env();
	}
}

void Env::step_env()
{
	// Not used...
	process_state();
	update_state();
	generate_output();
}

std::pair<double, double> Env::get_val(int i, int j)
{

	return { m_graph[(i * m_num_nodes * m_stride) + j * m_stride],
		m_graph[(i * m_num_nodes * m_stride) + j * m_stride + 1] };
}

void Env::set_val(int i, int j, std::pair<double, double> new_val)
{
	m_graph[(i * m_num_nodes * m_stride) + j * m_stride] = new_val.first;
	m_graph[(i * m_num_nodes * m_stride) + j * m_stride + 1] = new_val.second;
}

double Env::get_heuristic(int i, int j)
{
	return m_graph[(i * m_num_nodes * m_stride) + j * m_stride + 1];
}

void Env::set_heuristic(int i, int j, double new_heuristic_value)
{
	m_graph[(i * m_num_nodes * m_stride) + j * m_stride + 1] = new_heuristic_value;
}

std::vector<double>& Env::get_ref_graph()
{
	return m_ref_graph;
}

std::vector<double>& Env::get_graph()
{
	return m_graph;
}

void Env::clear_env()
{
	m_ref_graph.clear();
	m_graph.clear();
	m_num_nodes = 0;
	m_orders.clear();
}

void Env::reset_env()
{
	// only necessary to reset heuristic
	for (int i = 0; i < m_num_nodes; i++) {
		m_graph[i * m_stride + 1] = 0.0;
	}
}

double Env::get_path_distance(std::vector<int> guess)
{
	// returns the distance travelled given a set of gueses
	double result = 0.0;
	for (int i = 1; i < guess.size(); i++) {
		result += get_val(guess[i - 1], guess[i]).first;
	}

	return result;
}

std::vector<std::pair<int, int>>& Env::get_orders()
{
	return m_orders;
}

int Env::get_stride()
{
	return m_stride;
}

int Env::get_num_nodes()
{
	return m_num_nodes;
}

std::mt19937& Env::get_rng()
{
	return m_rng;
}

void Env::process_state()
{

}

void Env::update_state()
{

}

void Env::generate_output()
{

}
