# sic-optimization

For exploring various optimization heuristics.

## Ant Colony Optimization and the Traveling Salesman Problem

Currently the files explore the viability of Ant Colony Optimization (ACO) to solve a Traveling Salesman Problem. Wherin we must
find the shortest route to visit a set of N cities. An example of a usecase is for a delivery company to find the shortest route
to complete its daily log. in root/data the .csv files show that ACO is able to solve problems even up to 50 cities while having 
a predictable increase of computation time. Meanwhile Brute Force becomes untenable at 11 cities and even using Branch and Bound 
with a close upper bound is too unpredictable on its computation time as you never know when devolve into an exhaustive search,
hence in the general case Branch and Bound also becomes untenable around 11 cities. 

Included in the data files we see that as we iterate from 2 cities to 40 ACO is able to consistently perform better than the Nearest Neighbor
Heuristic. Of course one cannot beat the Nearest Neighbor Heuristic in terms of speed but if one needs to have a better prediction of the
shortest tour along N cities Ant Colony might be a viable alternative. 

