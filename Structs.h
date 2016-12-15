#pragma once
#include <cstdint>

/// ----- INPUT ARRAY CONFIGURATION ----- ///

struct InputConfig {
	uint8_t inputCount;
	uint32_t* inputSizes;
	bool* inputAlloc;
};

/// ----- BREAK UI32 INTO 4 UI8's ----- ///

struct ui32ui8 {
	union {
		uint32_t data;
		uint8_t vars[4];
		struct {
			uint8_t ui8_1;
			uint8_t ui8_2;
			uint8_t ui8_3;
			uint8_t ui8_4;
		};
	};
};

/// ----- CHROMOSOME DNA PACKAGE ----- ///

struct Chromosome {
	uint32_t* mod_nseed;
	uint32_t* mod_inout;
	uint32_t* mod_ioind;
	uint8_t* mod_inind;
	uint8_t* mod_impact;
	uint8_t* mod_index;
	uint8_t* mod_start;
	uint8_t mod_count;
	uint8_t seed_count;
};

/// ----- LINEAR NODE INDEXER ----- ///

struct NodeLink {
	uint8_t type;
	uint8_t index;
};