#pragma once

#include "opcodes.h"

namespace k::instruction
{
	typedef UInt8 InstructionValue;

	namespace arg
	{
		typedef Int8	sbyte;
		typedef UInt8	ubyte;
		typedef Int16	sword;
		typedef UInt16	uword;
		typedef Int32	slong;
		typedef UInt32	ulong;
		typedef Int64	squad;
		typedef UInt64	uquad;

		template<typename _Ty>
		constexpr _Ty get(const InstructionValue* data)
		{
			if constexpr (std::same_as<_Ty, sbyte> || std::same_as<_Ty, ubyte>)
			{
				return static_cast<_Ty>(*data);
			}
			else if constexpr (std::same_as<_Ty, sword> || std::same_as<_Ty, uword>)
			{
				return static_cast<_Ty>(
					(static_cast<UInt16>(reinterpret_cast<const UInt8*>(data)[0])) |
					(static_cast<UInt16>(reinterpret_cast<const UInt8*>(data)[1]) << 8)
				);
			}
			else if constexpr (std::same_as<_Ty, slong> || std::same_as<_Ty, ulong>)
			{
				return static_cast<_Ty>(
					(static_cast<UInt32>(reinterpret_cast<const UInt8*>(data)[0])) |
					(static_cast<UInt32>(reinterpret_cast<const UInt8*>(data)[1]) << 8) |
					(static_cast<UInt32>(reinterpret_cast<const UInt8*>(data)[2]) << 16) |
					(static_cast<UInt32>(reinterpret_cast<const UInt8*>(data)[3]) << 24)
				);
			}
			else if constexpr (std::same_as<_Ty, squad> || std::same_as<_Ty, uquad>)
			{
				return static_cast<_Ty>(
					(static_cast<UInt64>(reinterpret_cast<const UInt8*>(data)[0])) |
					(static_cast<UInt64>(reinterpret_cast<const UInt8*>(data)[1]) << 8) |
					(static_cast<UInt64>(reinterpret_cast<const UInt8*>(data)[2]) << 16) |
					(static_cast<UInt64>(reinterpret_cast<const UInt8*>(data)[3]) << 24) |
					(static_cast<UInt64>(reinterpret_cast<const UInt8*>(data)[4]) << 32) |
					(static_cast<UInt64>(reinterpret_cast<const UInt8*>(data)[5]) << 40) |
					(static_cast<UInt64>(reinterpret_cast<const UInt8*>(data)[6]) << 48) |
					(static_cast<UInt64>(reinterpret_cast<const UInt8*>(data)[7]) << 56)
				);
			}
			else
			{
				static_assert(false, "Invalid instruction argument extraction type");
			}
		}

		template<typename _Ty>
		void set(void* data, _Ty value)
		{
			UInt8* ptr = reinterpret_cast<UInt8*>(data);
			if constexpr (std::same_as<_Ty, sbyte> || std::same_as<_Ty, ubyte>)
			{
				*ptr = static_cast<UInt8>(value);
			}
			else if constexpr (std::same_as<_Ty, sword> || std::same_as<_Ty, uword>)
			{
				ptr[0] = static_cast<UInt8>(value & 0xffU);
				ptr[1] = static_cast<UInt8>((value >> 8) & 0xffU);
			}
			else if constexpr (std::same_as<_Ty, slong> || std::same_as<_Ty, ulong>)
			{
				ptr[0] = static_cast<UInt8>(value & 0xffU);
				ptr[1] = static_cast<UInt8>((value >> 8) & 0xffU);
				ptr[2] = static_cast<UInt8>((value >> 16) & 0xffU);
				ptr[3] = static_cast<UInt8>((value >> 24) & 0xffU);
			}
			else if constexpr (std::same_as<_Ty, squad> || std::same_as<_Ty, uquad>)
			{
				ptr[0] = static_cast<UInt8>(value & 0xffU);
				ptr[1] = static_cast<UInt8>((value >> 8) & 0xffU);
				ptr[2] = static_cast<UInt8>((value >> 16) & 0xffU);
				ptr[3] = static_cast<UInt8>((value >> 24) & 0xffU);
				ptr[4] = static_cast<UInt8>((value >> 32) & 0xffU);
				ptr[5] = static_cast<UInt8>((value >> 40) & 0xffU);
				ptr[6] = static_cast<UInt8>((value >> 48) & 0xffU);
				ptr[7] = static_cast<UInt8>((value >> 56) & 0xffU);
			}
			else
			{
				static_assert(false, "Invalid instruction argument set type");
			}
		}
	}
}
