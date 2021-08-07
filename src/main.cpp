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

	int num_tests;
	int max_num_nodes = 40; // for second test

	std::vector<float> results;
	std::vector<float> time_results;

	for (int i = 2; i < 10; i++) {

		//Stamp the loop and init a temp env
		std::cout << "\nLoop: " << i << "\n";
		int j = std::min(i, 20);
		Env tmpenv{ i, j * 30, j * 5 };
		int test_tracker = 0; // used for delimiting in CSV later


		//Ant Colony
		start = std::chrono::high_resolution_clock::now();
		tmpenv.runACLoop();
		float actmp = tmpenv.getACDist(); // used for upperbound hint for B&B
		results.push_back(actmp);
		stamp = std::chrono::high_resolution_clock::now();
		time_stamp = (stamp - start).count() / MILLION;
		time_results.push_back(time_stamp);

		std::cout << time_stamp << "\n";
		test_tracker++;


		//Nearest Neighbor
		start = stamp;
		results.push_back(tmpenv.getNNDist());
		stamp = std::chrono::high_resolution_clock::now();
		time_stamp = (stamp - start).count() / MILLION;
		time_results.push_back(time_stamp);

		std::cout << time_stamp << "\n";
		test_tracker++;


		// One Ahead Nearest
		start = stamp;
		results.push_back(tmpenv.getOneAheadNNDist());
		stamp = std::chrono::high_resolution_clock::now();
		time_stamp = (stamp - start).count() / MILLION;
		time_results.push_back(time_stamp);

		std::cout << time_stamp << "\n";
		test_tracker++;


		// Brach and Bound
		start = stamp;
		results.push_back(tmpenv.getBBDist());
		stamp = std::chrono::high_resolution_clock::now();
		time_stamp = (stamp - start).count() / MILLION;
		time_results.push_back(time_stamp);

		std::cout << time_stamp << "\n";
		test_tracker++;


		//Brach and Bound with ant hint
		start = stamp;
		results.push_back(tmpenv.getBBDist(actmp));
		stamp = std::chrono::high_resolution_clock::now();
		time_stamp = (stamp - start).count() / MILLION;
		time_results.push_back(time_stamp);

		std::cout << time_stamp << "\n";
		test_tracker++;


		//Brute Force
		start = stamp;
		results.push_back(tmpenv.getBruteForceDist());
		stamp = std::chrono::high_resolution_clock::now();
		time_stamp = (stamp - start).count() / MILLION;
		time_results.push_back(time_stamp);

		std::cout << time_stamp << "\n";
		test_tracker++;



		num_tests = test_tracker; // sets modulo for flat vector
	}

	int count = 0;
	for (auto& item : results) {
		std::cout << item << ",   ";
		if (count % num_tests == (num_tests - 1)) std::cout << "\n";
		count++;
	}

	//convert to csv.
	std::ofstream myfile;
	myfile.open("total_distance_1.csv");

	// myfile << "AntColony, NearestNeighbor, OneAheadNearest, BranchBound, BranchAntColony, BruteForce\n";
	myfile << "AnyColony, NearestNeighbor";
	for (int i = 0; i < results.size(); i++) {
		if (i % num_tests == (num_tests - 1)) {
			myfile << results[i] << "\n";
		}
		else {
			myfile << results[i] << ",";
		}

	}
	myfile.close();
	myfile.open("total_time_1.csv");

	// myfile << "AntColony, NearestNeighbor, OneAheadNearest, BranchBound, BranchAntColony, BruteForce\n";
	myfile << "AnyColony, NearestNeighbor";
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

	Redo test for Ant Colony Nearest Neighbor


	*/

	results.clear();
	time_results.clear();

	for (int i = 2; i < max_num_nodes; i++) {

	//Stamp the loop and init a temp env
		std::cout << "\nLoop: " << i << "\n";
		int j = std::min(i, 20);
		Env tmpenv{ i, j * 30, j * 5 };
		int test_tracker = 0;


		//Ant Colony
		start = std::chrono::high_resolution_clock::now();
		tmpenv.runACLoop();
		float actmp = tmpenv.getACDist(); // used for upperbound hint for B&B
		results.push_back(actmp);
		stamp = std::chrono::high_resolution_clock::now();
		time_stamp = (stamp - start).count() / MILLION;
		time_results.push_back(time_stamp);

		std::cout << time_stamp << "\n";
		test_tracker++;


		//Nearest Neighbor
		start = stamp;
		results.push_back(tmpenv.getNNDist());
		stamp = std::chrono::high_resolution_clock::now();
		time_stamp = (stamp - start).count() / MILLION;
		time_results.push_back(time_stamp);

		std::cout << time_stamp << "\n";
		test_tracker++;


		num_tests = test_tracker; // sets modulo for flat vector
	}

	// print distance results
	count = 0;
	for (auto& item : results) {
		std::cout << item << ",   ";
		if (count % num_tests == (num_tests - 1)) std::cout << "\n";
		count++;
	}

	//convert to csv.

	myfile.open("total_distance_2.csv");
	myfile << "AnyColony, NearestNeighbor";
	for (int i = 0; i < results.size(); i++) {
		if (i % num_tests == (num_tests - 1)) {
			myfile << results[i] << "\n";
		}
		else {
			myfile << results[i] << ",";
		}
	}
	myfile.close();

	myfile.open("total_time_2.csv");
	myfile << "AnyColony, NearestNeighbor";
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
