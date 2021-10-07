import pathlib
import os
import plotly.express as px
import pandas as pd

DATA_DIR = "./data/prod/"

def get_csv(file):
	data_dir = pathlib.Path.joinpath(pathlib.Path.cwd(), DATA_DIR).resolve()
	df = pd.read_csv(data_dir / file, index_col="num_nodes")
	df = df.rename({"NearestNeighbor_0_Ahead": "NearestNeighbor"}, axis = 1)

	return df


def create_graph(
	input_df,
	title,yaxis_title,
	name,
	legend_title = "Heuristic"
	):

	fig = px.line(input_df,
	 x=input_df.index, y =  input_df.columns[:])
	fig.update_layout(
		title = title,
		yaxis_title = yaxis_title,
		xaxis_title = "Number of Nodes",
		legend_title = "Heuristic"
	)
	destination_path = pathlib.Path\
		.joinpath(pathlib.Path.cwd(), "./figures/{}.png".format(name)).resolve()
	fig.write_image(destination_path, width = 600, height = 400)

def main():

	small_time_df = get_csv("small_graph_time.csv")
	small_dist_df = get_csv("small_graph_dist.csv")
	medium_time_df = get_csv("medium_graph_time.csv")\
		.drop("RandomNeighbor", axis = 1)
	medium_dist_df = get_csv("medium_graph_dist.csv")\
		.drop("RandomNeighbor", axis = 1)
	motivation_dist_df = get_csv("medium_graph_dist.csv")\
		[["NearestNeighbor", "RandomNeighbor"]]

	large_time_df = get_csv("large_graph_time.csv")
	large_dist_df = get_csv("large_graph_dist.csv")

	create_graph(
		small_dist_df,
		"Heuristic Distance Comparison",
		"Avg. Distance of Solution",
		"small_dist")
	create_graph(
		small_time_df,
		"Heuristic Time Comparison",
		"Avg. Time (Million CPU Cycles)",
		"small_time")
	create_graph(
		motivation_dist_df,
		"Heuristic Distance Comparison",
		"Avg. Distance of Solution",
		"motivation_dist")
	create_graph(
		medium_dist_df,
		"Heuristic Distance Comparison",
		"Avg. Distance of Solution",
		"medium_dist")
	create_graph(
		medium_time_df,
		"Heuristic Time Comparison",
		"Avg. Time (Million CPU Cycles)",
		"medium_time")
	create_graph(
		large_time_df,
		"Simulated Annealing Time at Scale",
		"Avg. Time (Million CPU Cycles)",
		"large_time")
	create_graph(
		large_dist_df,
		"Simulated Annealing Distance at Scale",
		"Avg. Distance of Solution",
		"large_dist")



if __name__ == "__main__":
	main()
