#include "runtime.h"

using k::instruction::InstructionValue;

#define get_arg(_Type, _ArgIdx) instruction::arg::get<instruction::arg::_Type>(insts + (instOffset + (_ArgIdx)))
#define get_ubyte(_ArgIdx) get_arg(ubyte, _ArgIdx)
#define get_sbyte(_ArgIdx) get_arg(sbyte, _ArgIdx)
#define get_uword(_ArgIdx) get_arg(uword, _ArgIdx)
#define get_sword(_ArgIdx) get_arg(sword, _ArgIdx)
#define get_ulong(_ArgIdx) get_arg(ulong, _ArgIdx)
#define get_slong(_ArgIdx) get_arg(slong, _ArgIdx)
#define get_uquad(_ArgIdx) get_arg(uquad, _ArgIdx)
#define get_squad(_ArgIdx) get_arg(squad, _ArgIdx)

#define opcode_case(_Opcode) case Opcode::_Opcode: {
#define opcode_end_and_jump(_Bytes, _Tag) instOffset += _Bytes; } goto _Tag
#define opcode_abort_and_jump(_Bytes, _Tag) instOffset += _Bytes; goto _Tag
#define opcode_end(_Bytes) opcode_end_and_jump(_Bytes, main_loop)
#define opcode_abort_error(_Bytes) opcode_abort_and_jump(_Bytes, error_zone)

#define check_errors(_Bytes) if(state._error.state) opcode_abort_error(_Bytes)

namespace k::runtime
{
	data::Value execute(RuntimeState& state, Callable& input_callable, const data::Value* input_self, const data::Value* args, Size argsCount)
	{
		const InstructionValue* insts;
		Offset instOffset;
		Callable* callable;
		data::Value* vars;
		data::Value* self;
		data::Value* temps;
		Offset tempsTop;

		state._calls.pushNative();
		callable = &input_callable;
		state._values.push(0, argsCount, callable->stackCount(), &vars);
		self = vars + callable->varsCount();
		temps = self + 1;
		tempsTop = 0;
		instOffset = 0;
		insts = callable->instructionData();
		for (Offset i = 0; i < argsCount; ++i)
			vars[i] = args[i];
		if (input_self)
			*self = *input_self;

	main_loop:
		switch (reinterpret_cast<const Opcode*>(insts)[instOffset])
		{
			opcode_case(NOP)
			opcode_end(1);


			opcode_case(POP)
				--tempsTop;
			opcode_end(1);

			opcode_case(POP2)
				tempsTop -= 2;
			opcode_end(1);


			opcode_case(SWAP)
				data::Value::swap(temps[tempsTop - 1], temps[tempsTop - 2]);
			opcode_end(1);


			opcode_case(DUP)
				temps[tempsTop] = temps[tempsTop - 1];
				++tempsTop;
			opcode_end(1);

			opcode_case(DUP_X1)
				temps[tempsTop] = temps[tempsTop - 1];
				temps[tempsTop - 1] = temps[tempsTop - 2];
				temps[tempsTop - 2] = temps[tempsTop];
				++tempsTop;
			opcode_end(1);

			opcode_case(DUP_X2)
				temps[tempsTop] = temps[tempsTop - 1];
				temps[tempsTop - 1] = temps[tempsTop - 2];
				temps[tempsTop - 2] = temps[tempsTop - 3];
				temps[tempsTop - 3] = temps[tempsTop];
				++tempsTop;
			opcode_end(1);


			opcode_case(LOADC_U)
				temps[tempsTop++] = nullptr;
			opcode_end(1);

			opcode_case(LOADC_B)
				temps[tempsTop++] = get_ubyte(1) != 0;
			opcode_end(2);

			opcode_case(LOADC_I)
				temps[tempsTop++] = get_sbyte(1);
			opcode_end(2);

			opcode_case(LOADC_R)
				temps[tempsTop++] = static_cast<double>(get_sbyte(1));
			opcode_end(2);

			opcode_case(LOADC)
				temps[tempsTop++] = callable->constant(get_ubyte(1));
			opcode_end(2);

			opcode_case(LOADCW)
				temps[tempsTop++] = callable->constant(get_uword(1));
			opcode_end(3);

			opcode_case(LOADCL)
				temps[tempsTop++] = callable->constant(get_ulong(1));
			opcode_end(5);


			opcode_case(LOAD_S)
				temps[tempsTop++] = *self;
			opcode_end(1);

			opcode_case(LOAD_0)
				temps[tempsTop++] = vars[0];
			opcode_end(1);

			opcode_case(LOAD_1)
				temps[tempsTop++] = vars[1];
			opcode_end(1);

			opcode_case(LOAD_2)
				temps[tempsTop++] = vars[2];
			opcode_end(1);

			opcode_case(LOAD_3)
				temps[tempsTop++] = vars[3];
			opcode_end(1);

			opcode_case(LOAD)
				temps[tempsTop++] = vars[get_ubyte(1)];
			opcode_end(2);


			opcode_case(NEW_ARRAY)
				temps[tempsTop++] = callable->heap().create_array();
			opcode_end(1);

			opcode_case(NEW_ARRAY_C)
				temps[tempsTop++] = callable->heap().create_array(get_ubyte(1));
			opcode_end(2);

			opcode_case(NEW_ARRAY_L)
				data::Integer idx = temps[tempsTop - 1].runtime_cast_integer(state);
				check_errors(1);
				temps[tempsTop - 1] = callable->heap().create_array(temps[tempsTop - 1].runtime_cast_integer(state));
			opcode_end(1);


			opcode_case(STORE_S)
				*self = temps[--tempsTop];
			opcode_end(1);
			
			opcode_case(STORE_0)
				vars[0] = temps[--tempsTop];
			opcode_end(1);

			opcode_case(STORE_1)
				vars[1] = temps[--tempsTop];
			opcode_end(1);

			opcode_case(STORE_2)
				vars[2] = temps[--tempsTop];
			opcode_end(1);

			opcode_case(STORE_3)
				vars[3] = temps[--tempsTop];
			opcode_end(1);

			opcode_case(STORE)
				vars[get_ubyte(1)] = temps[--tempsTop];
			opcode_end(2);
		}

	error_zone:
		return data::Value();
	}
}
