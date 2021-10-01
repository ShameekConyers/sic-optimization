
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
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


const std::string TEST_DIR = "../data/test/";
void clean_dir(std::string directory = TEST_DIR)
{
	std::filesystem::remove_all(directory);
}

TEST_CASE("Env_Setup")
{
	Env env{};
	env.initialize_env(10);
	env.clear_env();

	env.initialize_env(10, 0);
	env.clear_env();

	env.initialize_env(10, 0, 5);
	env.reset_env();
}

TEST_CASE("AntSystem_Setup")
{
	Env env{};
	env.initialize_env(10, 0);
	AntSystem ant_colony{ env };
}

TEST_CASE("HeuristicDataHandler_Setup")
{
	HeuristicDataHandler data_handler{};
}

TEST_CASE("Heuristics")
{


	Env env{};
	HeuristicDataHandler data_handler{};

	SECTION("AntSystem")
	{
		clean_dir();
		env.initialize_env(10, 0);
		AntSystem ant_colony{ env };
		auto res = ant_colony.get_path();
		REQUIRE(res.size() == 11);
		REQUIRE(res[0] == res[10]);


		data_handler.append_results(&ant_colony);
		data_handler.dump_results("ant_test", TEST_DIR);
	}


	SECTION("AntSystem_Complex")
	{
		clean_dir();
		for (int scenario = 0; scenario < 30; scenario++) {
			env.clear_env();
			env.initialize_env(15, scenario + 30);
			AntSystem ant_system{ env };
			AntColonySystem ant_colony_system{ env };
			FarthestNeighbor farthest_neighbor{ env };
			NearestNeighbor nearest_neighbor{ env, 0 };


			data_handler.append_results(&ant_system);
			data_handler.append_results(&ant_colony_system);
			data_handler.append_results(&nearest_neighbor);
			data_handler.append_results(&farthest_neighbor);
		}

		data_handler.dump_results("test", TEST_DIR);
	}


}

TEST_CASE("Env")
{
	Env env{};
	HeuristicDataHandler data_handler{};

	SECTION("Simple")
	{
		clean_dir();
		for (int num_nodes = 2; num_nodes < 12; num_nodes++) {

			env.clear_env();
			env.initialize_env(num_nodes, 0);
			std::vector<EnvHeuristic*> heuristic_vec;
			AntSystem ant_system{ env };
			AntColonySystem ant_colony_system{ env };
			NearestNeighbor one_ahead{ env, 0 };
			NearestNeighbor two_ahead{ env, 1 };
			NearestNeighbor three_ahead{ env, 2 };
			BruteForceTSP brute_force_tsp{ env };
			BranchBoundTSP branch_bound_tsp{ env };

			heuristic_vec = {
				&ant_system,
				&ant_colony_system,
				&one_ahead,
				&two_ahead,
				&three_ahead,
				&brute_force_tsp,
				&branch_bound_tsp
			};


			for (int i = 0; i < heuristic_vec.size(); i++) {
				data_handler.append_results(heuristic_vec[i]);
			}
		}
		data_handler.dump_results("test", TEST_DIR);
	}

	SECTION("Aggregate_Simple")
	{
		clean_dir();

		for (int num_nodes = 2; num_nodes < 8; num_nodes++) {
			for (int scenario = 0; scenario < 5; scenario++) {
				env.clear_env();
				env.initialize_env(num_nodes, scenario);
				std::vector<EnvHeuristic*> heuristic_vec;
				AntSystem ant_system{ env };
				AntColonySystem ant_colony_system{ env };
				NearestNeighbor one_ahead{ env, 0 };
				NearestNeighbor two_ahead{ env, 1 };
				NearestNeighbor three_ahead{ env, 2 };
				RandomNeighbor random_neighbor{ env };
				FarthestNeighbor farthest_neighbor{ env };
				BruteForceTSP brute_force_tsp{ env };
				BranchBoundTSP branch_bound_tsp{ env };

				heuristic_vec = {
					&ant_system,
					&ant_colony_system,
					&one_ahead,
					&two_ahead,
					&random_neighbor,
					&farthest_neighbor,
					&three_ahead,
					&brute_force_tsp,
					&branch_bound_tsp
				};


				for (int i = 0; i < heuristic_vec.size(); i++) {
					data_handler.append_results(heuristic_vec[i]);
				}
			}
		}
		data_handler.dump_results("test", TEST_DIR);
	}

	// SECTION("Aggregate_Complex")
	// {
	// 	clean_dir();

	// 	for (int num_nodes = 20; num_nodes <= 50; num_nodes = num_nodes + 10) {
	// 		for (int scenario = 0; scenario < 30; scenario++) {
	// 			env.clear_env();
	// 			env.initialize_env(num_nodes, scenario);
	// 			std::vector<EnvHeuristic*> heuristic_vec;
	// 			AntSystem ant_system{ env };
	// 			AntColonySystem ant_colony_system{ env };
	// 			NearestNeighbor one_ahead{ env, 0 };
	// 			NearestNeighbor two_ahead{ env, 1 };
	// 			FarthestNeighbor farthest_neighbor{ env };

	// 			heuristic_vec = {
	// 				&ant_system,
	// 				// &ant_colony_system,
	// 				&one_ahead,
	// 				&two_ahead,
	// 				&farthest_neighbor
	// 			};


	// 			for (int i = 0; i < heuristic_vec.size(); i++) {
	// 				data_handler.append_results(heuristic_vec[i]);
	// 			}
	// 		}
	// 	}
	// 	data_handler.dump_results("test", TEST_DIR);
	// }

}
