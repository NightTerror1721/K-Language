#include "chunk.h"

namespace k
{
	#pragma warning(push)
	#pragma warning(disable:26495)
	Chunk::Constant::Constant(const Constant& right) :
		_type(right._type)
	{
		switch (_type)
		{
			default:
			case Type::Undefined:
				_undefined = nullptr;
				break;

			case Type::Integer:
				_integer = right._integer;
				break;

			case Type::Real:
				_real = right._real;
				break;

			case Type::Boolean:
				_boolean = right._boolean;
				break;

			case Type::String:
				_string = new std::string(*right._string);
				break;
		}
	}
	#pragma warning(pop)

	#pragma warning(push)
	#pragma warning(disable:26495)
	Chunk::Constant::Constant(Constant&& right) noexcept :
		_type(right._type)
	{
		switch (_type)
		{
			default:
			case Type::Undefined:
				_undefined = nullptr;
				break;

			case Type::Integer:
				_integer = right._integer;
				break;

			case Type::Real:
				_real = right._real;
				break;

			case Type::Boolean:
				_boolean = right._boolean;
				break;

			case Type::String:
				_string = new std::string(std::move(*right._string));
				delete right._string;
				break;
		}

		right._type = Type::Undefined;
		right._undefined = nullptr;
	}
	#pragma warning(pop)

	Chunk::Constant& Chunk::Constant::operator= (const Constant& right)
	{
		this->~Constant();
		return utils::copy(*this, right);
	}

	Chunk::Constant& Chunk::Constant::operator= (Constant&& right) noexcept
	{
		this->~Constant();
		return utils::move(*this, std::move(right));
	}

	Chunk::Constant::Constant::~Constant()
	{
		if (_type == Type::String)
			delete _string;

		_type = Type::Undefined;
		_undefined = nullptr;
	}

	Chunk::Constant& Chunk::Constant::operator= (decltype(nullptr))
	{
		this->~Constant();
		return *this;
	}

	Chunk::Constant& Chunk::Constant::operator= (bool right)
	{
		this->~Constant();
		_type = Type::Boolean;
		_boolean = right;
		return *this;
	}

	Chunk::Constant& Chunk::Constant::operator= (const std::string& right)
	{
		if (_type == Type::String)
			*_string = right;
		else
		{
			_type = Type::Boolean;
			_string = new std::string(right);
		}
		
		return *this;
	}

	Chunk::Constant& Chunk::Constant::operator= (std::string&& right)
	{
		if (_type == Type::String)
			*_string = std::move(right);
		else
		{
			_type = Type::Boolean;
			_string = new std::string(std::move(right));
		}

		return *this;
	}

	Chunk::Constant& Chunk::Constant::operator= (const char* right)
	{
		if (_type == Type::String)
			*_string = right;
		else
		{
			_type = Type::Boolean;
			_string = new std::string(right);
		}

		return *this;
	}

	Chunk::Constant& Chunk::Constant::operator= (const std::pair<const char*, Size>& right)
	{
		this->~Constant();
		_type = Type::String;
		_string = new std::string(right.first, right.second);
		return *this;
	}

	data::Value Chunk::Constant::make_value(mem::Heap& heap) const
	{
		switch (_type)
		{
			default:
			case Type::Undefined:
				return nullptr;

			case Type::Integer:
				return _integer;

			case Type::Real:
				return _real;

			case Type::Boolean:
				return _boolean;

			case Type::String:
				return heap.create_string(*_string);
		}
	}

	bool Chunk::Constant::operator== (const Constant& right) const
	{
		if (_type == right._type)
		{
			switch (_type)
			{
				case Type::Undefined:
					return true;

				case Type::Integer:
					return _integer == right._integer;

				case Type::Real:
					return _real == right._real;

				case Type::Boolean:
					return _boolean == right._boolean;

				case Type::String:
					return *_string == *right._string;
			}
		}
		return false;
	}

	std::strong_ordering Chunk::Constant::operator<=> (const Constant& right) const
	{
		if (_type == right._type)
		{
			switch (_type)
			{
				default:
				case Type::Undefined:
					return std::strong_ordering::equal;

				case Type::Integer:
					return _integer <=> right._integer;

				case Type::Real:
					return _real == right._real
						? std::strong_ordering::equal
						: _real > right._real ? std::strong_ordering::greater : std::strong_ordering::less;

				case Type::Boolean:
					return _boolean <=> right._boolean;

				case Type::String:
					return *_string == *right._string
						? std::strong_ordering::equal
						: *_string > *right._string ? std::strong_ordering::greater : std::strong_ordering::less;
			}
		}
		else return _type <=> right._type;
	}
}

namespace k
{
	Chunk::Chunk(
		mem::Heap& heap,
		Chunk* chunks,
		Size chunksCount,
		const Constant* constants,
		Size constantsCount,
		const instruction::InstructionValue* instructions,
		Size instructionsCount,
		Size varsCount,
		Size tempsCount
	) :
		_heap(&heap),
		_chunkCount(chunksCount),
		_chunks(chunksCount == 0 ? nullptr : new Chunk[chunksCount]),
		_constantsCount(constantsCount),
		_constants(constantsCount == 0 ? nullptr : new data::Value[constantsCount]),
		_instructionsCount(instructionsCount),
		_instructions(instructionsCount == 0 ? nullptr : new instruction::InstructionValue[instructionsCount]),
		_varsCount(varsCount),
		_tempsCount(tempsCount)
	{
		for (Offset i = 0; i < chunksCount; ++i)
			utils::move(_chunks[i], std::move(*chunks));

		for (Offset i = 0; i < constantsCount; ++i)
			_constants[i] = constants[i].make_value(heap);

		if (instructionsCount > 0)
			std::memcpy(_instructions, instructions, instructionsCount * sizeof(instruction::InstructionValue));
	}

	Chunk::~Chunk()
	{
		if (_chunks)
			delete[] _chunks;
		if (_constants)
			delete[] _constants;
		if (_instructions)
			delete[] _instructions;
	}

	Chunk::Chunk(Chunk&& right) noexcept :
		_heap(right._heap),
		_chunks(right._chunks),
		_chunkCount(right._chunkCount),
		_constants(right._constants),
		_constantsCount(right._constantsCount),
		_instructions(right._instructions),
		_instructionsCount(right._instructionsCount),
		_varsCount(right._varsCount),
		_tempsCount(right._tempsCount)
	{
		utils::construct(right);
	}

	Chunk& Chunk::operator= (Chunk&& right) noexcept
	{
		this->~Chunk();
		return utils::move(*this, std::move(right));
	}
}
