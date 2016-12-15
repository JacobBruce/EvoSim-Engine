#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cassert>
#include "ExtFuncs.h"
#include "Evolve.h"

using namespace std;

double GLOBALS::health_decay	=	0.001; //higher = agents die faster
uint32_t GLOBALS::mutate_step	=	128; //higher = lower number of of mutations
uint8_t GLOBALS::mutate_shift	=	20;  //higher = single mutations have more affect
uint8_t GLOBALS::mutate_shirr	=	20;  //higher = more variation in mutation resistance
uint8_t GLOBALS::chromo_shift	=	4;   //higher = more variation in chromosome counts
uint8_t GLOBALS::max_chromos	=	20;  //maximum number of agent chromosomes
uint8_t GLOBALS::max_mnodes		=	50;  //maximum number of nodes per module
uint8_t GLOBALS::max_cmods		=	99;  //maximum number of modules per chromosome
uint8_t GLOBALS::max_cseeds		=	20;  //maximum number of module seeds per chromosome

inline void LoadInputSets(const uint32_t setCount, uint32_t* setSize)
{
	cout << "Loading input data ...\n\n";
	//NOTE: here you should load all input data into a global
	//array so it can be accessed quickly in SetInputs()
	//Also save size of each set into setSize
}

inline void SetInputs(cint** inArr, uint32_t s, uint32_t i)
{
	//NOTE: here you should use data loaded by LoadInputSets()
	//to fill any shared input arrays used by all agents
}

int main()
{
	//NOTE: the following code is a non-working example of
	//how to use this framework, you should read the
	//comments properly then modify the code so it works
	//the way you need it to. See the Resource.h file
	//to change the definition of the cint type. The
	//inputs and outputs of an agent must be an integer
	//type because of how they work. If you have floating
	//point values you can convert them into integers
	//before feeding them to agents as input.

	uint32_t genCnt = 50; //number of generations to run
	uint32_t inpCnt = 2; //number of agent input arrays
	uint32_t setCnt = 10; //number of training data sets
	uint32_t* setSize; //holds size of each training set
	uint32_t oSize = 50; //size of agent output array

	setSize = new uint32_t[setCnt]; //allocate ui32 array
	LoadInputSets(setCnt, setSize); //load all input data

	InputConfig iConfig; //create input configuration object
	iConfig.inputCount = inpCnt; //save number of input arrays to config
	iConfig.inputSizes = new uint32_t[inpCnt]; //list of input array sizes
	iConfig.inputAlloc = new bool[inpCnt]; //list of input array types

	iConfig.inputSizes[0] = 50;		//size of the first input array
	iConfig.inputSizes[1] = 50;		//size of the second input array
	iConfig.inputAlloc[0] = false;	//false = shared input array
	iConfig.inputAlloc[1] = true;	//true = seperate array for each agent

	cint** inputArray = new cint*[inpCnt]; //array of pointers to the input arrays
	inputArray[0] = new cint[iConfig.inputSizes[0]]; //allocate shared input array
	inputArray[1] = nullptr; //not shared so will be allocated by the agent

	mt19937 mt_rand = SeededRandomEngine(); //used to pick random training sets
	uniform_int_distribution<int> set_dist(0, setCnt-1);

	// initialize a new random population (see Evolve.h)
	Population pop = Population(5, 10, 5, 20000, 512, 256,
					 256, 256, oSize, iConfig, inputArray);

	// enter tha main training/evolution loop
	for (;pop.generation < genCnt; ++pop.generation)
	{
		uint32_t setIndex = set_dist(mt_rand); // pick a random training set
		pop.PrepGeneration(); // prepare agents for new generation (see Evolve.cpp)

		cout << "Running generation " << pop.generation << " on set " << setIndex << " ...\n";

		// loop through each entry in the training set
		for (uint32_t e=0; e < setSize[setIndex]; e++)
		{
			// fill input arrays with data from entry e
			SetInputs(inputArray, setIndex, e);
			pop.RunIteration(); // run the current iteration
		}

		pop.PrintFitVals(); //print fitness values for each species

		// print fitness values for entire population
		cout << "Population fitness:\n";
		cout << "Top: " << pop.topFitness << ", Avg: " << pop.avgFitness << "\n\n";
	}

	// save dna of two best agents from each species
	// BATCH0 folder must already exist with two
	// folders inside it named '1st' and '2nd'
	pop.SaveBestAgents("C:\\GENETICS_DIR\\BATCH0\\");

	return 0;
}