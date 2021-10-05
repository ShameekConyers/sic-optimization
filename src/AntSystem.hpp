#pragma once
#include "Env.hpp"

class Timer;
class EnvHeuristic;

inline const double AS_PHEROMONE_PARAM = 0.5;
inline const double AS_PHEROMONE_DECAY = 0.5;
inline const double AS_DISTANCE_PARAM = 2.0;

// child data structure of AntSystem
struct Ant {
	Ant(int initial_position);

	// used to keep track of the edge traversal;
	std::vector<int> m_history;
	// The node the Ant currently is at
	int m_position;
};

// class driver for the Ant Colony algorithm
class AntSystem : public EnvHeuristic {
public:
	AntSystem(
		Env& env,
		double pheromone_parameter = AS_PHEROMONE_PARAM,
		double pheromone_decay = AS_PHEROMONE_DECAY,
		double distance_parameter = AS_DISTANCE_PARAM
	);

	//returns the chosen optimal edge traversal
	std::vector<int> get_path();

private:
	void run_loop(
		int num_steps = 0
	);

	// does 1 step on env
	void process_step();

	void update_heuristic();

	double get_heuristic(
		int from,
		int to
	);

	// number of ants
	int m_num_ants;

	// the collection of ants
	std::vector<Ant> m_ants;

	double m_pheromone_parameter;
	double m_pheromone_decay;
	double m_distance_parameter;

};
