#pragma once
#include <ctime>
#include <queue>
#include <random>
#include "Modules.h"
#include "Structs.h"

using namespace std;

/// -------------------------------- ///
/// ----- EVOLVING AGENT CLASS ----- ///
/// -------------------------------- ///

class EvoAgent {
private:
	Chromosome* genomeX;
	Chromosome* genomeY;

	Module** modulesX;
	Module** modulesY;

	bool autoFree;

public:
	string id;

	uint8_t chrCount;
	uint32_t radResist;
	uint32_t compLimit;
	uint32_t itrCnt;

	double health;
	double fitness;

	cint** inputArrSet;  // read only
	cint* outputArr;     // write only
	cint* communeArr;    // read/write
	cint* workingArr;    // read/write
	cint* heritableArr;  // read/write

	InputConfig inputConfig;

	uint32_t outArrSize;
	uint32_t wrkArrSize;
	uint32_t herArrSize;
	uint32_t comArrSize;

public:
	EvoAgent() :
		autoFree(false),
		itrCnt(0ui32),
		health(1.0),
		genomeX(nullptr),
		genomeY(nullptr),
		modulesX(nullptr),
		modulesY(nullptr)
	{}
	EvoAgent(cint** iArray, InputConfig iConfig, uint32_t oSize, uint32_t wSize, uint32_t hSize,
	uint8_t cSize, uint32_t cLim, uint8_t mMin, uint8_t sMin, uint8_t mMax, uint8_t sMax) :
		autoFree(true),
		itrCnt(0ui32),
		health(1.0),
		inputConfig(iConfig),
	    chrCount(cSize),
		outArrSize(oSize),
		wrkArrSize(wSize),
		herArrSize(hSize),
		compLimit(cLim),
		modulesX(nullptr),
		modulesY(nullptr)
	{
		inputArrSet = new cint*[iConfig.inputCount];
		outputArr = new cint[oSize];
		workingArr = new cint[wSize];
		heritableArr = new cint[hSize];

		genomeX = new Chromosome[cSize];
		genomeY = new Chromosome[cSize];

		ui32ui8 randBytes;
		mt19937 mt_rand = SeededRandomEngine();

		uniform_int_distribution<int> int_dist(INT32_MIN, INT32_MAX);
		uniform_int_distribution<int> byte_dist(0, UINT8_MAX);

		uniform_int_distribution<int> mc_dist(mMin, mMax);
		uniform_int_distribution<int> sc_dist(sMin, sMax);

		for (uint32_t i=0; i < iConfig.inputCount; ++i) {
			if (iConfig.inputAlloc[i]) {
				inputArrSet[i] = new cint[iConfig.inputSizes[i]];
				fill_n(inputArrSet[i], inputConfig.inputSizes[i], 0);
			} else {
				inputArrSet[i] = iArray[i];
			}
		}

		for (uint8_t c=0; c < cSize; c++)
		{
			genomeX[c].mod_count = mc_dist(mt_rand);
			genomeX[c].seed_count = sc_dist(mt_rand);

			genomeX[c].mod_start = new uint8_t[genomeX[c].mod_count];
			genomeX[c].mod_impact = new uint8_t[genomeX[c].mod_count];
			genomeX[c].mod_inout = new uint32_t[genomeX[c].mod_count];
			genomeX[c].mod_ioind = new uint32_t[genomeX[c].mod_count];
			genomeX[c].mod_inind = new uint8_t[genomeX[c].mod_count];
			genomeX[c].mod_index = new uint8_t[genomeX[c].mod_count];
			genomeX[c].mod_nseed = new uint32_t[genomeX[c].seed_count];

			for (uint8_t s=0; s < genomeX[c].seed_count; s++)
			{ genomeX[c].mod_nseed[s] = int_dist(mt_rand); }

			for (uint8_t m=0; m < genomeX[c].mod_count; m++)
			{
				genomeX[c].mod_start[m] = byte_dist(mt_rand);
				genomeX[c].mod_impact[m] = byte_dist(mt_rand);
				genomeX[c].mod_inind[m] = byte_dist(mt_rand);
				genomeX[c].mod_index[m] = byte_dist(mt_rand);
				genomeX[c].mod_index[m] %= genomeX[c].seed_count;

				randBytes.ui8_1 = byte_dist(mt_rand);
				randBytes.ui8_2 = byte_dist(mt_rand);
				randBytes.ui8_3 = byte_dist(mt_rand);
				randBytes.ui8_4 = byte_dist(mt_rand);
				genomeX[c].mod_inout[m] = randBytes.data;

				randBytes.ui8_1 = byte_dist(mt_rand);
				randBytes.ui8_2 = byte_dist(mt_rand);
				randBytes.ui8_3 = byte_dist(mt_rand);
				randBytes.ui8_4 = byte_dist(mt_rand);
				genomeX[c].mod_ioind[m] = randBytes.data;
			}
		}

		for (uint8_t c=0; c < cSize; c++)
		{
			CloneChromo(genomeY[c], genomeX[c]);
			MutateChromo(genomeY[c], GLOBALS::mutate_step, GLOBALS::mutate_shift);
		}

		for (uint32_t h=0; h < hSize; h++)
		{ heritableArr[h] = int_dist(mt_rand); }
	}
	EvoAgent(cint** iArray, InputConfig iConfig, uint32_t oSize) :
		autoFree(false),
		itrCnt(0ui32),
		health(1.0),
		inputConfig(iConfig),
		outArrSize(oSize),
		modulesX(nullptr),
		modulesY(nullptr)
	{
		inputArrSet = new cint*[iConfig.inputCount];

		for (uint32_t i=0; i < iConfig.inputCount; ++i) {
			if (inputConfig.inputAlloc[i]) {
				inputArrSet[i] = nullptr;
			} else {
				inputArrSet[i] = iArray[i];
			}
		}
	}
	EvoAgent(Chromosome* GX, Chromosome* GY, cint** iArray, InputConfig iConfig, uint32_t cLim,  
	uint8_t cSize, uint32_t oSize, uint32_t wSize, uint32_t hSize, bool alloc = true) :
		autoFree(alloc),
		itrCnt(0ui32),
		health(1.0),
		genomeX(GX),
		genomeY(GY),
		inputConfig(iConfig),
	    chrCount(cSize),
		outArrSize(oSize),
		wrkArrSize(wSize),
		herArrSize(hSize),
		compLimit(cLim),
		modulesX(nullptr),
		modulesY(nullptr)
	{
		if (alloc) {

			inputArrSet = new cint*[iConfig.inputCount];
			outputArr = new cint[oSize];
			workingArr = new cint[wSize];
			heritableArr = new cint[hSize];

			for (uint32_t i=0; i < iConfig.inputCount; ++i) {
				if (inputConfig.inputAlloc[i]) {
					inputArrSet[i] = new cint[inputConfig.inputSizes[i]];
					fill_n(inputArrSet[i], inputConfig.inputSizes[i], 0);
				} else {
					inputArrSet[i] = iArray[i];
				}
			}
		}
	}
	~EvoAgent()
	{
		if (autoFree) {

			delete[] outputArr;
			delete[] workingArr;
			delete[] heritableArr;

			for (uint32_t i=0; i < inputConfig.inputCount; ++i) {
				if (inputConfig.inputAlloc[i]) {
					delete[] inputArrSet[i];
				}
			}

			delete[] inputArrSet;

			for (uint8_t c=0; c < chrCount; c++)
			{
				delete[] genomeX[c].mod_impact;
				delete[] genomeX[c].mod_inind;
				delete[] genomeX[c].mod_inout;
				delete[] genomeX[c].mod_ioind;
				delete[] genomeX[c].mod_index;
				delete[] genomeX[c].mod_start;
				delete[] genomeX[c].mod_nseed;

				delete[] genomeY[c].mod_impact;
				delete[] genomeY[c].mod_inind;
				delete[] genomeY[c].mod_inout;
				delete[] genomeY[c].mod_ioind;
				delete[] genomeY[c].mod_index;
				delete[] genomeY[c].mod_start;
				delete[] genomeY[c].mod_nseed;
			}

			if (modulesX != nullptr) {
				for (uint8_t c=0; c < chrCount; c++)
				{
					delete[] modulesX[c];
					delete[] modulesY[c];
				}
			}
		}
	}
	void FreeProto(const bool freeArrs)
	{
		if (freeArrs) {
			delete[] heritableArr;
			delete[] inputArrSet;
		}

		for (uint8_t c=0; c < chrCount; c++)
		{
			delete[] genomeX[c].mod_impact;
			delete[] genomeX[c].mod_inind;
			delete[] genomeX[c].mod_inout;
			delete[] genomeX[c].mod_ioind;
			delete[] genomeX[c].mod_index;
			delete[] genomeX[c].mod_start;
			delete[] genomeX[c].mod_nseed;

			delete[] genomeY[c].mod_impact;
			delete[] genomeY[c].mod_inind;
			delete[] genomeY[c].mod_inout;
			delete[] genomeY[c].mod_ioind;
			delete[] genomeY[c].mod_index;
			delete[] genomeY[c].mod_start;
			delete[] genomeY[c].mod_nseed;
		}
	}
	bool SaveGenome(const string fileDir)
	{
		uint8_t mc, sc;
		string filename = id + ".dna";
		FILE* pFile = fopen((fileDir+filename).c_str(), "wb");

		if (pFile == NULL) {
			cout << "Cannot write file: " << fileDir+filename << "\n";
			return false;
		}

		fwrite(&chrCount, sizeof(uint8_t), 1, pFile);
		fwrite(&compLimit, sizeof(uint32_t), 1, pFile);
		fwrite(&wrkArrSize, sizeof(uint32_t), 1, pFile);

		for (uint8_t c=0; c < chrCount; ++c)
		{
			mc = genomeX[c].mod_count;
			sc = genomeX[c].seed_count;

			fwrite(&mc, sizeof(uint8_t), 1, pFile);
			fwrite(&sc, sizeof(uint8_t), 1, pFile);

			fwrite(genomeX[c].mod_start, sizeof(uint8_t), mc, pFile);
			fwrite(genomeX[c].mod_impact, sizeof(uint8_t), mc, pFile);
			fwrite(genomeX[c].mod_index, sizeof(uint8_t), mc, pFile);
			fwrite(genomeX[c].mod_inind, sizeof(uint8_t), mc, pFile);
			fwrite(genomeX[c].mod_ioind, sizeof(uint32_t), mc, pFile);
			fwrite(genomeX[c].mod_inout, sizeof(uint32_t), mc, pFile);
			fwrite(genomeX[c].mod_nseed, sizeof(uint32_t), sc, pFile);

			mc = genomeY[c].mod_count;
			sc = genomeY[c].seed_count;

			fwrite(&mc, sizeof(uint8_t), 1, pFile);
			fwrite(&sc, sizeof(uint8_t), 1, pFile);

			fwrite(genomeY[c].mod_start, sizeof(uint8_t), mc, pFile);
			fwrite(genomeY[c].mod_impact, sizeof(uint8_t), mc, pFile);
			fwrite(genomeY[c].mod_index, sizeof(uint8_t), mc, pFile);
			fwrite(genomeY[c].mod_inind, sizeof(uint8_t), mc, pFile);
			fwrite(genomeY[c].mod_ioind, sizeof(uint32_t), mc, pFile);
			fwrite(genomeY[c].mod_inout, sizeof(uint32_t), mc, pFile);
			fwrite(genomeY[c].mod_nseed, sizeof(uint32_t), sc, pFile);
		}

		fclose(pFile);
		filename = id + ".her";

		pFile = fopen((fileDir+filename).c_str(), "wb");

		fwrite(&herArrSize, sizeof(uint32_t), 1, pFile);
		fwrite(heritableArr, sizeof(cint), herArrSize, pFile);

		fclose(pFile);
		return true;
	}
	bool LoadGenome(const string fileDir, const string agentID, const bool top)
	{
		uint8_t mc, sc;
		string filename;
		id = agentID;

		if (top) {
			filename = "1st\\" + id + ".dna";
		} else {
			filename = "2nd\\" + id + ".dna";
		}

		FILE* pFile = fopen((fileDir+filename).c_str(), "rb");

		if (pFile == NULL) {
			cout << "Cannot read file: " << fileDir+filename << "\n";
			return false;
		}

		fread(&chrCount, sizeof(uint8_t), 1, pFile);
		fread(&compLimit, sizeof(uint32_t), 1, pFile);
		fread(&wrkArrSize, sizeof(uint32_t), 1, pFile);

		genomeX = new Chromosome[chrCount];
		genomeY = new Chromosome[chrCount];

		for (uint8_t c=0; c < chrCount; ++c)
		{
			fread(&(genomeX[c].mod_count), sizeof(uint8_t), 1, pFile);
			fread(&(genomeX[c].seed_count), sizeof(uint8_t), 1, pFile);

			mc = genomeX[c].mod_count;
			sc = genomeX[c].seed_count;

			genomeX[c].mod_start = new uint8_t[mc];
			genomeX[c].mod_impact = new uint8_t[mc];
			genomeX[c].mod_index = new uint8_t[mc];
			genomeX[c].mod_inind = new uint8_t[mc];
			genomeX[c].mod_ioind = new uint32_t[mc];
			genomeX[c].mod_inout = new uint32_t[mc];
			genomeX[c].mod_nseed = new uint32_t[sc];

			fread(genomeX[c].mod_start, sizeof(uint8_t), mc, pFile);
			fread(genomeX[c].mod_impact, sizeof(uint8_t), mc, pFile);
			fread(genomeX[c].mod_index, sizeof(uint8_t), mc, pFile);
			fread(genomeX[c].mod_inind, sizeof(uint8_t), mc, pFile);
			fread(genomeX[c].mod_ioind, sizeof(uint32_t), mc, pFile);
			fread(genomeX[c].mod_inout, sizeof(uint32_t), mc, pFile);
			fread(genomeX[c].mod_nseed, sizeof(uint32_t), sc, pFile);

			fread(&(genomeY[c].mod_count), sizeof(uint8_t), 1, pFile);
			fread(&(genomeY[c].seed_count), sizeof(uint8_t), 1, pFile);

			mc = genomeY[c].mod_count;
			sc = genomeY[c].seed_count;

			genomeY[c].mod_start = new uint8_t[mc];
			genomeY[c].mod_impact = new uint8_t[mc];
			genomeY[c].mod_index = new uint8_t[mc];
			genomeY[c].mod_inind = new uint8_t[mc];
			genomeY[c].mod_ioind = new uint32_t[mc];
			genomeY[c].mod_inout = new uint32_t[mc];
			genomeY[c].mod_nseed = new uint32_t[sc];

			fread(genomeY[c].mod_start, sizeof(uint8_t), mc, pFile);
			fread(genomeY[c].mod_impact, sizeof(uint8_t), mc, pFile);
			fread(genomeY[c].mod_index, sizeof(uint8_t), mc, pFile);
			fread(genomeY[c].mod_inind, sizeof(uint8_t), mc, pFile);
			fread(genomeY[c].mod_ioind, sizeof(uint32_t), mc, pFile);
			fread(genomeY[c].mod_inout, sizeof(uint32_t), mc, pFile);
			fread(genomeY[c].mod_nseed, sizeof(uint32_t), sc, pFile);
		}

		fclose(pFile);

		if (top) {
			filename = "1st\\" + id + ".her";
		} else {
			filename = "2nd\\" + id + ".her";
		}

		pFile = fopen((fileDir+filename).c_str(), "rb");
		fread(&herArrSize, sizeof(uint32_t), 1, pFile);

		heritableArr = new cint[herArrSize];
		fread(heritableArr, sizeof(cint), herArrSize, pFile);

		fclose(pFile);
		return true;
	}
	void CopyAgent(EvoAgent& agent)
	{
		genomeX = agent.genomeX;
		genomeY = agent.genomeY;

		modulesX = agent.modulesX;
		modulesY = agent.modulesY;

		chrCount = agent.chrCount;
		compLimit = agent.compLimit;

		inputArrSet = agent.inputArrSet;
		outputArr = agent.outputArr;
		communeArr = agent.communeArr;
		workingArr = agent.workingArr;
		heritableArr = agent.heritableArr;

		inputConfig = agent.inputConfig;
		outArrSize = agent.outArrSize;
		wrkArrSize = agent.wrkArrSize;
		herArrSize = agent.herArrSize;
		comArrSize = agent.comArrSize;
	}
	void CloneChromo(Chromosome& dest, const Chromosome source)
	{
		dest.mod_count = source.mod_count;
		dest.seed_count = source.seed_count;

		dest.mod_start = new uint8_t[dest.mod_count];
		dest.mod_impact = new uint8_t[dest.mod_count];
		dest.mod_inout = new uint32_t[dest.mod_count];
		dest.mod_ioind = new uint32_t[dest.mod_count];
		dest.mod_inind = new uint8_t[dest.mod_count];
		dest.mod_index = new uint8_t[dest.mod_count];
		dest.mod_nseed = new uint32_t[dest.seed_count];

		for (uint8_t s=0; s < dest.seed_count; s++)
		{ dest.mod_nseed[s] = source.mod_nseed[s]; }

		for (uint8_t m=0; m < dest.mod_count; m++)
		{
			dest.mod_start[m] = source.mod_start[m];
			dest.mod_impact[m] = source.mod_impact[m];
			dest.mod_inout[m] = source.mod_inout[m];
			dest.mod_ioind[m] = source.mod_ioind[m];
			dest.mod_inind[m] = source.mod_inind[m];
			dest.mod_index[m] = source.mod_index[m];
		}
	}
	void MutateChromo(Chromosome& chromo, uint32_t step, uint8_t shift)
	{
		srand(time(NULL) + rand());
		uint32_t sInd = rand() % step;

		while (sInd < chromo.mod_count) {
			chromo.mod_start[sInd] += rand_range(-shift, shift);
			sInd += (rand() % (step + radResist)) + 1;
		}
		sInd -= chromo.mod_count;

		while (sInd < chromo.mod_count) {
			chromo.mod_impact[sInd] += rand_range(-shift, shift);
			sInd += (rand() % (step + radResist)) + 1;
		}
		sInd -= chromo.mod_count;

		while (sInd < chromo.mod_count) {
			chromo.mod_index[sInd] += rand_range(-shift, shift);
			chromo.mod_index[sInd] %= chromo.seed_count;
			sInd += (rand() % (step + radResist)) + 1;
		}
		sInd -= chromo.mod_count;

		while (sInd < chromo.mod_count) {
			chromo.mod_inind[sInd] += rand_range(-shift, shift);
			sInd += (rand() % (step + radResist)) + 1;
		}
		sInd -= chromo.mod_count;

		while (sInd < chromo.seed_count) {
			chromo.mod_nseed[sInd] += rand_range(-shift, shift);
			sInd += (rand() % (step + radResist)) + 1;
		}
		sInd -= chromo.seed_count;

		while (sInd < chromo.mod_count*4ui32) {
			ui32ui8 inout; uint32_t i = sInd * 0.25f;
			inout.data = chromo.mod_inout[i];
			while (i == uint32_t(sInd * 0.25f)) {
				inout.vars[sInd%4] += rand_range(-shift, shift);
				sInd += (rand() % (step + radResist)) + 1;
			}
			chromo.mod_inout[i] = inout.data;
		}
		sInd -= chromo.mod_count * 4ui32;

		while (sInd < chromo.mod_count*4ui32) {
			ui32ui8 inout; uint32_t i = sInd * 0.25f;
			inout.data = chromo.mod_ioind[i];
			while (i == uint32_t(sInd * 0.25f)) {
				inout.vars[sInd%4] += rand_range(-shift, shift);
				sInd += (rand() % (step + radResist)) + 1;
			}
			chromo.mod_ioind[i] = inout.data;
		}
	}
	void MutateHerArr(uint32_t step, uint8_t shift)
	{
		for (uint32_t h=0; h < herArrSize;) {
			heritableArr[h] += rand_range(-shift, shift);
			h += (rand() % (step + radResist)) + 1;
		}
	}
	uint32_t** ExpandSeeds(const Chromosome& chrome)
	{
		uint32_t** result = new uint32_t*[chrome.seed_count];

		mt19937 mt_rand = SeededRandomEngine();
		uniform_int_distribution<int> byte_dist(0, UINT8_MAX);

		for (uint8_t s=0; s < chrome.seed_count; s++)
		{
			mt_rand.seed(chrome.mod_nseed[s]);
			result[s] = new uint32_t[UINT8_MAX];
			ui32ui8 randBytes;

			for (uint8_t i=0; i < UINT8_MAX; i++)
			{
				randBytes.ui8_1 = byte_dist(mt_rand);
				randBytes.ui8_2 = byte_dist(mt_rand);
				randBytes.ui8_3 = byte_dist(mt_rand);
				randBytes.ui8_4 = byte_dist(mt_rand);
				result[s][i] = randBytes.data;
			}
		}

		return result;
	}
	cint* GetInputArray(const uint8_t t, uint8_t i, uint32_t& size)
	{
		switch (t % 4) {
			case 0:
				i %= inputConfig.inputCount;
				size = inputConfig.inputSizes[i];
				return inputArrSet[i];
			case 1:
				size = comArrSize;
				return communeArr;
			case 2:
				size = wrkArrSize;
				return workingArr;
			case 3:
				size = herArrSize;
				return heritableArr;
		}	
	}
	cint* GetOutputArray(const uint8_t target, uint32_t& size)
	{
		switch (target % 4) {
			case 0:
				size = outArrSize;
				return outputArr;
			case 1:
				size = comArrSize;
				return communeArr;
			case 2:
				size = wrkArrSize;
				return workingArr;
			case 3:
				size = herArrSize;
				return heritableArr;
		}
	}
	void CreateMods(Chromosome& chromo, Module* mods, uint32_t** nsArr)
	{
		ui32ui8 inout; ui32ui8 ioind;
		uint32_t iaSize0; uint32_t iaSize1;
		uint32_t oaSize0; uint32_t oaSize1;

		for (uint8_t m=0; m < chromo.mod_count; m++)
		{
			uint32_t* nsiArr = nsArr[chromo.mod_index[m]];
			mods[m].CreateNodes(m, chromo, nsiArr);

			ioind.data = chromo.mod_ioind[m];
			inout.data = chromo.mod_inout[m];

			cint* inpArr0 = GetInputArray(inout.ui8_1, chromo.mod_inind[m], iaSize0);
			cint* inpArr1 = GetInputArray(inout.ui8_2, chromo.mod_inind[m], iaSize1);

			cint* outArr0 = GetOutputArray(inout.ui8_3, oaSize0);
			cint* outArr1 = GetOutputArray(inout.ui8_4, oaSize1);

			mods[m].SetInputs(0, inpArr0, iaSize0, nsiArr[ioind.ui8_1]);
			mods[m].SetInputs(1, inpArr1, iaSize1, nsiArr[ioind.ui8_2]);

			mods[m].SetOutputs(0, outArr0, oaSize0, nsiArr[ioind.ui8_3]);
			mods[m].SetOutputs(1, outArr1, oaSize1, nsiArr[ioind.ui8_4]);

			mods[m].pFitness = &fitness;
		}
	}
	void BuildFromDNA(cint* comArray, uint32_t caSize, uint8_t radRes)
	{
		uint32_t** nsArrX = nullptr;
		uint32_t** nsArrY = nullptr;

		communeArr = comArray;
		comArrSize = caSize;

		modulesX = new Module*[chrCount];
		modulesY = new Module*[chrCount];

		radResist = radRes;
		autoFree = true;

        for (uint8_t c=0; c < chrCount; c++)
		{
			Chromosome& chromoX = genomeX[c];
			Chromosome& chromoY = genomeY[c];

			nsArrX = ExpandSeeds(chromoX);
			nsArrY = ExpandSeeds(chromoY);

			modulesX[c] = new Module[chromoX.mod_count];
			modulesY[c] = new Module[chromoY.mod_count];

			CreateMods(chromoX, modulesX[c], nsArrX);
			CreateMods(chromoY, modulesY[c], nsArrY);

			for (uint8_t s=0; s < chromoX.seed_count; s++)
			{ delete[] nsArrX[s]; delete[] nsArrY[s]; }
        }
	}
	Chromosome* CreateGamete()
	{
		srand(time(NULL) + rand());

		Chromosome* result = new Chromosome[chrCount];
		Chromosome* targChr;

        for (uint8_t c=0; c < chrCount; c++)
		{
			if (rand() % 2) {
				CloneChromo(result[c], genomeX[c]);
				targChr = genomeY;
			} else {
				CloneChromo(result[c], genomeY[c]);
				targChr = genomeX;
			}

			if (rand() % 2) { continue; }

			uint8_t count = 0;
			uint8_t start, dist;

			if (rand() % 2) {
				start = rand() % result[c].mod_count;
				dist = rand() % result[c].mod_count;
				for (uint8_t m=start; count++ < dist; m++) {
					m %= result[c].mod_count;
					result[c].mod_impact[m] = targChr[c].mod_impact[m];
				}
			}

			if (rand() % 2) {
				start = rand() % result[c].mod_count;
				dist = rand() % result[c].mod_count;
				for (uint8_t m=start; count++ < dist; m++) {
					m %= result[c].mod_count;
					result[c].mod_inout[m] = targChr[c].mod_inout[m];
				}
			}

			if (rand() % 2) {
				start = rand() % result[c].mod_count;
				dist = rand() % result[c].mod_count;
				for (uint8_t m=start; count++ < dist; m++) {
					m %= result[c].mod_count;
					result[c].mod_ioind[m] = targChr[c].mod_ioind[m];
				}
			}

			if (rand() % 2) {
				start = rand() % result[c].mod_count;
				dist = rand() % result[c].mod_count;
				for (uint8_t m=start; count++ < dist; m++) {
					m %= result[c].mod_count;
					result[c].mod_inind[m] = targChr[c].mod_inind[m];
				}
			}

			if (rand() % 2) {
				start = rand() % result[c].mod_count;
				dist = rand() % result[c].mod_count;
				for (uint8_t m=start; count++ < dist; m++) {
					m %= result[c].mod_count;
					result[c].mod_index[m] = targChr[c].mod_index[m];
				}
			}

			if (rand() % 2) {
				start = rand() % result[c].mod_count;
				dist = rand() % result[c].mod_count;
				for (uint8_t m=start; count++ < dist; m++) {
					m %= result[c].mod_count;
					result[c].mod_start[m] = targChr[c].mod_start[m];
				}
			}

			if (rand() % 2) {
				start = rand() % result[c].seed_count;
				dist = rand() % result[c].seed_count;
				for (uint8_t s=start; count++ < dist; s++) {
					s %= result[c].seed_count;
					result[c].mod_nseed[s] = targChr[c].mod_nseed[s];
				}
			}

			MutateChromo(result[c], GLOBALS::mutate_step, GLOBALS::mutate_shift);
		}

		return result;
	}
	void InheritArray(cint* hArray, uint32_t hSize)
	{
		for (uint32_t i=0; i < hSize; i++) {
			heritableArr[i] = hArray[i];
		}
		
		MutateHerArr(GLOBALS::mutate_step, GLOBALS::mutate_shift);
	}
	EvoAgent CreateZygote(EvoAgent& partner)
	{
		Chromosome* new_GX = CreateGamete();
		Chromosome* new_GY = partner.CreateGamete();

		EvoAgent result = EvoAgent(new_GX, new_GY, inputArrSet, inputConfig,
						  compLimit, chrCount, outArrSize, wrkArrSize, herArrSize);

		result.InheritArray(heritableArr, herArrSize);
		result.autoFree = false;

		return result;
	}
	void ComputeOutput()
	{
		++itrCnt;
		uint32_t compCost = 0;
		srand(time(NULL)+rand());

		for (uint8_t c=0; c < chrCount; c++) {
            for (uint32_t m=0; m < genomeX[c].mod_count; m++)
			{
				if (genomeX[c].mod_impact[m] > genomeY[c].mod_impact[m]) {

					modulesX[c][m].Compute();
					compCost += modulesX[c][m].compCost;
					if (compCost >= compLimit) { return; }

				} else if (genomeX[c].mod_impact[m] < genomeY[c].mod_impact[m]) {

					modulesY[c][m].Compute();
					compCost += modulesY[c][m].compCost;
					if (compCost >= compLimit) { return; }

				} else {

					modulesX[c][m].Compute();
					compCost += modulesX[c][m].compCost;
					if (compCost >= compLimit) { return; }
				
					modulesY[c][m].Compute();
					compCost += modulesY[c][m].compCost;
					if (compCost >= compLimit) { return; }
				}
            }
		}
	}
};