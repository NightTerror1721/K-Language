#include "callable.h"

namespace k
{
	Callable::Callable(const Chunk& chunk, Size upsCount) :
		_chunk(&chunk),
		_ups(upsCount == 0 ? nullptr : new data::Value[upsCount]),
		_upsCount(upsCount),
		_locals()
	{}

	Callable::Callable(Callable&& right) noexcept :
		_chunk(right._chunk),
		_ups(right._ups),
		_upsCount(right._upsCount),
		_locals(std::move(right._locals))
	{
		right.~Callable();
	}

	Callable& Callable::operator= (Callable&& right) noexcept
	{
		this->~Callable();
		return utils::move(*this, std::move(right));
	}

	Callable::~Callable()
	{
		if (_ups)
		{
			delete[] _ups;
			_ups = nullptr;
			_upsCount = 0;
		}
	}

	void Callable::setUps(const data::Value* src)
	{
		Size len = _upsCount;
		data::Value* ups = _ups;

		for (Offset i = 0; i < len; ++i)
			ups[i] = src[i];
	}

	void Callable::getUps(data::Value* dst)
	{
		Size len = _upsCount;
		data::Value* ups = _ups;

		for (Offset i = 0; i < len; ++i)
			dst[i] = ups[i];
	}
}
