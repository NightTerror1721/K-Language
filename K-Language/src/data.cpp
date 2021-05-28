#include "data.h"

namespace k::data
{
	using mem::MemoryBlock;

	Value Value::clone()
	{
		switch (_type)
		{
			case DataType::Undefined:
				return Value();

			case DataType::Integer:
				return Value(_data.integer);

			case DataType::Real:
				return Value(_data.real);

			case DataType::Boolean:
				return Value(_data.boolean);

			case DataType::String:
				
		}
	}



	Array::Array(Value* array, Size len) :
		MemoryBlock(),
		_array(len)
	{
		for (Value* ptr = _array.data(); len > 0; --len, ++ptr, ++array)
			*ptr = *array;
	}

	Array::Array(const std::vector<Value>& vector) :
		MemoryBlock(),
		_array(vector)
	{}

	Array::Array(std::vector<Value>&& vector) noexcept :
		MemoryBlock(),
		_array(std::move(vector))
	{}

	Array::Array(std::initializer_list<Value> args) :
		MemoryBlock(),
		_array(args)
	{}

	Array::Array(Size length, const Value& default_value) :
		MemoryBlock(),
		_array(length, default_value)
	{}

	Array::Array(Size length) :
		MemoryBlock(),
		_array(length)
	{}
}

namespace k::mem
{
	Heap::Heap() :
		_front{ nullptr },
		_back{ nullptr }
	{}

	Heap::~Heap()
	{
		for (MemoryBlock* block = _front, *next; block; block = next)
		{
			next = block->_next;
			utils::destroy(*block);
			utils::free(block);
		}

		_front = _back = nullptr;
	}

	MemoryBlock* Heap::allocate(Size size)
	{
		MemoryBlock* block = utils::malloc<MemoryBlock>(std::max(size, sizeof(MemoryBlock)));
		block->_owner = this;
		block->_next = nullptr;
		block->_prev = _back;
		block->_refs = 0;

		if (!_front)
			_front = _back = block;
		else
		{
			_back->_next = block;
			_back = block;
		}

		return block;
	}

	void Heap::deallocate(MemoryBlock* block)
	{
		if (block->_owner == this)
		{
			if (block->_next)
				block->_next->_prev = block->_prev;
			if (block->_prev)
				block->_prev->_next = block->_next;

			if (block->_owner->_front == block)
				block->_owner->_front = block->_next;
			if (block->_owner->_back == block)
				block->_owner->_back = block->_prev;

			utils::destroy(block);
			utils::free(block);
		}
	}
}