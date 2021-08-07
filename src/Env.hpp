#pragma once

#include <unordered_map>
#include <utility>
#include <set>
#include <vector>
#include <random>


class Ant {
public:
	Ant(int init_pos);

	std::set<std::pair<int, int>> m_visited; // clear this after every loop iteration
	int m_pos;
};

class EnvNode {
public:
	int x;
	int y; //X and y coords

};

class MappedEnv {

	// Key: (Node1, Node2) ... Value: Cost
	std::unordered_map<
		std::set<EnvNode, EnvNode>,
		std::pair<float, float>>
		m_graph;
};

class Env {
public:
	Env(int num_nodes, int num_ants, int num_steps);

	//returns (Dist, Pheromone) for graph
	std::pair<float, float> getVal(int i, int j);
	void setVal(int i, int j,
		std::pair<float, float> val);

	void runACLoop();
	std::vector<std::pair<int, int>> generateACO();
	float getACDist();
	float getBruteForceDist();
	float getBruteForceDistBFS(std::set<int>& to_visit, int cur_node);
	float getNNDist(bool complex = true);
	float getOneAheadNNDist(bool complex = true);
	float getBBDist(float upper_bound_hint = 0.0);
	float getBBDistDFS(std::set<int>& to_visit, float ub, int cur_node);
	std::vector<std::pair<float, float>> getRefGraph();

private:
	void processACState(); //make each ant pick
	void updateACState(); //update pheromones
	float calculatePred(int from, int to);



	//flat map where m_ref_graph contains the  (X, Y) coord
	// m_graph contains the precomputed distance and pheromone levels
	// m_probabilies contain the state change probabilites
	std::vector<
		std::pair<float, float>> m_ref_graph;
	std::vector<
		std::pair<float, float>> m_graph;
	int m_num_nodes;

	//collection of ants
	std::vector<Ant> m_ants;

	//utility
	std::mt19937 m_rng;
	int m_num_steps;
};
