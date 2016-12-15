#pragma once
#include <cstdint>

// custom integer type
typedef int64_t cint;

#define CINT_MAX INT64_MAX;
#define CINT_DEC 0.00000001
#define CINT_BSM 63i64

// global environment vars
namespace GLOBALS {
	extern uint32_t mutate_step;
	extern uint8_t mutate_shift;
	extern uint8_t mutate_shirr;
	extern uint8_t chromo_shift;
	extern uint8_t max_chromos;
	extern uint8_t max_mnodes;
	extern uint8_t max_cmods;
	extern uint8_t max_cseeds;
	extern double health_decay;
};