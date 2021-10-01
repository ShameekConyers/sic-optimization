#pragma once

#include <vector>
#include <utility>
#include <map>
#include <string>
#include <fstream>
#include <filesystem>
#include <cassert>
#include <numeric>

class EnvHeuristic;

/*
	vector data_structure where:
		index of pairs where:
			index 1 is num_nodes,
			index 2 is heuristic name
			data (dist, time)
*/
class HeuristicDataHandler {
public:
	HeuristicDataHandler()
		: m_data{},
		m_table{}
	{

	}

	/*
		Expect a tuple of:
		num_nodes,
		heuristic_name,
		std::pair<double, double> of dist, time,
	*/
	void append_results(EnvHeuristic* env_heuristic)
	{

		auto [env_num_nodes, heuristic_name, dist_time_pair] =
			env_heuristic->get_results();
		assert(env_num_nodes != 0);
		// find the index into the m_data pair
		int data_index = 0;
		if (m_table.count(env_num_nodes) == 0) {
			for (auto& node_data : m_data) {
				if (node_data.first == env_num_nodes) {
					m_table[env_num_nodes] = data_index;
					break;
				}
				data_index++;
			}
			if (data_index == m_data.size()) {
				m_data.push_back({ env_num_nodes, {} });
				m_table[env_num_nodes] = data_index;
			}
		}
		else {
			data_index = m_table[env_num_nodes];
		}

		// add to the pair
		m_data[data_index].second[heuristic_name].first.push_back(
			dist_time_pair.first);
		m_data[data_index].second[heuristic_name].second.push_back(
			dist_time_pair.second);
	}

	// writes it's contents into two csv files of dist/time to a local data folder
	void dump_results(std::string test_name = "heur",
		std::string data_dir = "../data/")
	{
		if (m_data.empty()) return;

		std::filesystem::create_directories(data_dir);
		std::ofstream myfile;
		std::string dist_csv = data_dir + test_name + "_dist.csv";
		std::string time_csv = data_dir + test_name + "_time.csv";

		std::sort(m_data.begin(), m_data.end());

		int num_cols = m_data[0].second.size();
		int counter = 0;

		// dist csv writer
		myfile.open(dist_csv);

		//Write dist col names
		myfile << "num_nodes";
		for (auto& heur_tests : m_data[0].second) {
			myfile << "," << heur_tests.first;
		}
		myfile << "\n";

		//Write dist col data
		for (auto& node_data : m_data) {
			myfile << node_data.first;
			for (auto& heur_tests : node_data.second) {
				double num_nodes = heur_tests.second.first.size();
				double sum = std::accumulate(
					heur_tests.second.first.begin(),
					heur_tests.second.first.end(),
					0.0
				);
				double mean = sum / num_nodes;
				myfile << "," << mean;
			}
			myfile << "\n";
		}
		myfile.close();


		// time in million CPU cylces writer
		myfile.open(time_csv);

		//Write col names
		myfile << "num_nodes";
		for (auto& heur_tests : m_data[0].second) {
			myfile << "," << heur_tests.first;
		}
		myfile << "\n";

		//Write time col data
		for (auto& node_data : m_data) {
			myfile << node_data.first;
			for (auto& heur_tests : node_data.second) {
				double num_nodes = heur_tests.second.second.size();
				double sum = std::accumulate(
					heur_tests.second.second.begin(),
					heur_tests.second.second.end(),
					0.0
				);
				double mean = sum / num_nodes;
				myfile << "," << mean;
			}
			myfile << "\n";
		}

		myfile.close();
		m_data.clear();
		m_table.clear();
	}

private:

	//Allows sorting
	std::vector<
		std::pair<int,
		std::map<std::string,
		std::pair<std::vector<double>, std::vector<double>>
		>>> m_data;

	// table for node_num to index;
	std::map<int, int> m_table;
};
