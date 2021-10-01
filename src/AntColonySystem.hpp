#pragma once
#include "Env.hpp"

struct Ant;

inline const double ACS_PHEROMONE_PARAM = 0.5;
inline const double ACS_PHEROMONE_DECAY = 0.7;
inline const double ACS_DISTANCE_PARAM = 3.0;


class AntColonySystem : public EnvHeuristic {
public:
	AntColonySystem(
		Env& env,
		double pheromone_parameter = ACS_PHEROMONE_PARAM,
		double pheromone_decay = ACS_PHEROMONE_DECAY,
		double distance_parameter = ACS_DISTANCE_PARAM
	);
	void run_loop(
		int num_steps = 0
	);

	std::vector<int> get_path();

	std::vector<int> find_best_heuristic_path();
private:
	void process_step();

	void initialize_heuristic();

	void update_heuristic_global();
	void update_heuristic_from_path(
		std::vector<int>& path,
		double path_distance
	);

	double get_heuristic(
		int from,
		int to
	);

	int m_num_ants;
	std::vector<Ant> m_ants;

	std::vector<int> m_global_best_path;
	double m_global_best_distance;
	double m_minimum_pheromone;
	double m_maximum_pheromone;

	double m_pheromone_parameter;
	double m_pheromone_decay;
	double m_distance_parameter;

};



/*
	ACS
	Elitist Ant System
	MMAS
	ASrank
*/
