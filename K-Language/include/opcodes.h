#pragma once

#include "common.h"

namespace k
{
	enum class Opcode : UInt8
	{
		NOP,			//(0): [0] -> [0]

		POP,			//(0): [1] -> [0]
		POP2,			//(0): [2] -> [0]

		SWAP,			//(0): [2] -> [2]

		DUP,			//(0): [1] -> [2]
		DUP_X1,			//(0): [2] -> [3]
		DUP_X2,			//(0): [3] -> [4]

		LOADC_U,		//(0): [0] -> [1]
		LOADC_B,		//(1): [0] -> [1]
		LOADC_I,		//(1): [0] -> [1]
		LOADC_R,		//(1): [0] -> [1]
		LOADC,			//(1): [0] -> [1]
		LOADCW,			//(2): [0] -> [1]
		LOADCL,			//(4): [0] -> [1]

		LOAD_S,			//(0): [0] -> [1]
		LOAD_0,			//(0): [0] -> [1]
		LOAD_1,			//(0): [0] -> [1]
		LOAD_2,			//(0): [0] -> [1]
		LOAD_3,			//(0): [0] -> [1]
		LOAD,			//(1): [0] -> [1]

		NEW_ARRAY,		//(0): [0] -> [1]
		NEW_ARRAY_C,	//(1): [0] -> [1]
		NEW_ARRAY_L,	//(0): [1] -> [1]

		STORE_S,		//(0): [1] -> [0]
		STORE_0,		//(0): [1] -> [0]
		STORE_1,		//(0): [1] -> [0]
		STORE_2,		//(0): [1] -> [0]
		STORE_3,		//(0): [1] -> [0]
		STORE,			//(1): [1] -> [0]
	};
}

namespace k::opcode
{

}
