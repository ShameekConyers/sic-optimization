
#include <iostream>
#include <filesystem>
#include <string>

#include "../src/Env.hpp"
#include "../src/Utils.hpp"
#include "../src/AntSystem.hpp"
#include "../src/AntColonySystem.hpp"
#include "../src/HeuristicDataHandler.hpp"
#include "../src/NearestNeighbor.hpp"
#include "../src/ExactAlgorithms.hpp"


const std::string PROD_DIR = "../data/prod/";
void clean_dir(std::string directory = PROD_DIR)
{
	std::filesystem::remove_all(directory);
}

int main()
{
	clean_dir();

	Env env{};
	HeuristicDataHandler data_handler{};

	std::cout << "Small Graph Started" << "...\n";
	for (int num_nodes = 2; num_nodes <= 10; num_nodes++) {
		std::cout << num_nodes << "...\n";
		for (int scenario = 0; scenario < 30; scenario++) {
			env.clear_env();
			env.initialize_env(num_nodes, scenario);
			std::vector<EnvHeuristic*> heuristic_vec;
			AntSystem ant_system{ env };
			NearestNeighbor one_ahead{ env, 0 };
			BruteForceTSP brute_force_tsp{ env };
			BranchBoundTSP branch_bound_tsp{ env };

			heuristic_vec = {
				&ant_system,
				&one_ahead,
				&brute_force_tsp,
				&branch_bound_tsp
			};
			for (int i = 0; i < heuristic_vec.size(); i++) {
				data_handler.append_results(heuristic_vec[i]);
			}

		}
	}
	data_handler.dump_results("small_graph", PROD_DIR);

	std::cout << "Medium Graph Started" << "...\n";
	for (int num_nodes = 2; num_nodes <= 50; num_nodes = num_nodes + 1) {
		std::cout << num_nodes << "...\n";
		for (int scenario = 0; scenario < 30; scenario++) {
			env.clear_env();
			env.initialize_env(num_nodes, scenario);
			std::vector<EnvHeuristic*> heuristic_vec;
			AntSystem ant_system{ env };
			NearestNeighbor one_ahead{ env, 0 };
			RandomNeighbor random_neighbor{ env };

			heuristic_vec = {
				&ant_system,
				&one_ahead,
				&random_neighbor,
			};


			for (int i = 0; i < heuristic_vec.size(); i++) {
				data_handler.append_results(heuristic_vec[i]);
			}
		}
	}
	data_handler.dump_results("medium_graph", PROD_DIR);
}
