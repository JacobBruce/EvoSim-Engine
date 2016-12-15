#pragma once
#include "Nodes.h"

using namespace std;

Module::Module() :
	ntypeCnt(),
	regArr(),
	autoIO(),
	nodeIndex(0),
	nodeCount(0)
{
	iipArr[0] = nullptr;
	iipArr[1] = nullptr;
	oipArr[0] = nullptr;
	oipArr[1] = nullptr;
}

Module::~Module()
{
	for (uint8_t t=0; t < 48; t++)
	{
		if (nodes[t] != nullptr)
		{ FreeNodes(nodes[t]); }
	}
	delete[] ntLinks;
}

void Module::CreateNodes(const uint8_t m, Chromosome& chromo, uint32_t* nodeDNA)
{
	ui32ui8 intData;
	uint8_t nodeType;
	uint8_t ndIndex;

	intData.data = nodeDNA[0];
	compLimit = max(1ui8, intData.ui8_1);

	autoIO[0] = intData.ui8_2 % 2;
	autoIO[1] = intData.ui8_3 % 2;

	nodeCount = (intData.ui8_4 % GLOBALS::max_mnodes) + 1ui8;
	ntLinks = new NodeLink[nodeCount];

	ndIndex = chromo.mod_start[m];

	for (uint8_t n=0; n < nodeCount; ++n)
	{
		intData.data = nodeDNA[ndIndex++];
		nodeType = intData.ui8_1 % 48ui8;
		ntLinks[n].type = nodeType;
		ntLinks[n].index = ntypeCnt[nodeType]++;
	}

	for (uint8_t t=0; t < 48; t++)
	{ nodes[t] = AllocNodes(t, ntypeCnt[t]); }

	ndIndex = chromo.mod_start[m];

	for (uint8_t n=0; n < nodeCount; ++n)
	{
		intData.data = nodeDNA[ndIndex++];

		SetNode(nodes[ntLinks[n].type], ntLinks[n].index, intData.ui8_2%2,
				intData.ui8_2<128, intData.ui8_3, intData.ui8_4);
	}
}

void Module::SetInputs(bool target, cint* inArr, uint32_t arrSize, uint32_t index)
{
	iipFirst[target] = &(inArr[0]);
	iipLast[target] = &(inArr[arrSize-1]);
	iipArr[target] = &(inArr[index%arrSize]);
}

void Module::SetOutputs(bool target, cint* outArr, uint32_t arrSize, uint32_t index)
{
	oipFirst[target] = &(outArr[0]);
	oipLast[target] = &(outArr[arrSize-1]);
	oipArr[target] = &(outArr[index%arrSize]);
}

void Module::LoadReg()
{
	regArr[0] = *(iipArr[0]);
	regArr[1] = *(iipArr[1]);
}

void Module::SaveReg()
{
    *(oipArr[0]) = regArr[0];
    *(oipArr[1]) = regArr[1];
}

void Module::Compute()
{
	compCost = 0;
	if (autoIO[0]) { LoadReg(); }
	for (nodeIndex=0; nodeIndex < nodeCount; nodeIndex++) {
		NodeLink& nLink = ntLinks[nodeIndex];
		GetNode(nodes[nLink.type], nLink.index).Compute(*this);
		if (compCost >= compLimit) { return; }
	}
	if (autoIO[1]) { SaveReg(); }
}