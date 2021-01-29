# GeneticAlgorithm - Shortest Path
Project made for the Genetic Algorithms class

## The problem
Find the shortest path between between two positions in a map/matrix,
following these movement conditions:

- An individual may only move one position at a time (diagonals included);

- Visiting a position already visited before is allowed.
![untitled(6)](https://user-images.githubusercontent.com/37164779/106311712-564b3680-6244-11eb-831b-ac76a4f2c1da.png)

# The algorithm
The algorithm is made out of 4 steps:
- Create initial population;

- Fitness function;

- Crossover;

- Mutation;

# Fitness function
Given a threshold, only select individuals which path length is less or equal
than the given threshold.

# Crossover
Given intersection points of two individuals, create a new one combining the segments
of the parents at random.
![BbeCs](https://user-images.githubusercontent.com/37164779/106311334-be4d4d00-6243-11eb-9e04-8e7288d0bb75.png)

# Mutation
The mutation is made by selecting part of the individual's path and creating a new one, 
with a new random length, increasing variability.

# Explaining video (PT-BR)
https://drive.google.com/file/d/1oM82fZIgWDyuzrUFMGKBfOJk3v6g30qv/view?usp=sharing
