import plotly.graph_objects as pltly_go
import plotly.express as pltly_ex
import pandas as pd
pd.options.plotting.backend = "plotly"


import os

# if not os.path.exists("images"):
#     os.mkdir("images")





df = pd.read_csv("./data/prelim_dist.csv")
res_df = pd.DataFrame()
res_df['Ant Colony'] = (df['AntColony'] - df['BranchBound'])/ df['BranchBound'] * 100
res_df['Nearest Neighbor'] = (df['NearestNeighbor'] - df['BranchBound'])/ df['BranchBound'] * 100

fig = res_df.plot(
	title = "Percent Error from optimial Solution",
	labels = dict(index = "Number of Cities", value = "Error", 	variable = "Series"),
)
fig.update_yaxes(tickprefix="%")
fig.write_image("figures/prelim_dist_figure.png")




df = pd.read_csv("./data/total_distance.csv")
# df = pltly_ex.data.gapminder().query("continent=='Oceania'")

res_df = (-df['AntColony'] + df['NearestNeighbor']) /((df['AntColony'] + df['NearestNeighbor'])/2) * 100
print(df.columns)

fig = res_df.plot(
	title = "Distance of Nearest Neighbor distance vs Ant Colonoy",
	labels = dict(index = "Number of Cities", value = "Difference of distances", 	variable = "Nearest Neighbor"),
)
fig.update_yaxes(tickprefix="%")
fig.write_image("figures/total_dist_figure.png")



df = pd.read_csv("./data/total_time.csv")
fig = df.plot(
	title = "Time to Find Solution",
	labels = dict(index = "Number of Cities", value = "Million CPU cycles", 	variable = "Any Colony"),
)
fig.update_yaxes(tickprefix="%")
fig.write_image("figures/total_time.png")
