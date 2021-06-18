#pragma once

#include <unordered_map>
#include <type_traits>
#include <exception>
#include <iostream>
#include <concepts>
#include <utility>
#include <cstdint>
#include <vector>
#include <list>
#include <map>

namespace k
{
	typedef std::int8_t  Int8;
	typedef std::int16_t Int16;
	typedef std::int32_t Int32;
	typedef std::int64_t Int64;

	typedef std::uint8_t  UInt8;
	typedef std::uint16_t UInt16;
	typedef std::uint32_t UInt32;
	typedef std::uint64_t UInt64;

	typedef std::byte Byte;

	typedef std::size_t Size;
	typedef std::size_t Offset;
}

namespace k::utils
{
	template<typename _Ty = void>
	inline _Ty* malloc(Size size)
	{
		return reinterpret_cast<_Ty*>(::operator new(size));
	}

	inline void free(void* ptr)
	{
		::operator delete(ptr);
	}

	template<typename _Ty, typename... _Args>
	inline _Ty& construct(_Ty& object, _Args&&... args)
	{
		return new (&object) _Ty(std::forward<_Args>(args)...), object;
	}

	template<typename _Ty>
	inline void destroy(_Ty& object)
	{
		object.~_Ty();
	}

	template<typename _Ty>
	inline _Ty& copy(_Ty& dst, const _Ty& src)
	{
		return construct(dst, src);
	}

	template<typename _Ty>
	inline _Ty& move(_Ty& dst, _Ty&& src)
	{
		return construct(dst, std::move(src));
	}


	template<typename _ValueTy, typename _MinTy, typename _MaxTy>
	constexpr _ValueTy clamp(_ValueTy value, _MinTy min, _MaxTy max)
	{
		if constexpr (std::same_as< _MinTy, _ValueTy>)
		{
			if constexpr (std::same_as< _MaxTy, _ValueTy>)
				return std::max(std::min(max, value), min);
			else return std::max(std::min(static_cast<_ValueTy>(max), value), min);
		}
		else
		{
			if constexpr (std::same_as< _MaxTy, _ValueTy>)
				return std::max(std::min(max, value), static_cast<_ValueTy>(min));
			else return std::max(std::min(static_cast<_ValueTy>(max), value), static_cast<_ValueTy>(min));
		}
	}
}

namespace k::error
{
	class RuntimeError : public std::exception
	{
	public:
		inline RuntimeError(const char* msg) noexcept : exception(msg) {}
		inline RuntimeError(const std::string& msg) noexcept : exception(msg.c_str()) {}
	};
}
