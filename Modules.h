#pragma once
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include "Resource.h"
#include "Structs.h"

/// ----- ABSTRACT NODE CLASS ----- ///

class Node {
	friend class Module;
public:
	const uint8_t cost;
	const uint8_t type;
protected:
	Node(uint8_t c, uint8_t t) : cost(c), type(t) {}
	virtual void Compute(Module& mod)=0;
};

/// ----- AGENT MODULE CLASS ----- ///

class Module {
	friend class NegNode; friend class RanNode;
	friend class IncNode; friend class DecNode;
	friend class FitNode; friend class SgnNode;
	friend class LotNode; friend class NotNode;

	friend class SinNode; friend class SigNode;
	friend class CosNode; friend class PowNode;
	friend class SqrNode; friend class AbsNode;
	friend class MinNode; friend class MaxNode;

	friend class SumNode; friend class SubNode;
	friend class MulNode; friend class LftNode;
	friend class AndNode; friend class RgtNode;
	friend class BorNode; friend class XorNode;

	friend class AddNode; friend class DwnNode;
	friend class TmsNode; friend class LshNode;
	friend class RshNode; friend class CanNode;
	friend class CorNode; friend class CexNode;

	friend class EqtNode; friend class UntNode;
	friend class GrtNode; friend class LstNode;
	friend class GreNode; friend class LseNode;
	friend class LorNode; friend class LanNode;

	friend class GotoNode; friend class ExitNode;
	friend class FlipNode; friend class CopyNode;
	friend class LoadNode; friend class SaveNode;
	friend class IncpNode; friend class DecpNode;
protected:
	uint32_t nodeCount;
	uint8_t nodeIndex;
	uint8_t ntypeCnt[48];
	Node* nodes[48];
	NodeLink* ntLinks;
	cint* iipArr[2];
	cint* oipArr[2];
	cint* iipLast[2];
	cint* oipLast[2];
	cint* iipFirst[2];
	cint* oipFirst[2];
	cint regArr[2];
	bool autoIO[2];
public:
	uint32_t compCost;
	uint8_t compLimit;
	double* pFitness;
public:
	Module();
	~Module();
	void CreateNodes(const uint8_t m, Chromosome& chromo, uint32_t* nodeDNA);
	void SetInputs(bool target, cint* inArr, uint32_t arrSize, uint32_t index);
	void SetOutputs(bool target, cint* outArr, uint32_t arrSize, uint32_t index);
	void LoadReg();
	void SaveReg();
	void Compute();
};