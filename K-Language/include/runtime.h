#pragma once

#include "data.h"
#include "opcodes.h"
#include "callable.h"

namespace k::runtime
{
	struct CallInfo
	{
		std::ptrdiff_t bottom;
		Callable* callable;
		std::ptrdiff_t offset;
	};

	class ValueStack
	{
	public:
		static constexpr Size default_value_count = 8192;
		static constexpr Size default_capacity = sizeof(data::Value) * default_value_count;

	private:
		data::Value* _bottom;
		data::Value* _top;
		data::Value* _current;
		Size _capacity;

	public:
		inline ValueStack() :
			_bottom(utils::malloc<data::Value>(default_capacity)),
			_top(_bottom + default_value_count),
			_current(_bottom),
			_capacity(default_capacity)
		{}

		inline ~ValueStack()
		{
			for (data::Value* value = _bottom; value < _current; ++value)
				value->~Value();

			utils::free(_bottom);
			_bottom = _top = _current = nullptr;
		}

		inline std::ptrdiff_t current_offset() { return _current - _bottom; }

		inline void push(Offset excluded_count, Offset args_count, Size len, data::Value** bottom)
		{
			len += 1;
			if (_current - excluded_count + len > _top)
			{
				data::Value* old = _bottom;

				_bottom = utils::malloc<data::Value>(_capacity + default_capacity);
				std::memcpy(_bottom, old, _capacity);

				_capacity = _capacity + default_capacity;
				_top = _bottom + _capacity;
				_current = _bottom + (_current - old);

				utils::free(old);
			}

			data::Value* base = _current - excluded_count;
			_current += len;

			if(args_count < len)
				std::memset(base + args_count, 0, sizeof(data::Value) * (len - args_count));

			*bottom = base;
		}

		inline void pop(std::uintptr_t bottom)
		{
			data::Value* up_current = _current;
			_current = _bottom + bottom;

			data::Value* current = _current;
			Size len = up_current - current;
			for (Offset i = 0; i < len; ++i)
				current[i].~Value();
		}
	};

	class CallStack
	{
	public:
		static constexpr Size info_count = 8192;
		static constexpr Size capacity = sizeof(CallInfo) * info_count;

	private:
		CallInfo* _bottom;
		CallInfo* _top;
		CallInfo* _current;

	public:
		inline CallStack() :
			_bottom(utils::malloc<CallInfo>(capacity)),
			_top(_bottom + info_count),
			_current(_bottom)
		{}

		inline ~CallStack()
		{
			utils::free(_bottom);
			_bottom = _top = _current = nullptr;
		}

		inline bool push(Callable* callable, std::ptrdiff_t bottom, Size len, std::ptrdiff_t offset)
		{
			if (_current >= _top)
				return false;

			_current->callable = callable;
			_current->bottom = bottom;
			_current->offset = offset;
			return true;
		}

		inline bool pushNative()
		{
			return push(nullptr, 0, 0, 0);
		}

		inline CallInfo* pop()
		{
			if (_current <= _bottom)
				return nullptr;

			return --_current;
		}
	};

	class RuntimeState;
	data::Value execute(RuntimeState& state, Callable& callable, const data::Value* self, const data::Value* args, Size argsCount);

	class RuntimeState
	{
	private:
		CallStack _calls;
		ValueStack _values;

		struct {
			data::Value error;
			bool state = false;
		} _error;

	public:
		inline bool hasError() const { return _error.state; }
		inline void setError(const data::Value& error)
		{
			_error.error = error;
			_error.state = true;
		}
		inline const data::Value& getError() const { return _error.error; }
		inline void clearError()
		{
			_error.error = nullptr;
			_error.state = false;
		}

	public:
		friend data::Value execute(RuntimeState& state, Callable& callable, const data::Value* self, const data::Value* args, Size argsCount);
	};
}
