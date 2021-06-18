#include "data.h"
#include "callable.h"
#include "runtime.h"

namespace k::data
{
	using mem::MemoryBlock;


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



	Object::Object(const Value& value, ConstructType type) :
		MemoryBlock(),
		_props(),
		_parent(type == ConstructType::Parent ? value : nullptr),
		_class(type == ConstructType::Class ? value : nullptr)
	{}
	Object::Object(const std::unordered_map<std::string, Property>& props, const Value& value, ConstructType type) :
		MemoryBlock(),
		_props(props),
		_parent(type == ConstructType::Parent ? value : nullptr),
		_class(type == ConstructType::Class ? value : nullptr)
	{}
	Object::Object(const std::unordered_map<std::string, Property>& props) :
		MemoryBlock(),
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



	Function::Function(const Chunk& chunk, Size upsCount, const std::string& name) :
		MemoryBlock(),
		_name(name),
		_callable(new Callable(chunk, upsCount))
	{}

	Function::~Function()
	{
		if (_callable)
			delete _callable;
	}

	void Function::call(runtime::RuntimeState& state, const Value* args, Size argsCount)
	{
		runtime::execute(state, *_callable, args, argsCount);
	}
	/*void Function::call(runtime::RuntimeState& state, std::initializer_list<Value> args);
	void Function::call(runtime::RuntimeState& state, const std::vector<Value>& args);

	void Function::invoke(runtime::RuntimeState& state, const Value& self, const Value* args, Size argsCount);
	void Function::invoke(runtime::RuntimeState& state, const Value& self, std::initializer_list<Value> args);
	void Function::invoke(runtime::RuntimeState& state, const Value& self, const std::vector<Value>& args);*/
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

namespace k::data
{
	Integer Value::runtime_cast_integer(runtime::RuntimeState& state) const
	{
		switch (_type)
		{
			case DataType::Undefined: return 0;
			case DataType::Integer: return _data.integer;
			case DataType::Real: return static_cast<Integer>(_data.real);
			case DataType::Boolean: return static_cast<Integer>(_data.boolean);
		}

		return 0;
	}

	/*Real Value::runtime_cast_real(runtime::RuntimeState& state) const
	{

	}

	Boolean Value::runtime_cast_boolean(runtime::RuntimeState& state) const
	{

	}*/
}
