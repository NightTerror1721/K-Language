#pragma once

#include "common.h"

namespace k::data { class Value; }

namespace k::mem
{
	class Heap;

	class MemoryBlock
	{
	private:
		Heap* _owner;
		MemoryBlock* _next;
		MemoryBlock* _prev;
		std::uintmax_t _refs;

	public:
		#pragma warning(push)
		#pragma warning(disable:26495)
		MemoryBlock() = default;
		#pragma warning(pop)

		virtual ~MemoryBlock() = default;

		MemoryBlock(const MemoryBlock&) = delete;
		MemoryBlock& operator= (const MemoryBlock&) = delete;

		inline void inc_ref() { ++_refs; }
		void dec_ref();

		friend class Heap;
		friend class data::Value;

	protected:

	};
}

namespace k::data
{
	enum class DataType
	{
		Undefined,

		Integer,
		Real,
		Boolean,
		String,

		Array,
		Object,

		Function,

		Iterator,

		Userdata
	};

	constexpr bool isScalarDataType(DataType type)
	{
		return static_cast<int>(type) <= static_cast<int>(DataType::Boolean);
	}

	typedef Int64 Integer;
	typedef double Real;
	typedef bool Boolean;

	class String;
	class Array;
	class Object;
	class Function;
	class Iterator;
	class Userdata;

	class Value
	{
	private:
		DataType _type;
		union {
			decltype(nullptr) undefined;
			Integer integer;
			Real real;
			Boolean boolean;
			String* string;
			Array* array;
			Object* object;
			Function* function;
			Iterator* iterator;
			Userdata* userdata;

			mem::MemoryBlock* _block;
		} _data;

	public:
		inline Value() : _type{ DataType::Undefined }, _data{ .undefined = nullptr } {}
		inline Value(const Value& right) : Value()
		{
			_type = right._type;
			if (!isScalarDataType(right._type))
				_data._block->inc_ref();
		}
		inline Value(Value&& right) noexcept : _type{ right._type }, _data{ std::move(right._data) }
		{
			right._type = DataType::Undefined;
			right._data.undefined = nullptr;
		}
		inline ~Value()
		{
			if (!isScalarDataType(_type))
				_data._block->dec_ref();
			
			_type = DataType::Undefined;
			_data.undefined = nullptr;
		}


		/* Direct Creation */
		inline Value(decltype(nullptr)) : Value() {}
		
		template<std::integral _Ty>
		inline Value(_Ty data) : _type{ DataType::Integer }, _data{ .integer = static_cast<Integer>(data) } {}

		template<std::floating_point _Ty>
		inline Value(_Ty data) : _type{ DataType::Real }, _data{ .real = static_cast<Real>(data) } {}

		inline Value(Boolean data) : _type{ DataType::Boolean }, _data{ .boolean = data } {}

		Value(String* data);
		Value(Array* data);
		Value(Object* data);
		Value(Function* data);
		Value(Iterator* data);
		Value(Userdata* data);

		
		/* Direct creation refs */
		inline Value(String& data) : Value(&data) {}
		inline Value(Array& data) : Value(&data) {}
		inline Value(Object& data) : Value(&data) {}
		inline Value(Function& data) : Value(&data) {}
		inline Value(Iterator& data) : Value(&data) {}
		inline Value(Userdata& data) : Value(&data) {}


		/* Copies and Moves */
		inline Value& operator= (decltype(nullptr))
		{
			this->~Value();
			return *this;
		}

		template<std::integral _Ty>
		inline Value& operator= (_Ty right)
		{
			if (!isScalarDataType(_type))
				_data._block->dec_ref();

			_type = DataType::Integer;
			_data.integer = static_cast<Integer>(right);
			return *this;
		}

		template<std::floating_point _Ty>
		inline Value& operator= (_Ty right)
		{
			if (!isScalarDataType(_type))
				_data._block->dec_ref();

			_type = DataType::Real;
			_data.real = static_cast<Real>(right);
			return *this;
		}

		inline Value& operator= (Boolean right)
		{
			if (!isScalarDataType(_type))
				_data._block->dec_ref();

			_type = DataType::Boolean;
			_data.boolean = right;
			return *this;
		}

		inline Value& operator= (String* right)
		{
			if (!isScalarDataType(_type))
				_data._block->dec_ref();

			_type = right ? DataType::Boolean : DataType::Undefined;
			_data.string = right;
			if (right)
				_data._block->inc_ref();
			return *this;
		}
		inline Value& operator= (String& right) { return *this = &right; }

		inline Value& operator= (Array* right)
		{
			if (!isScalarDataType(_type))
				_data._block->dec_ref();

			_type = right ? DataType::Array : DataType::Undefined;
			_data.array = right;
			if (right)
				_data._block->inc_ref();
			return *this;
		}
		inline Value& operator= (Array& right) { return *this = &right; }

		inline Value& operator= (Object* right)
		{
			if (!isScalarDataType(_type))
				_data._block->dec_ref();

			_type = right ? DataType::Object : DataType::Undefined;
			_data.object = right;
			if (right)
				_data._block->inc_ref();
			return *this;
		}
		inline Value& operator= (Object& right) { return *this = &right; }

		inline Value& operator= (Function* right)
		{
			if (!isScalarDataType(_type))
				_data._block->dec_ref();

			_type = right ? DataType::Function : DataType::Undefined;
			_data.function = right;
			if (right)
				_data._block->inc_ref();
			return *this;
		}
		inline Value& operator= (Function& right) { return *this = &right; }

		inline Value& operator= (Iterator* right)
		{
			if (!isScalarDataType(_type))
				_data._block->dec_ref();

			_type = right ? DataType::Iterator : DataType::Undefined;
			_data.iterator = right;
			if (right)
				_data._block->inc_ref();
			return *this;
		}
		inline Value& operator= (Iterator& right) { return *this = &right; }

		inline Value& operator= (Userdata* right)
		{
			if (!isScalarDataType(_type))
				_data._block->dec_ref();

			_type = right ? DataType::Userdata : DataType::Undefined;
			_data.userdata = right;
			if (right)
				_data._block->inc_ref();
			return *this;
		}
		inline Value& operator= (Userdata& right) { return *this = &right; }

		inline Value& operator= (const Value& right)
		{
			if (!isScalarDataType(_type))
				_data._block->dec_ref();

			_type = right._type;
			_data = right._data;
			if (!isScalarDataType(_type))
				_data._block->inc_ref();

			return *this;
		}

		inline Value& operator= (Value&& right) noexcept
		{
			if (!isScalarDataType(_type))
				_data._block->dec_ref();

			_type = right._type;
			_data = std::move(right._data);

			right._type = DataType::Undefined;
			right._data.undefined = nullptr;

			return *this;
		}

	public:
		inline mem::Heap* heap() const
		{
			return isScalarDataType(_type) ? nullptr : _data._block->_owner;
		}

		inline DataType type() const { return _type; }

		inline Integer integer() const { return _data.integer; }
		inline Real real() const { return _data.real; }
		inline Boolean boolean() const { return _data.boolean; }
		
		inline String& string() { return *_data.string; }
		inline const String& string() const { return *_data.string; }

		inline Array& array() { return *_data.array; }
		inline const Array& array() const { return *_data.array; }

		inline Object& object() { return *_data.object; }
		inline const Object& object() const { return *_data.object; }

		inline Function& function() { return *_data.function; }
		inline const Function& function() const { return *_data.function; }

		inline Iterator& iterator() { return *_data.iterator; }
		inline const Iterator& iterator() const { return *_data.iterator; }

		inline Userdata& userdata() { return *_data.userdata; }
		inline const Userdata& userdata() const { return *_data.userdata; }
	};

	

	class String : public mem::MemoryBlock, public std::string
	{
	public:
		String() = default;
		virtual ~String() = default;

		String(const String&) = delete;
		String& operator= (const String&) = delete;

	public:
		inline String(const char* str) :
			std::string(str)
		{}

		inline String(const char* str, Size size) :
			std::string(str, size)
		{}

		inline String(const std::string& str) :
			std::string(str)
		{}
	};

	class Array : public mem::MemoryBlock
	{
	private:
		std::vector<Value> _array;

	public:
		using iterator = decltype(_array)::iterator;
		using const_iterator = decltype(_array)::const_iterator;

	public:
		Array() = default;
		virtual ~Array() = default;

		Array(const Array&) = delete;
		Array& operator= (const Array&) = delete;

	public:
		Array(Value* array, Size len);
		Array(const std::vector<Value>& vector);
		Array(std::vector<Value>&& vector) noexcept;
		Array(std::initializer_list<Value> args);
		Array(Size length, const Value& default_value);
		explicit Array(Size length);

	public:
		inline Size length() const { return _array.size(); }
		inline Size size() const { return _array.size(); }

		inline bool empty() const { return _array.empty(); }

		inline void resize(Size new_len) { _array.resize(new_len); }

		inline void push_back(const Value& value) { _array.push_back(value); }
		inline void push_back(Value&& value) { _array.push_back(std::move(value)); }

		inline void insert(const_iterator where, const Value& value) { _array.insert(where, value); }
		inline void insert(const_iterator where, Value&& value) { _array.insert(where, std::move(value)); }

		inline Value& front() { return _array.front(); }
		inline const Value& front() const { return _array.front(); }

		inline Value& back() { return _array.back(); }
		inline const Value& back() const { return _array.back(); }

		inline void pop_back() { _array.pop_back(); }

		inline void erase(const_iterator where) { _array.erase(where); }
		inline void erase(const_iterator first, const_iterator last) { _array.erase(first, last); }

		inline void clear() { _array.clear(); }

	public:
		inline operator bool() const { return !_array.empty(); }
		inline bool operator! () const { return _array.empty(); }

		inline Value& operator[] (Offset index) { return _array[index]; }
		inline const Value& operator[] (Offset index) const { return _array[index]; }

	public:
		inline iterator begin() { return _array.begin(); }
		inline const_iterator begin() const { return _array.begin(); }
		inline const_iterator cbegin() const { return _array.cbegin(); }
		inline iterator end() { return _array.end(); }
		inline const_iterator end() const { return _array.end(); }
		inline const_iterator cend() const { return _array.cend(); }
	};

	class Object : public mem::MemoryBlock
	{
	public:
		class Property
		{
		private:
			bool _const = false;
			Value _value;

		public:
			inline Property(const Value& value, bool is_const = false) :
				_const{ is_const },
				_value{ value }
			{}
			inline Property(Value&& value, bool is_const = false) :
				_const{ is_const },
				_value{ std::move(value) }
			{}

			Property() = default;

			Property(const Property&) = default;
			Property& operator= (const Property&) noexcept = default;

			~Property() = default;

			Property(Property&&) noexcept = delete;
			Property& operator= (Property&&) noexcept = delete;

		public:
			inline bool isConst() const { return _const; }

			inline Value& value() { return _value; }
			inline const Value& value() const { return _value; }

			inline Value& operator* () { return _value; }
			inline const Value& operator* () const { return _value; }
		};

	private:
		std::unordered_map<std::string, Property> _props;
	};

	class Function : public mem::MemoryBlock {};
	class Iterator : public mem::MemoryBlock {};
	class Userdata : public mem::MemoryBlock {};
}



namespace k::data
{
	inline Value::Value(String* data) : _type{ data ? DataType::String : DataType::Undefined }, _data{ .string = data }
	{
		if (_data.string)
			_data.string->inc_ref();
	}

	inline Value::Value(Array* data) : _type{ data ? DataType::Array : DataType::Undefined }, _data{ .array = data }
	{
		if (_data.array)
			_data.array->inc_ref();
	}

	inline Value::Value(Object* data) : _type{ data ? DataType::Object : DataType::Undefined }, _data{ .object = data }
	{
		if (_data.object)
			_data.object->inc_ref();
	}

	inline Value::Value(Function* data) : _type{ data ? DataType::Function : DataType::Undefined }, _data{ .function = data }
	{
		if (_data.function)
			_data.function->inc_ref();
	}

	inline Value::Value(Iterator* data) : _type{ data ? DataType::Iterator : DataType::Undefined }, _data{ .iterator = data }
	{
		if (_data.iterator)
			_data.iterator->inc_ref();
	}

	inline Value::Value(Userdata* data) : _type{ data ? DataType::Userdata : DataType::Undefined }, _data{ .userdata = data }
	{
		if (_data.userdata)
			_data.userdata->inc_ref();
	}
}




namespace k::mem
{
	class Heap
	{
	private:
		MemoryBlock* _front;
		MemoryBlock* _back;

	public:
		Heap();
		~Heap();

		Heap(Heap&&) noexcept = default;
		Heap& operator= (Heap&&) noexcept = default;

		Heap(const Heap&) = delete;
		Heap& operator= (const Heap&) = delete;

	public:
		MemoryBlock* allocate(Size size);
		void deallocate(MemoryBlock* block);

	private:
		template<std::derived_from<MemoryBlock> _Ty, typename... _Args>
		_Ty* create_and_construct(_Args&&... args)
		{
			_Ty* data = reinterpret_cast<_Ty*>(allocate(sizeof(_Ty)));
			return &utils::construct<_Ty>(*data, std::forward<_Args>(args)...);
		}

	public:
		inline data::Value create_string() { return create_and_construct<data::String>(); }
		data::Value create_string(const char* str) { return create_and_construct<data::String>(str); }
		data::Value create_string(const std::string& str) { return create_and_construct<data::String>(str); }

		inline data::Value create_array() { return create_and_construct<data::Array>(); }
		inline data::Value create_array(Size len) { return create_and_construct<data::Array>(len); }
		inline data::Value create_array(Size len, const data::Value& default_value) { return create_and_construct<data::Array>(len, default_value); }
		inline data::Value create_array(data::Value* array, Size len) { return create_and_construct<data::Array>(array, len); }
		inline data::Value create_array(const std::vector<data::Value>& vector) { return create_and_construct<data::Array>(vector); }
		inline data::Value create_array(std::vector<data::Value>&& vector) { return create_and_construct<data::Array>(std::move(vector)); }
		inline data::Value create_array(std::initializer_list<data::Value> args) { return create_and_construct<data::Array>(args); }
	};


	inline void MemoryBlock::dec_ref()
	{
		if (_refs > 0)
			--_refs;
		
		if(_refs == 0 && _owner)
			_owner->deallocate(this);
	}
}
