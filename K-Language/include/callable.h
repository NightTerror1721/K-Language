#pragma once

#include "chunk.h"

namespace k
{
	class Callable
	{
	private:
		const Chunk* _chunk = nullptr;

		data::Value* _ups = nullptr;
		Size _upsCount = 0;
		
		std::unordered_map<std::string, data::Value> _locals;

	public:
		Callable() = default;

		Callable(const Callable&) = delete;
		Callable& operator= (const Callable&) = delete;

	public:
		Callable(const Chunk& chunk, Size upsCount);

		Callable(Callable&& right) noexcept;
		Callable& operator= (Callable&& right) noexcept;

		~Callable();

	public:
		inline const Chunk& chunk() const { return *_chunk; }

		inline Size upsCount() const { return _upsCount; }

		inline const data::Value& setLocalValue(const std::string& name, const data::Value& value)
		{
			return _locals.insert({ name, value }), value;
		}

		inline const data::Value getLocalValue(const std::string& name)
		{
			const auto& it = _locals.find(name);
			if (it == _locals.end())
				return data::Value();
			return it->second;
		}

	public:
		inline mem::Heap& heap() const { return _chunk->heap(); }

		inline const Chunk* chunk(Offset offset) const { return _chunk->chunk(offset); }
		inline Size chunksCount() const { return _chunk->chunksCount(); }

		inline const data::Value& constant(Offset offset) const { return _chunk->constant(offset); }
		inline Size constantsCount() const { return _chunk->constantsCount(); }

		inline const instruction::InstructionValue* instructionData(Offset offset = 0) const
		{
			return _chunk->instructionData(offset);
		}
		inline instruction::InstructionValue instruction(Offset offset = 0) const
		{
			return _chunk->instruction(offset);
		}
		inline Size instructionsCount() const { return _chunk->instructionsCount(); }

		inline Size varsCount() const { return _chunk->varsCount(); }
		inline Size tempsCount() const { return _chunk->tempsCount(); }
		inline Size stackCount() const { return _chunk->stackCount(); }

	public:
		void setUps(const data::Value* src);
		void getUps(data::Value* dst);
	};
}
