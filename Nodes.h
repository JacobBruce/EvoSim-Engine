#pragma once
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include "Resource.h"
#include "Modules.h"

using namespace std;

// ------------------------------------------- //
// --------  INTERMEDIATE NODE TYPES  -------- //
// ------------------------------------------- //

class NodeType1 : public Node {
protected:
	bool target;
public:
	NodeType1(uint8_t c) : Node(c, 0) {}
	void Set(bool t) {
		target = t;
	}
};

class NodeType2 : public Node {
protected:
	bool target;
	uint8_t shift;
public:
	NodeType2(uint8_t c) : Node(c, 1) {}
	void Set(bool t, uint8_t s) {
		target = t;
		shift = s;
	}
};

class NodeType3 : public Node {
protected:
	bool target;
	int8_t shift;
public:
	NodeType3(uint8_t c) : Node(c, 2) {}
	void Set(bool t, int8_t s) {
		target = t;
		shift = s;
	}
};

class NodeType4 : public Node {
protected:
	uint8_t index;
public:
	NodeType4(uint8_t c) : Node(c, 3) {}
	void Set(uint8_t s) {
		index = s;
	}
};

class NodeType5 : public Node {
protected:
	bool targ1, targ2;
public:
	NodeType5(uint8_t c) : Node(c, 4) {}
	void Set(bool t1, bool t2) {
		targ1 = t1;
		targ2 = t2;
	}
};

class NodeType6 : public Node {
public:
	NodeType6(uint8_t c) : Node(c, 5) {}
};

// ------------------------------------------- //
// ---------  SINGLE OPERATOR NODES  --------- //
// ------------------------------------------- //

class NegNode : public NodeType1 {
public:
	NegNode() : NodeType1(1) {}
	void Compute(Module& mod) {
		mod.regArr[target] = -(mod.regArr[target]);
		mod.compCost += cost;
	}
};

class LotNode : public NodeType1 {
public:
	LotNode() : NodeType1(1) {}
	void Compute(Module& mod) {
		mod.regArr[target] = !(mod.regArr[target]);
		mod.compCost += cost;
	}
};

class NotNode : public NodeType1 {
public:
	NotNode() : NodeType1(1) {}
	void Compute(Module& mod) {
		mod.regArr[target] = ~(mod.regArr[target]);
		mod.compCost += cost;
	}
};

class IncNode : public NodeType1 {
public:
	IncNode() : NodeType1(1) {}
	void Compute(Module& mod) {
		++(mod.regArr[target]);
		mod.compCost += cost;
	}
};

class DecNode : public NodeType1 {
public:
	DecNode() : NodeType1(1) {}
	void Compute(Module& mod) {
		--(mod.regArr[target]);
		mod.compCost += cost;
	}
};

class RanNode : public NodeType1 {
public:
	RanNode() : NodeType1(2) {}
	void Compute(Module& mod) {
		mod.regArr[target] = rand();
		mod.compCost += cost;
	}
};

class SgnNode : public NodeType1 {
public:
	SgnNode() : NodeType1(2) {}
	void Compute(Module& mod) {
		mod.regArr[target] = mod.regArr[!target] >> CINT_BSM;
		mod.compCost += cost;
	}
};

class FitNode : public NodeType1 {
public:
	FitNode() : NodeType1(2) {}
	void Compute(Module& mod) {
		mod.regArr[target] = *(mod.pFitness) / CINT_DEC;
		mod.compCost += cost;
	}
};

// ------------------------------------------- //
// ----------  MATH FUNCTION NODES  ---------- //
// ------------------------------------------- //

class SinNode : public NodeType1 {
public:
	SinNode() : NodeType1(3) {}
	void Compute(Module& mod) {
		mod.regArr[target] = sin(mod.regArr[!target] * CINT_DEC) * CINT_MAX;
		mod.compCost += cost;
	}
};

class CosNode : public NodeType1 {
public:
	CosNode() : NodeType1(3) {}
	void Compute(Module& mod) {
		mod.regArr[target] = cos(mod.regArr[!target] * CINT_DEC) * CINT_MAX;
		mod.compCost += cost;
	}
};

class SigNode : public NodeType1 {
public:
	SigNode() : NodeType1(3) {}
	void Compute(Module& mod) {
		mod.regArr[target] = tanh(mod.regArr[!target] * CINT_DEC) * CINT_MAX;
		mod.compCost += cost;
	}
};

class SqrNode : public NodeType1 {
public:
	SqrNode() : NodeType1(2) {}
	void Compute(Module& mod) {
		mod.regArr[target] = pow(mod.regArr[!target], 2);
		mod.compCost += cost;
	}
};

class AbsNode : public NodeType1 {
public:
	AbsNode() : NodeType1(2) {}
	void Compute(Module& mod) {
		mod.regArr[target] = abs(mod.regArr[!target]);
		mod.compCost += cost;
	}
};

class PowNode : public NodeType1 {
public:
	PowNode() : NodeType1(2) {}
	void Compute(Module& mod) {
		mod.regArr[target] = pow(mod.regArr[0], mod.regArr[1]);
		mod.compCost += cost;
	}
};

class MinNode : public NodeType1 {
public:
	MinNode() : NodeType1(3) {}
	void Compute(Module& mod) {
		mod.regArr[target] = min(mod.regArr[0], mod.regArr[1]);
		mod.compCost += cost;
	}
};

class MaxNode : public NodeType1 {
public:
	MaxNode() : NodeType1(3) {}
	void Compute(Module& mod) {
		mod.regArr[target] = max(mod.regArr[0], mod.regArr[1]);
		mod.compCost += cost;
	}
};

// ------------------------------------------- //
// ---------  DOUBLE OPERATOR NODES  --------- //
// ------------------------------------------- //

class SumNode : public NodeType1 {
public:
	SumNode() : NodeType1(1) {}
	void Compute(Module& mod) {
		mod.regArr[target] = mod.regArr[0] + mod.regArr[1];
		mod.compCost += cost;
	}
};

class SubNode : public NodeType1 {
public:
	SubNode() : NodeType1(1) {}
	void Compute(Module& mod) {
		mod.regArr[target] = mod.regArr[0] - mod.regArr[1];
		mod.compCost += cost;
	}
};

class MulNode : public NodeType1 {
public:
	MulNode() : NodeType1(1) {}
	void Compute(Module& mod) {
		mod.regArr[target] = mod.regArr[0] * mod.regArr[1];
		mod.compCost += cost;
	}
};

class AndNode : public NodeType1 {
public:
	AndNode() : NodeType1(1) {}
	void Compute(Module& mod) {
		mod.regArr[target] = mod.regArr[0] & mod.regArr[1];
		mod.compCost += cost;
	}
};

class BorNode : public NodeType1 {
public:
	BorNode() : NodeType1(1) {}
	void Compute(Module& mod) {
		mod.regArr[target] = mod.regArr[0] | mod.regArr[1];
		mod.compCost += cost;
	}
};

class XorNode : public NodeType1 {
public:
	XorNode() : NodeType1(1) {}
	void Compute(Module& mod) {
		mod.regArr[target] = mod.regArr[0] ^ mod.regArr[1];
		mod.compCost += cost;
	}
};

class LftNode : public NodeType1 {
public:
	LftNode() : NodeType1(1) {}
	void Compute(Module& mod) {
		mod.regArr[target] = mod.regArr[0] << mod.regArr[1];
		mod.compCost += cost;
	}
};

class RgtNode : public NodeType1 {
public:
	RgtNode() : NodeType1(1) {}
	void Compute(Module& mod) {
		mod.regArr[target] = mod.regArr[0] >> mod.regArr[1];
		mod.compCost += cost;
	}
};

// ------------------------------------------- //
// -------------  COMPOUND NODES  ------------ //
// ------------------------------------------- //

class AddNode : public NodeType2 {
public:
	AddNode() : NodeType2(1) {}
	void Compute(Module& mod) {
		mod.regArr[target] += shift;
		mod.compCost += cost;
	}
};

class DwnNode : public NodeType2 {
public:
	DwnNode() : NodeType2(1) {}
	void Compute(Module& mod) {
		mod.regArr[target] -= shift;
		mod.compCost += cost;
	}
};

class TmsNode : public NodeType3 {
public:
	TmsNode() : NodeType3(1) {}
	void Compute(Module& mod) {
		mod.regArr[target] *= shift;
		mod.compCost += cost;
	}
};

class LshNode : public NodeType3 {
public:
	LshNode() : NodeType3(1) {}
	void Compute(Module& mod) {
		mod.regArr[target] <<= shift;
		mod.compCost += cost;
	}
};

class RshNode : public NodeType3 {
public:
	RshNode() : NodeType3(1) {}
	void Compute(Module& mod) {
		mod.regArr[target] >>= shift;
		mod.compCost += cost;
	}
};

class CanNode : public NodeType3 {
public:
	CanNode() : NodeType3(1) {}
	void Compute(Module& mod) {
		mod.regArr[target] &= shift;
		mod.compCost += cost;
	}
};

class CorNode : public NodeType3 {
public:
	CorNode() : NodeType3(1) {}
	void Compute(Module& mod) {
		mod.regArr[target] |= shift;
		mod.compCost += cost;
	}
};

class CexNode : public NodeType3 {
public:
	CexNode() : NodeType3(1) {}
	void Compute(Module& mod) {
		mod.regArr[target] ^= shift;
		mod.compCost += cost;
	}
};

// ------------------------------------------- //
// -----------  CONDITIONAL NODES  ----------- //
// ------------------------------------------- //

class EqtNode : public NodeType4 {
public:
	EqtNode() : NodeType4(3) {}
	void Compute(Module& mod) {
		if (mod.regArr[0] == mod.regArr[1]) {
			mod.nodeIndex = index;
		}
		mod.compCost += cost;
	}
};

class UntNode : public NodeType4 {
public:
	UntNode() : NodeType4(3) {}
	void Compute(Module& mod) {
		if (mod.regArr[0] != mod.regArr[1]) {
			mod.nodeIndex = index;
		}
		mod.compCost += cost;
	}
};

class GrtNode : public NodeType4 {
public:
	GrtNode() : NodeType4(3) {}
	void Compute(Module& mod) {
		if (mod.regArr[0] > mod.regArr[1]) {
			mod.nodeIndex = index;
		}
		mod.compCost += cost;
	}
};

class LstNode : public NodeType4 {
public:
	LstNode() : NodeType4(3) {}
	void Compute(Module& mod) {
		if (mod.regArr[0] < mod.regArr[1]) {
			mod.nodeIndex = index;
		}
		mod.compCost += cost;
	}
};

class GreNode : public NodeType4 {
public:
	GreNode() : NodeType4(3) {}
	void Compute(Module& mod) {
		if (mod.regArr[0] >= mod.regArr[1]) {
			mod.nodeIndex = index;
		}
		mod.compCost += cost;
	}
};

class LseNode : public NodeType4 {
public:
	LseNode() : NodeType4(3) {}
	void Compute(Module& mod) {
		if (mod.regArr[0] <= mod.regArr[1]) {
			mod.nodeIndex = index;
		}
		mod.compCost += cost;
	}
};

class LorNode : public NodeType4 {
public:
	LorNode() : NodeType4(3) {}
	void Compute(Module& mod) {
		if (mod.regArr[0] || mod.regArr[1]) {
			mod.nodeIndex = index;
		}
		mod.compCost += cost;
	}
};

class LanNode : public NodeType4 {
public:
	LanNode() : NodeType4(3) {}
	void Compute(Module& mod) {
		if (mod.regArr[0] && mod.regArr[1]) {
			mod.nodeIndex = index;
		}
		mod.compCost += cost;
	}
};

// ------------------------------------------- //
// ----------  CONTROL FLOW NODES  ----------- //
// ------------------------------------------- //

class GotoNode : public NodeType4 {
public:
	GotoNode() : NodeType4(1) {}
	void Compute(Module& mod) {
		mod.nodeIndex = index;
		mod.compCost += cost;
	}
};

class ExitNode : public NodeType6 {
public:
	ExitNode() : NodeType6(0) {}
	void Compute(Module& mod) {
		mod.nodeIndex = UINT8_MAX-1;
		mod.compCost += cost;
	}
};

class FlipNode : public NodeType1 {
public:
	FlipNode() : NodeType1(2) {}
	void Compute(Module& mod) {
		cint temp = mod.regArr[0];
		mod.regArr[0] = mod.regArr[1];
		mod.regArr[1] = temp;
		mod.compCost += cost;
	}
};

class CopyNode : public NodeType1 {
public:
	CopyNode() : NodeType1(2) {}
	void Compute(Module& mod) {
		cint temp = mod.regArr[target];
		mod.regArr[target] = mod.regArr[!target];
		mod.regArr[!target] = temp;
		mod.compCost += cost;
	}
};

class LoadNode : public NodeType5 {
public:
	LoadNode() : NodeType5(1) {}
	void Compute(Module& mod) {
		mod.regArr[targ1] = *(mod.iipArr[targ2]);
		mod.compCost += cost;
	}
};

class SaveNode : public NodeType5 {
public:
	SaveNode() : NodeType5(1) {}
	void Compute(Module& mod) {
		*(mod.oipArr[targ1]) = mod.regArr[targ2];
		mod.compCost += cost;
	}
};

class IncpNode : public NodeType5 {
public:
	IncpNode() : NodeType5(3) {}
	void Compute(Module& mod) {
		switch (targ1) {
		case false:
			if (mod.iipArr[targ2] != mod.iipLast[targ2])
			{ ++mod.iipArr[targ2]; } break;
		case true:
			if (mod.oipArr[targ2] != mod.oipLast[targ2])
			{ ++mod.oipArr[targ2]; } break;
		}
		mod.compCost += cost;
	}
};

class DecpNode : public NodeType5 {
public:
	DecpNode() : NodeType5(3) {}
	void Compute(Module& mod) {
		switch (targ1) {
		case false:
			if (mod.iipArr[targ2] != mod.iipFirst[targ2])
			{ --mod.iipArr[targ2]; } break;
		case true:
			if (mod.oipArr[targ2] != mod.oipFirst[targ2])
			{ --mod.oipArr[targ2]; } break;
		}
		mod.compCost += cost;
	}
};

//------ NODE ARRAY FUNCTIONS ------//

inline Node* AllocNodes(uint8_t type, uint8_t size)
{
	if (size == 0ui8) { return nullptr; }

	switch (type) {

		case 0: return new NegNode[size];
		case 1: return new LotNode[size];
		case 2: return new NotNode[size];
		case 3: return new IncNode[size];
		case 4: return new DecNode[size];
		case 5: return new RanNode[size];
		case 6: return new FitNode[size];
		case 7: return new SgnNode[size];

		case 8: return new SinNode[size];
		case 9: return new CosNode[size];
		case 10: return new SigNode[size];
		case 11: return new SqrNode[size];
		case 12: return new PowNode[size];
		case 13: return new AbsNode[size];
		case 14: return new MinNode[size];
		case 15: return new MaxNode[size];

		case 16: return new SumNode[size];
		case 17: return new SubNode[size];
		case 18: return new MulNode[size];
		case 19: return new AndNode[size];
		case 20: return new BorNode[size];
		case 21: return new XorNode[size];
		case 22: return new LftNode[size];
		case 23: return new RgtNode[size];

		case 24: return new AddNode[size];
		case 25: return new DwnNode[size];
		case 26: return new TmsNode[size];
		case 27: return new LshNode[size];
		case 28: return new RshNode[size];
		case 29: return new CanNode[size];
		case 30: return new CorNode[size];
		case 31: return new CexNode[size];

		case 32: return new EqtNode[size];
		case 33: return new UntNode[size];
		case 34: return new GrtNode[size];
		case 35: return new LstNode[size];
		case 36: return new GreNode[size];
		case 37: return new LseNode[size];
		case 38: return new LorNode[size];
		case 39: return new LanNode[size];

		case 40: return new GotoNode[size];
		case 41: return new ExitNode[size];
		case 42: return new FlipNode[size];
		case 43: return new CopyNode[size];
		case 44: return new LoadNode[size];
		case 45: return new SaveNode[size];
		case 46: return new IncpNode[size];
		case 47: return new DecpNode[size];

		default: return nullptr;
	}
}

inline void SetNode(Node* nodes, uint8_t ni, bool targ1, bool targ2, uint8_t ui8, int8_t si8)
{
	switch (nodes->type) {
		case 0: ((NodeType1*)nodes)[ni].Set(targ1); break;
		case 1: ((NodeType2*)nodes)[ni].Set(targ1, ui8); break;
		case 2: ((NodeType3*)nodes)[ni].Set(targ1, si8); break;
		case 3: ((NodeType4*)nodes)[ni].Set(ui8); break;
		case 4: ((NodeType5*)nodes)[ni].Set(targ1, targ2); break;
		case 5: break;
	}
}

inline Node& GetNode(Node* nodes, const uint8_t index)
{
	switch (nodes->type) {
		case 0: return ((NodeType1*)nodes)[index]; break;
		case 1: return ((NodeType2*)nodes)[index]; break;
		case 2: return ((NodeType3*)nodes)[index]; break;
		case 3: return ((NodeType4*)nodes)[index]; break;
		case 4: return ((NodeType5*)nodes)[index]; break;
		case 5: return ((NodeType6*)nodes)[index]; break;
	}
}

inline void FreeNodes(Node* nodes)
{
	switch (nodes->type) {
		case 0: delete[] ((NodeType1*)nodes); break;
		case 1: delete[] ((NodeType2*)nodes); break;
		case 2: delete[] ((NodeType3*)nodes); break;
		case 3: delete[] ((NodeType4*)nodes); break;
		case 4: delete[] ((NodeType5*)nodes); break;
		case 5: delete[] ((NodeType6*)nodes); break;
	}
}