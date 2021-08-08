#include "Env.hpp"
#include <iostream>
#include <cmath>
#include <chrono>
#include <fstream>
#include <algorithm>

int main()
{
	std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::time_point stamp;
	float time_stamp;
	const float MILLION = 1'000'000.0;
	float num_samples = 30.0;

	int num_tests;
	int max_num_nodes = 30; // for second test

	std::vector<float> results;
	std::vector<float> time_results;


	/*
		Compares the results to the Exact solution
	*/
	for (int i = 2; i < 11; i++) {
		std::cout << "\nLoop: " << i << "\n";

		int ant_colony_sum = 0;
		int nearest_neighbor_sum = 0;
		int k_neighbor_sum = 0;
		int branch_bound_sum = 0;
		int branch_hint_sum = 0;
		int brute_force_sum = 0;

		float ant_colony_time = 0.0;
		float nearest_neighbor_time = 0.0;
		float k_neighbor_time = 0.0;
		float branch_bound_time = 0.0;
		float branch_hint_time = 0.0;
		float brute_force_time = 0.0;



		for (int k = 0; k < num_samples; k++) {
		//Stamp the loop and init a temp env

			int j = std::min(i, 20);
			Env tmpenv{ i, j * 20, j * 10 };
			int test_tracker = 0; // used for delimiting in CSV later


			//Ant Colony
			start = std::chrono::high_resolution_clock::now();
			tmpenv.runACLoop();
			float actmp = tmpenv.getACDist(); // used for upperbound hint for B&B
			ant_colony_sum += actmp;
			stamp = std::chrono::high_resolution_clock::now();
			time_stamp = (stamp - start).count() / MILLION;
			ant_colony_time += time_stamp;
			test_tracker++;


			//Nearest Neighbor
			start = stamp;
			nearest_neighbor_sum += tmpenv.getNNDist();
			stamp = std::chrono::high_resolution_clock::now();
			time_stamp = (stamp - start).count() / MILLION;
			nearest_neighbor_time += time_stamp;
			test_tracker++;


			// One Ahead Nearest
			start = stamp;
			k_neighbor_sum += tmpenv.getOneAheadNNDist();
			stamp = std::chrono::high_resolution_clock::now();
			time_stamp = (stamp - start).count() / MILLION;
			k_neighbor_time += time_stamp;
			test_tracker++;


			// Brach and Bound
			start = stamp;
			branch_bound_sum += tmpenv.getBBDist();
			stamp = std::chrono::high_resolution_clock::now();
			time_stamp = (stamp - start).count() / MILLION;
			branch_bound_time += time_stamp;
			test_tracker++;


			//Brach and Bound with ant hint
			start = stamp;
			branch_hint_sum += tmpenv.getBBDist(actmp);
			stamp = std::chrono::high_resolution_clock::now();
			time_stamp = (stamp - start).count() / MILLION;
			branch_hint_time += time_stamp;
			test_tracker++;


			//Brute Force
			start = stamp;
			brute_force_sum += tmpenv.getBruteForceDist();
			stamp = std::chrono::high_resolution_clock::now();
			time_stamp = (stamp - start).count() / MILLION;
			brute_force_time += time_stamp;
			test_tracker++;

			num_tests = test_tracker; // sets modulo for flat vector
		}
		results.push_back(ant_colony_sum / num_samples);
		results.push_back(nearest_neighbor_sum / num_samples);
		results.push_back(k_neighbor_sum / num_samples);
		results.push_back(branch_bound_sum / num_samples);
		results.push_back(branch_hint_sum / num_samples);
		results.push_back(brute_force_sum / num_samples);

		time_results.push_back(ant_colony_time / num_samples);
		time_results.push_back(nearest_neighbor_time / num_samples);
		time_results.push_back(k_neighbor_time / num_samples);
		time_results.push_back(branch_bound_time / num_samples);
		time_results.push_back(branch_hint_time / num_samples);
		time_results.push_back(brute_force_time / num_samples);
	}


	int count = 0;
	for (auto& item : results) {
		std::cout << item << ",   ";
		if (count % num_tests == (num_tests - 1)) std::cout << "\n";
		count++;
	}

	//convert to csv.
	std::ofstream myfile;
	myfile.open("../data/prelim_dist.csv");

	myfile << "AntColony,NearestNeighbor,OneAheadNearest,BranchBound,BranchAntColony,BruteForce\n";

	for (int i = 0; i < results.size(); i++) {
		if (i % num_tests == (num_tests - 1)) {
			myfile << results[i] << "\n";
		}
		else {
			myfile << results[i] << ",";
		}

	}
	myfile.close();
	myfile.open("../data/prelim_time.csv");

	myfile << "AntColony,NearestNeighbor,OneAheadNearest,BranchBound,BranchAntColony,BruteForce\n";

	for (int i = 0; i < results.size(); i++) {
		if (i % num_tests == (num_tests - 1)) {
			myfile << time_results[i] << "\n";
		}
		else {
			myfile << time_results[i] << ",";
		}

	}
	myfile.close();





	/*
		Redo test for direct comparison between NN and Ant Colony
	*/

	results.clear();
	time_results.clear();

	for (int i = 2; i < max_num_nodes; i++) {

		std::cout << "\nLoop: " << i << "\n";
		int ant_colony_sum = 0;
		int nearest_neighbor_sum = 0;

		float ant_colony_time_sum = 0.0;
		float nearest_neighbor_time_sum = 0.0;

		for (int k = 0; k < num_samples; k++) {


			//Stamp the loop and init a temp env
			int j = std::min(i, 20);
			Env tmpenv{ i, j * 30, j * 5 };
			int test_tracker = 0;


			//Ant Colony
			start = std::chrono::high_resolution_clock::now();
			tmpenv.runACLoop();
			float actmp = tmpenv.getACDist(); // used for upperbound hint for B&B
			ant_colony_sum += actmp;
			stamp = std::chrono::high_resolution_clock::now();
			ant_colony_time_sum += (stamp - start).count() / MILLION;

			test_tracker++;


			//Nearest Neighbor
			start = stamp;
			nearest_neighbor_sum += tmpenv.getNNDist();
			stamp = std::chrono::high_resolution_clock::now();
			nearest_neighbor_time_sum += (stamp - start).count() / MILLION;

			test_tracker++;


			num_tests = test_tracker; // sets modulo for flat vector
		}

		results.push_back(ant_colony_sum / num_samples);
		results.push_back(nearest_neighbor_sum / num_samples);

		time_results.push_back(ant_colony_time_sum / num_samples);
		time_results.push_back(nearest_neighbor_time_sum / num_samples);

	}

	// print distance results
	count = 0;
	for (auto& item : results) {
		std::cout << item << ",   ";
		if (count % num_tests == (num_tests - 1)) std::cout << "\n";
		count++;
	}

	//convert to csv.

	myfile.open("../data/total_distance.csv");
	myfile << "AntColony,NearestNeighbor\n";
	for (int i = 0; i < results.size(); i++) {
		if (i % num_tests == (num_tests - 1)) {
			myfile << results[i] << "\n";
		}
		else {
			myfile << results[i] << ",";
		}
	}
	myfile.close();

	myfile.open("../data/total_time.csv");
	myfile << "AntColony,NearestNeighbor\n";
	for (int i = 0; i < results.size(); i++) {
		if (i % num_tests == (num_tests - 1)) {
			myfile << time_results[i] << "\n";
		}
		else {
			myfile << time_results[i] << ",";
		}

	}
	myfile.close();
}
