#include "data.h"

namespace k::data
{
	using mem::MemoryBlock;


	Array::Array(Value* array, Size len) :
		_array(len)
	{
		for (Value* ptr = _array.data(); len > 0; --len, ++ptr, ++array)
			*ptr = *array;
	}

	Array::Array(const std::vector<Value>& vector) :
		_array(vector)
	{}

	Array::Array(std::vector<Value>&& vector) noexcept :
		_array(std::move(vector))
	{}

	Array::Array(std::initializer_list<Value> args) :
		_array(args)
	{}

	Array::Array(Size length, const Value& default_value) :
		_array(length, default_value)
	{}

	Array::Array(Size length) :
		_array(length)
	{}



	Object::Object(const Value& value, ConstructType type) :
		_props(),
		_parent(type == ConstructType::Parent ? value : nullptr),
		_class(type == ConstructType::Class ? value : nullptr)
	{}
	Object::Object(const std::unordered_map<std::string, Property>& props, const Value& value, ConstructType type) :
		_props(props),
		_parent(type == ConstructType::Parent ? value : nullptr),
		_class(type == ConstructType::Class ? value : nullptr)
	{}
	Object::Object(const std::unordered_map<std::string, Property>& props) :
		_props(props),
		_parent(),
		_class()
	{}

	Object::Property* Object::getProperty(const std::string& name)
	{
		const auto& it = _props.find(name);
		if (it == _props.end())
			return nullptr;
		return &it->second;
	}
	const Object::Property* Object::getProperty(const std::string& name) const
	{
		const auto& it = _props.find(name);
		if (it == _props.end())
			return nullptr;
		return &it->second;
	}

	bool Object::insert(const std::string& name, const Value& value, bool isConst)
	{
		auto result = _props.emplace(
			std::piecewise_construct,
			std::forward_as_tuple(name),
			std::forward_as_tuple(value, isConst)
		);

		return result.second;
	}
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
