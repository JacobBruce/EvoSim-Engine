#pragma once
#include "ExtFuncs.h"
#include "Evolve.h"

using namespace std;

static uint32_t ita_deaths=0;
static uint32_t its_deaths=0;

// ------------------------------------------- //
// ---------  START SPECIES METHODS  --------- //
// ------------------------------------------- //

Species::Species() : radResist(0ui32) {}
Species::Species(uint32_t radRes) : radResist(radRes) {}
Species::~Species() { delete[] communeArray; }

void Species::Generate(uint32_t aC, uint32_t cS)
{
	communeArray = new cint[cS];

	agentCount = aC;
	comArrSize = cS;

	for (uint32_t a=0; a < aC; a++) {
		agentPool.push_front(unique_ptr<EvoAgent>(new EvoAgent()));
	}
}

void Species::ResetIter()
{
	apItr = agentPool.begin();
}

EvoAgent& Species::NextAgent()
{
	prItr = apItr++;
	return **prItr;
}

void Species::KillAgent()
{
	EvoAgent& agent = **prItr;
	unusedIDs.push(agent.id);
	//cout << "Agent " + agent.id + " died after " +
	//	IntToStr(agent.itrCnt) + " iterations.\n";
	agentPool.erase(prItr);
	--agentCount;
	++ita_deaths;
}

void Species::PushAgent(EvoAgent* const agent)
{
	//cout << "Agent born into species " << id << "\n";
	if (unusedIDs.empty()) {
		agent->id = id + "-" + IntToStr(agentCount);
	} else {
		agent->id = unusedIDs.front();
		unusedIDs.pop();
	}
	agentPool.push_front(unique_ptr<EvoAgent>(agent));
	++agentCount;
}

// ------------------------------------------- //
// ----------  START GENUS METHODS  ---------- //
// ------------------------------------------- //

Genus::Genus(uint32_t sC) : speciCount(sC)
{
	for (uint32_t s=0; s < sC; s++) {
		speciPool.push_front(unique_ptr<Species>(new Species()));
	}
}

Genus::Genus(uint32_t sC, uint32_t rR) : speciCount(sC)
{
	srand(time(NULL)+rand());
	for (uint32_t s=0; s < sC; s++) {
		rR += rand_range(-GLOBALS::mutate_shirr, GLOBALS::mutate_shirr);
		speciPool.push_front(unique_ptr<Species>(new Species(rR)));
	}
}

void Genus::ResetIter()
{
	spItr = speciPool.begin();
}

Species& Genus::NextSpecies()
{
	prItr = spItr++;
	return **prItr;
}

void Genus::KillSpecies()
{
	Species& species = **prItr;
	unusedIDs.push(species.id);
	cout << "Species " + species.id + " went extinct!\n";
	speciPool.erase(prItr);
	--speciCount;
	++its_deaths;
}

void Genus::PushSpecies(Species* const species)
{
	if (unusedIDs.empty()) {
		species->id = id + "-" + IntToStr(speciCount);
	} else {
		species->id = unusedIDs.front();
		unusedIDs.pop();
	}
	speciPool.push_front(unique_ptr<Species>(species));
	++speciCount;
}

// ------------------------------------------- //
// -------  START POPULATION METHODS  -------- //
// ------------------------------------------- //

Population::Population() : genusCount(0), generation(0) {}

Population::Population(uint32_t gCnt, uint32_t aCnt, uint32_t sCnt, uint32_t lMax, uint32_t rMax,
uint32_t wMax, uint32_t hMax, uint32_t cMax, uint32_t oSize, InputConfig iConfig, cint** iArray) :
	genusCount(0), generation(0)
{
	uint32_t wSize, hSize, cSize, cLim, rRes;
	uint8_t chrCnt, mMin, mMax, dMin, dMax;

	mt19937 mt_rand = SeededRandomEngine();

	uniform_int_distribution<int> cm_dist(1, cMax);
	uniform_int_distribution<int> wk_dist(1, wMax);
	uniform_int_distribution<int> hr_dist(1, hMax);
	uniform_int_distribution<int> cl_dist(1, lMax);
	uniform_int_distribution<int> rr_dist(0, rMax);
	uniform_int_distribution<int> ch_dist(1, GLOBALS::max_chromos);
	uniform_int_distribution<int> md_dist(1, GLOBALS::max_cmods);
	uniform_int_distribution<int> sd_dist(1, GLOBALS::max_cseeds);
	uniform_int_distribution<int> cs_dist(-GLOBALS::chromo_shift, GLOBALS::chromo_shift);

	for (uint32_t g=0; g < gCnt; g++)
	{
		cout << "Generating genus " << g << " ...\n";

		chrCnt = ch_dist(mt_rand);
		cSize = cm_dist(mt_rand);
		wSize = wk_dist(mt_rand);
		hSize = hr_dist(mt_rand);
		cLim = cl_dist(mt_rand);
		rRes = rr_dist(mt_rand);

		mMin = md_dist(mt_rand);
		mMax = md_dist(mt_rand);
		dMin = sd_dist(mt_rand);
		dMax = sd_dist(mt_rand);

		if (mMin > mMax) { swap(mMin, mMax); }
		if (dMin > dMax) { swap(dMin, dMax); }

		Genus* pGenus = new Genus(sCnt, rRes);
		Genus& genus = *pGenus;

		PushGenus(pGenus);
		genus.ResetIter();

		for (uint32_t s = 0; s < sCnt; ++s)
		{
			Species& species = genus.NextSpecies();

			species.id = IntToStr(g) + "-" + IntToStr(s);
			cout << "Generating species " << species.id << " ...\n\n";

			chrCnt = max(1, chrCnt + cs_dist(mt_rand));

			species.Generate(aCnt, cSize);
			species.ResetIter();

			EvoAgent adam = EvoAgent(iArray, iConfig, oSize, wSize,
							hSize, chrCnt, cLim, mMin, dMin, mMax, dMax);

			Chromosome* GX = adam.CreateGamete();
			Chromosome* GY = adam.CreateGamete();

			EvoAgent eve = EvoAgent(GX, GY, iArray, iConfig,
						   cLim, chrCnt, oSize, wSize, hSize, false);

			for (uint32_t a = 0; a < aCnt; ++a)
			{
				EvoAgent& agent = species.NextAgent();

				agent.id = species.id + "-" + IntToStr(a);
				cout << "Generating agent " << agent.id << " ...\n";

				agent.CopyAgent(adam.CreateZygote(eve));
				agent.BuildFromDNA(species.communeArray, cSize, species.radResist);
			}

			eve.FreeProto(false);
			cout << "\n";
		}
	}
}

void Population::LoadBestAgents(string geneticsDir, uint32_t aCnt,
				 uint32_t oSize, InputConfig iConfig, cint** iArray)
{
	size_t bpos; uint32_t cSize;
	string line, adamID, eveID;
	string filename = geneticsDir+"agent_list.txt";
	ifstream agentList(filename);

	if (agentList.is_open()) {

		getline(agentList, line);
		uint32_t gc = stoi(line);

		for (uint32_t g=0; g < gc; ++g)
		{
			getline(agentList, line);
			uint32_t sc = stoi(line);

			Genus* pGenus = new Genus(sc);
			Genus& genus = *pGenus;

			PushGenus(pGenus);
			genus.ResetIter();

			cout << "Importing genus " << genus.id << " ...\n";

			for (uint32_t s=0; s < sc; ++s)
			{
				bool asexual = false;
				Species& species = genus.NextSpecies();

				getline(agentList, line);
				bpos = line.find(" ");

				species.radResist = stoi(line.substr(0, bpos));
				cSize = stoi(line.substr(bpos+1));

				getline(agentList, line);
				bpos = line.find(" ");

				adamID = line.substr(0, bpos);
				eveID = line.substr(bpos+1);

				if (eveID == "NONE") {
					asexual = true;
					eveID = adamID;
				}

				species.id = genus.id + "-" + IntToStr(s);
				cout << "Importing species " << species.id << " ...\n\n";

				species.Generate(aCnt, cSize);
				species.ResetIter();

				EvoAgent adam = EvoAgent(iArray, iConfig, oSize);
				EvoAgent eve = EvoAgent(iArray, iConfig, oSize);

				adam.LoadGenome(geneticsDir, adamID, true);
				eve.LoadGenome(geneticsDir, eveID, asexual);

				for (uint32_t a = 0; a < aCnt; ++a)
				{
					EvoAgent& agent = species.NextAgent();

					agent.id = species.id + "-" + IntToStr(a);
					cout << "Generating agent " << agent.id << " ...\n";

					if (a % 2) {
						agent.CopyAgent(eve.CreateZygote(adam));
						agent.BuildFromDNA(species.communeArray, cSize, species.radResist);
					} else {
						agent.CopyAgent(adam.CreateZygote(eve));
						agent.BuildFromDNA(species.communeArray, cSize, species.radResist);
					}
				}

				adam.FreeProto(true);
				eve.FreeProto(true);
				cout << "\n";
			}
		}

		agentList.close();

	} else {

		cout << "Cannot open file: " << filename << "\n";
	}
}

void Population::SaveBestAgents(const string geneticsDir)
{
	cout << "Saving best agents to file ...\n";
	ofstream agentList(geneticsDir+"agent_list.txt");
	agentList << genusCount << "\n";

	for (auto gi = genusPool.begin(); gi != genusPool.end(); ++gi)
	{
		Genus& genus = **gi;
		genus.ResetIter();

		agentList << genus.speciCount << "\n";

		for (uint32_t s=0; s < genus.speciCount; ++s)
		{
			Species& species = genus.NextSpecies();
			species.ResetIter();

			EvoAgent* fstAgent = nullptr;
			EvoAgent* sndAgent = nullptr;

			double fHigh = 0.0;
			double sHigh = 0.0;

			for (uint32_t a=0; a < species.agentCount; ++a)
			{
				EvoAgent& agent = species.NextAgent();
				PrepAgent(agent);

				if (agent.fitness > fHigh) {
					sndAgent = fstAgent;
					sHigh = fHigh;
					fstAgent = &agent;
					fHigh = agent.fitness;
				} else if (agent.fitness > sHigh) {
					sndAgent = &agent;
					sHigh = agent.fitness;
				}
			}

			agentList << int32_t(species.radResist) << " ";
			agentList << species.comArrSize << "\n";

			fstAgent->SaveGenome(geneticsDir+"1st\\");

			if (sndAgent == nullptr) {
				agentList << fstAgent->id << " NONE\n";
			} else {
				sndAgent->SaveGenome(geneticsDir+"2nd\\");
				agentList << fstAgent->id << " " << sndAgent->id << "\n";
			}
		}
	}

	agentList.close();
}

void Population::LoadAgentBatch(string dir, uint32_t bCnt,
uint32_t aCnt, uint32_t oSize, InputConfig iConfig, cint** iArray)
{
	for (uint32_t b=0; b < bCnt; ++b) {
		LoadBestAgents(dir+IntToStr(b)+"\\", aCnt, oSize, iConfig, iArray);
	}
}

void Population::PrintFitVals()
{
	for (auto gi = genusPool.begin(); gi != genusPool.end(); ++gi)
	{
		Genus& genus = **gi;
		genus.ResetIter();

		for (uint32_t s=0; s < genus.speciCount; ++s)
		{
			Species& species = genus.NextSpecies();
			species.ResetIter();

			cout << "Species " << species.id << " fitness:\n";
			cout << "Top: " << species.topFitness << ", Avg: " << species.avgFitness << "\n";
		}
	}
}

void Population::ResetIter()
{
	gpItr = genusPool.begin();
}

Genus& Population::NextGenus()
{
	prItr = gpItr++;
	return **prItr;
}

void Population::KillGenus()
{
	Genus& genus = **prItr;
	unusedIDs.push(genus.id);
	cout << "Genus " + genus.id + " went extinct!\n";
	genusPool.erase(prItr);
	--genusCount;
	++its_deaths;
}

void Population::PushGenus(Genus* const genus)
{
	if (unusedIDs.empty()) {
		genus->id = IntToStr(genusCount);
	} else {
		genus->id = unusedIDs.front();
		unusedIDs.pop();
	}
	genusPool.push_front(unique_ptr<Genus>(genus));
	++genusCount;
}

void Population::KillPopulation()
{
	genusCount = 0;
	generation = 0;
	genusPool.clear();
}

void Population::RunIteration()
{
	bool sDead, gDead;
	uint32_t sCnt, aCnt, gCnt;

	ita_deaths = 0;
	its_deaths = 0;

	avgFitness = 0.0;
	topFitness = 0.0;

	ResetIter();
	gCnt = genusCount;

	for (uint32_t g=0; g < gCnt; ++g)
	{
		Genus& genus = NextGenus();
		genus.ResetIter();

		genus.avgFitness = 0.0;
		genus.topFitness = 0.0;

		sCnt = genus.speciCount;
		gDead = false;

		for (uint32_t s=0; s < sCnt; ++s)
		{
			Species& species = genus.NextSpecies();
			species.ResetIter();

			species.topFitness = 0.0;
			species.avgFitness = 0.0;

			aCnt = species.agentCount;
			sDead = false;

			for (uint32_t a=0; a < aCnt; ++a)
			{
				EvoAgent& agent = species.NextAgent();

				agent.ComputeOutput();
				AssessAgent(agent);

				if (agent.health <= 0.0) {
					species.KillAgent();
					if (species.agentCount == 0) {
						if (genusCount > 1) {
							genus.KillSpecies();
							sDead = true;
							if (genus.speciCount == 0) {
								KillGenus();
								gDead = true;
							}
							break;
						} else {
							if (genus.speciCount > 1) {
								genus.KillSpecies();
								sDead = true;
								break;
							}
						}
					}
					continue;
				}

				species.avgFitness += agent.fitness;

				if (agent.fitness > species.topFitness) {
					species.topFitness = agent.fitness;
				}
			}

			if (sDead) { continue; }

			if (species.topFitness > genus.topFitness) {
				genus.topFitness = species.topFitness;
			}

			species.avgFitness /= species.agentCount;
			genus.avgFitness += species.avgFitness;
		}

		if (gDead) { continue; }

		if (genus.topFitness > topFitness) {
			topFitness = genus.topFitness;
		}

		genus.avgFitness /= genus.speciCount;
		avgFitness += genus.avgFitness;
	}

	avgFitness /= genusCount;

	GenOffspring();
}
void Population::GenOffspring()
{
	uint32_t randInt;
	uint32_t aHghst;
	uint32_t sHghst;
	uint32_t gHghst;
	uint32_t nHghst;

	EvoAgent* nAgent=nullptr;
	EvoAgent* pAgent=nullptr;
	Species* pSpecies=nullptr;
	Genus* pGenus=nullptr;

	queue<EvoAgent*> offspring;
	queue<Species*> speciTargs;

	srand(time(NULL));

	//TODO: create new species when one species is too full?
	//OR: random chance of new species appearing?
	//NOTE: this probably wont be worth the effort
	its_deaths = 0;

	while (ita_deaths-- > 0) {

		aHghst = 0;
		sHghst = 0;
		gHghst = 0;
		nHghst = 0;

		for (auto gi = genusPool.begin(); gi != genusPool.end(); ++gi)
		{
			Genus& genus = **gi;
			double fit_power = genus.avgFitness * genus.topFitness;
			randInt = rand() % uint32_t((fit_power + 1e-4) * 1e4);
			if (randInt >= gHghst) {
				genus.ResetIter();
				gHghst = randInt;
				pGenus = &genus;
			}
		}

		Genus& rGenus = *pGenus;

		for (uint32_t s=0; s < rGenus.speciCount; ++s)
		{
			Species& species = rGenus.NextSpecies();
			double fit_power = species.avgFitness * species.topFitness;
			randInt = rand() % uint32_t((fit_power + 1e-4) * 1e4);
			if (randInt >= sHghst) {
				species.ResetIter();
				sHghst = randInt;
				pSpecies = &species;
			}
		}

		Species& rSpecies = *pSpecies;

		if (rSpecies.agentCount > 1) {
			for (uint32_t a=0; a < rSpecies.agentCount; ++a)
			{
				EvoAgent& agent = rSpecies.NextAgent();
				randInt = rand() % uint32_t((agent.fitness + 1e-4) * 1e4);
				if (randInt >= aHghst) {
					nHghst = aHghst;
					nAgent = pAgent;
					aHghst = randInt;
					pAgent = &agent;
				} else if (randInt > nHghst) {
					nHghst = randInt;
					nAgent = &agent;
				}
			}
		} else {
			EvoAgent& agent = rSpecies.NextAgent();
			nAgent = pAgent = &agent;
		}

		EvoAgent* child = new EvoAgent();
		EvoAgent& rChild = *child;
			
		rChild.CopyAgent(pAgent->CreateZygote(*nAgent));
		rChild.BuildFromDNA(rSpecies.communeArray, rSpecies.comArrSize, rSpecies.radResist);
		PrepAgent(rChild);

		offspring.push(child);
		speciTargs.push(pSpecies);
	}

	while (offspring.size() > 0) {
		Species& species = *speciTargs.front();
		species.PushAgent(offspring.front());
		speciTargs.pop();
		offspring.pop();

	}
}

void Population::PrepGeneration()
{
	for (auto gi = genusPool.begin(); gi != genusPool.end(); ++gi)
	{
		Genus& genus = **gi;
		genus.ResetIter();

		for (uint32_t s=0; s < genus.speciCount; ++s)
		{
			Species& species = genus.NextSpecies();
			species.ResetIter();

			for (uint32_t a=0; a < species.agentCount; ++a)
			{
				EvoAgent& agent = species.NextAgent();
				PrepAgent(agent);
			}
		}
	}
}

// ------------------------------------------- //
// -------  CUSTOM POPULATION METHODS  ------- //
// ------------------------------------------- //

void Population::PrepAgent(EvoAgent& agent)
{
	//NOTE: here you should do anything
	//that needs to be done at the start
	//of each new generation when a new
	//training data set is used
}

void Population::AssessAgent(EvoAgent& agent)
{
	//NOTE: here you should update the fitness value for the agent.
	//It's often a good idea to compute a fitness value for each
	//iteration and then use the average of those values to determine
	//the final fitness value. You can use agent.itrCnt to get the
	//number of iterations an agent has lived through. Just add a
	//new member to the Agent class to store the sum of each fitness
	//value and then divide it by itrCnt. Some example code:

	//agent.fitSum += FitnessValueForThisIteration;
	//agent.fitness = agent.fitSum / agent.itrCnt;

	// do this last to decrease agent health proportional to fitness
	agent.health -= GLOBALS::health_decay / pow(agent.fitness, 2.0);
}