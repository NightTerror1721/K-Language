#pragma once

#include "instructions.h"
#include "data.h"

namespace k
{
	class Chunk
	{
	public:
		#pragma warning(push)
		#pragma warning(disable:26495)
		class Constant
		{
		public:
			enum class Type { Undefined, Integer, Real, Boolean, String };

		private:
			Type _type;
			union
			{
				decltype(nullptr) _undefined;
				data::Integer _integer;
				data::Real _real;
				data::Boolean _boolean;
				std::string* _string;
			};

		public:
			inline Constant() : _type(Type::Undefined), _undefined(nullptr) {}
			inline Constant(decltype(nullptr)) : Constant() {}

			template<std::integral _Ty>
			inline Constant(_Ty right) : _type(Type::Integer), _integer(static_cast<data::Integer>(right)) {}

			template<std::floating_point _Ty>
			inline Constant(_Ty right) : _type(Type::Real), _real(static_cast<data::Real>(right)) {}

			inline Constant(bool right) : _type(Type::Boolean), _boolean(right) {}

			inline Constant(const std::string& right) : _type(Type::String), _string(new std::string(right)) {}
			inline Constant(std::string&& right) : _type(Type::String), _string(new std::string(std::move(right))) {}

			inline Constant(const char* right) : _type(Type::String), _string(new std::string(right)) {}
			inline Constant(const char* right, Size size) : _type(Type::String), _string(new std::string(right, size)) {}

		public:
			Constant(const Constant& right);
			Constant& operator= (const Constant& right);

			Constant(Constant&&) noexcept;
			Constant& operator= (Constant&&) noexcept;

			~Constant();

			Constant& operator= (decltype(nullptr));
			Constant& operator= (bool right);
			Constant& operator= (const std::string& right);
			Constant& operator= (std::string&& right);
			Constant& operator= (const char* right);
			Constant& operator= (const std::pair<const char*, Size>& right);

			template<std::integral _Ty>
			Constant& operator= (_Ty right)
			{
				this->~Constant();
				_type = Type::Integer;
				_integer = static_cast<data::Integer>(right);
				return *this;
			}

			template<std::floating_point _Ty>
			Constant& operator= (_Ty right)
			{
				this->~Constant();
				_type = Type::Real;
				_real = static_cast<data::Real>(right);
				return *this;
			}

		public:
			inline Type type() const { return _type; }
			inline data::Integer integer() const { return _integer; }
			inline data::Real real() const { return _real; }
			inline data::Boolean boolean() const { return _boolean; }
			inline const std::string& string() const { return *_string; }

		public:
			data::Value make_value(mem::Heap& heap) const;

		public:
			bool operator== (const Constant& right) const;
			std::strong_ordering operator<=> (const Constant& right) const;
		};
		#pragma warning(pop)

	private:
		mutable mem::Heap* _heap = nullptr;

		Chunk* _chunks = nullptr;
		Size _chunkCount = 0;

		data::Value* _constants = nullptr;
		Size _constantsCount = 0;

		instruction::InstructionValue* _instructions = nullptr;
		Size _instructionsCount = 0;

		Size _varsCount = 0;
		Size _tempsCount = 0;

	public:
		Chunk() = default;

		Chunk(const Chunk&) = delete;
		Chunk& operator= (const Chunk&) = delete;

	public:
		Chunk(
			mem::Heap& heap,
			Chunk* chunks,
			Size chunksCount,
			const Constant* constants,
			Size constantsCount,
			const instruction::InstructionValue* instructions,
			Size instructionsCount,
			Size varsCount,
			Size tempsCount
		);
		~Chunk();

		Chunk(Chunk&& right) noexcept;
		Chunk& operator= (Chunk&& right) noexcept;

		inline Chunk(
			mem::Heap& heap,
			const std::vector<Chunk>& chunks,
			const std::vector<Constant>& constants,
			const std::vector<instruction::InstructionValue>& instructions,
			Size varsCount,
			Size tempsCount
		) : Chunk(
			heap,
			const_cast<Chunk*>(chunks.data()), chunks.size(),
			constants.data(), constants.size(),
			instructions.data(), instructions.size(),
			varsCount,
			tempsCount
		) {}

	public:
		inline mem::Heap& heap() const { return *_heap; }

		inline const Chunk* chunk(Offset offset) const { return _chunks + offset; }
		inline Size chunksCount() const { return _chunkCount; }

		inline const data::Value& constant(Offset offset) const { return _constants[offset]; }
		inline Size constantsCount() const { return _constantsCount; }

		inline const instruction::InstructionValue* instructionData(Offset offset = 0) const
		{
			return _instructions + offset;
		}
		inline instruction::InstructionValue instruction(Offset offset = 0) const
		{
			return _instructions[offset];
		}
		inline Size instructionsCount() const { return _instructionsCount; }

		inline Size varsCount() const { return _varsCount; }
		inline Size tempsCount() const { return _tempsCount; }
		inline Size stackCount() const { return _varsCount + _tempsCount; }
	};
}
