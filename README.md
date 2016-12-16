# EvoSim-Engine

A general purpose evolution engine for evolving algorithms.

##Basic Overview

The EvoSim Engine is a C++ console application designed to simulate evolution by taking inspiration from processes in biology and genetics with the goal being to train "virtual creatures" referred to as "agents". A population of agents can be evolved to solve a specific task by using a fitness function which measures how well each agent is performing. Agents with the highest fitness function will live longer and have more offspring as a result.

Each agent has a collection of arrays such as an input and output array, and also a collection of "modules". The modules perform operations on the arrays based on what "nodes" the module contains. The behavior of a module will depend on the nodes it contains and the overall behavior of an agent will depend on the modules it contains. Nodes come in many different types and allow agents to evolve very complex algorithms.

Many important principles behind evolution, natural selection, survival of the fittest, are modeled by the EvoSim Engine. Some of these principles include gene dominance, speciation and the ability for a species or entire genus to go extinct, modularity and procedural generation, communication between members of a species using a "community array" shared by agents, having two copies of each chromosome to allow creation of "gametes" and "zygotes", etc.

While the EvoSim Engine is fairly advanced compared to many evolutionary algorithms, the goal isn't to be as realistic as possible. The goal is to apply principles which produce the fastest possible rate of evolution by understanding the underlying core concepts and approximating them in an algorithmic fashion. For a more in depth explanation on how the engine works and the reasoning behind it please see the paper linked below:

http://j-d-b.net/files/Evolving_Algorithms-EvoSim_Engine.pdf
