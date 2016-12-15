#pragma once
#include <list>
#include <queue>
#include <array>
#include <memory>
#include <iostream>
#include <fstream>
#include "EvoAgent.h"

using namespace std;

class Species {
	friend class Population;
protected:
	list<unique_ptr<EvoAgent>> agentPool;
	list<unique_ptr<EvoAgent>>::iterator apItr;
	list<unique_ptr<EvoAgent>>::iterator prItr;
	queue<string> unusedIDs;
	uint32_t comArrSize;
	cint* communeArray;
public:
	string id;
	uint32_t radResist;
	uint32_t agentCount;
	double avgFitness;
	double topFitness;
public:
	Species();
	Species(uint32_t radRes);
	~Species();
	void Generate(uint32_t aC, uint32_t cS);
	void ResetIter();
	EvoAgent& NextAgent();
	void KillAgent();
	void PushAgent(EvoAgent* const agent);
};

class Genus {
	friend class Population;
protected:
	list<unique_ptr<Species>> speciPool;
	list<unique_ptr<Species>>::iterator spItr;
	list<unique_ptr<Species>>::iterator prItr;
	queue<string> unusedIDs;
public:
	string id;
	uint32_t speciCount;
	double avgFitness;
	double topFitness;
public:
	Genus(uint32_t sC);
	Genus(uint32_t sC, uint32_t rR);
	void ResetIter();
	Species& NextSpecies();
	void KillSpecies();
	void PushSpecies(Species* const species);
};

class Population {
private:
	list<unique_ptr<Genus>> genusPool;
	list<unique_ptr<Genus>>::iterator gpItr;
	list<unique_ptr<Genus>>::iterator prItr;
	queue<string> unusedIDs;
public:
	uint32_t genusCount;
	uint32_t generation;
	double avgFitness;
	double topFitness;
public:
	/////////////////////////////////////////////
	//          POPULATION CONSTRUCTOR         //
	// Description: create an empty population //
	// object. Useful with LoadBestAgents()    //
	/////////////////////////////////////////////
	Population();
	
	/////////////////////////////////////////////
	//          POPULATION CONSTRUCTOR         //
	// Description: generates a population of  //
	// random agents ready for evolving.       //
	// --------------------------------------- //
	// gCnt: genus count                       //
	// aCnt: number of agents per species      //
	// sCnt: number of species per genus       //
	// lMax: computation limit for agents      //
	// rMax: max mutation resistance           //
	// wMax: max size of working array         //
	// hMax: max size of herritable array      //
	// cMax: max size of community array       //
	// oSize: size of output array             //
	// iConfig: input configuration object     //
	// iArray: pointers to input array(s)      //
	/////////////////////////////////////////////
	Population(uint32_t gCnt, uint32_t aCnt, uint32_t sCnt, uint32_t lMax, uint32_t rMax,
	uint32_t wMax, uint32_t hMax, uint32_t cMax, uint32_t oSize, InputConfig iConfig, cint** iArray);
	
	/////////////////////////////////////////////
	//             LOAD BEST AGENTS            //
	// Description: loads agents saved by the  //
	// SaveBestAgents() function. Uses the two //
	// best agents saved from each species as  //
	// adam and eve to regenerate the species. //
	// --------------------------------------- //
	// geneticsDir: directory to load from     //
	// aCnt: number of agents per species      //
	// oSize: size of output array             //
	// iConfig: input configuration object     //
	// iArray: pointers to input array(s)      //
	/////////////////////////////////////////////
	void LoadBestAgents(string geneticsDir, uint32_t aCnt,
	uint32_t oSize, InputConfig iConfig, cint** iArray);
	
	/////////////////////////////////////////////
	//             SAVE BEST AGENTS            //
	// Description: saves to file the genetic  //
	// data of the two best agents in each     //
	// species. geneticsDir should already     //
	// exist and inside that folder should be  //
	// two folders named '1st' and '2nd'.      //
	// --------------------------------------- //
	// geneticsDir: directory to save files    //
	/////////////////////////////////////////////
	void SaveBestAgents(const string geneticsDir);
	
	/////////////////////////////////////////////
	//             LOAD AGENT BATCH            //
	// Description: loads multiple batches of  //
	// agents so you don't have to call        //
	// LoadBestAgents() many times. This is    //
	// useful because it lets you load a large //
	// population of agents you have already   //
	// trained instead of starting with random //
	// agents. When using SaveBestAgents()     //
	// give each folder the same name but with //
	// an increasing number eg BATCH0, BATCH1  //
	// --------------------------------------- //
	// dir: directory containing batch folders //
	// bCnt: number of batch folders           //
	// aCnt: number of agents per species      //
	// oSize: size of output array             //
	// iConfig: input configuration object     //
	// iArray: pointers to input array(s)      //
	/////////////////////////////////////////////
	void LoadAgentBatch(string dir, uint32_t bCnt, uint32_t aCnt,
	uint32_t oSize, InputConfig iConfig, cint** iArray);
	
	void PrintFitVals();
	void ResetIter();
	Genus& NextGenus();
	void KillGenus();
	void PushGenus(Genus* const genus);
	void KillPopulation();
	void RunIteration();
	void GenOffspring();
	void PrepGeneration();
	
	// CUSTOM FUNCTIONS BELOW
	void PrepAgent(EvoAgent& agent);
	void AssessAgent(EvoAgent& agent);
};