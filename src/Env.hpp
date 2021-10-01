#pragma once

#include <vector>
#include <utility>
#include <random>
#include <set>
#include <memory>
#include <string>
#include "Utils.hpp"

class Timer;

// Allows the interface for algorithms to interact with
class Env {
public:
	Env();

	// give env shape and seed env
	void initialize_env(int num_nodes, int seed = -1, int num_orders = -1);

	//used when number of steps known ahead of time
	void run_loop(int num_steps);

	//used when the number of steps is unknown, steps once.
	void step_env();

	// returns (obj dist, heur dist)  between node i from node j
	std::pair<double, double> get_val(int i, int j);

	// sets the weighted edge for have (distance, heuristic)
	void set_val(int i, int j, std::pair<double, double> new_val);

	// gets the heuristic between i and j
	double get_heuristic(int i, int j);

	// sets the weighted edge to have a heuristic
	void set_heuristic(int i, int j, double new_heuristic_value);

	//returns a reference to the coord graph, mutation not reccomended.
	std::vector<double>& get_ref_graph();

	//returns a reference to the dist graph, mutation not reccomended.
	std::vector<double>& get_graph();

	// clears the envs state, doesn't reinit env
	void clear_env();

	// resarts env with the same state it was initialized with
	void reset_env();

	// returns the performance given the path in nodes to follow.
	double get_path_distance(std::vector<int> path);

	// returns the (pickup, delivery) node locations as a nested vector of pairs
	std::vector<std::pair<int, int>>& get_orders();

	// returns stride
	int get_stride();

	// returns nodes
	int get_num_nodes();

	// returns the rng
	std::mt19937& get_rng();

private:
	// utility
	std::mt19937 m_rng;

	// Loop helpers
	void process_state();
	void update_state();
	void generate_output();

	//flat vector which holds the original (X, Y) graph with stride 2
	std::vector<double> m_ref_graph;

	//flat vector which holds the (obj dist, heur dist) graph, has stride 2
	std::vector<double> m_graph;

	// edge pair-vector which holds the (pickup, delivery) locations
	std::vector<std::pair<int, int>> m_orders;

	// number of nodes in fully connecteted graph
	int m_num_nodes;


	// number of times you need to move until new node, const for now
	const int m_stride = 2;


};

class EnvHeuristic {
public:
	EnvHeuristic(
		Env& env, std::string name
	)
		: m_env{ &env },
		m_timer{},
		m_num_nodes{},
		m_name{ name }
	{
		m_env->reset_env();
		m_num_nodes = m_env->get_num_nodes();
	}

	virtual std::vector<int> get_path()
	{
		return {};
	}

	std::pair<double, double> get_performance()
	{
		m_timer.set_timer();
		double distance = m_env->get_path_distance(get_path());
		double time = m_timer.get_timer();
		return { distance, time };
	}

	/**
	 * @brief Get the results object
	 *
	 * @return A tuple of (num_nodes, name, (dist, time))
	 */
	virtual std::tuple<int, std::string, std::pair<double, double>> get_results()
	{
		return { m_num_nodes, m_name, get_performance() };
	}

protected:
	Env* m_env;
	int m_num_nodes;
	std::string m_name;
	Timer m_timer;
};


// class driver for the Simulated Annealing algorithm
class SimulatedAnnealing {

};

// class driver for the Genetic algorithm
class GeneticAlgorithm {

};

// class driver for the Swarm algorithm
class ParticleSwarm {

};
