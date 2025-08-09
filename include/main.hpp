#if !defined(COBBLE_MAIN)
#define COBBLE_MAIN
// Auto-generated configuration is included
#include "configuration.hpp"

#include <cstdint>

/// @brief Unsigned 8-bit integer scalar
using U8 = std::uint8_t;
/// @brief Unsigned 16-bit integer scalar
using U16 = std::uint16_t;
/// @brief Unsigned 32-bit integer scalar
using U32 = std::uint32_t;
/// @brief Unsigned 64-bit integer scalar
using U64 = std::uint64_t;

/// @brief Signed 8-bit integer scalar
using S8 = std::int8_t;
/// @brief Signed 16-bit integer scalar
using S16 = std::int16_t;
/// @brief Signed 32-bit integer scalar
using S32 = std::int32_t;
/// @brief Signed 64-bit integer scalar
using S64 = std::int64_t;

/// @brief Floating-point 32-bit scalar
using F32 = float;
/// @brief Floating-point 64-bit scalar
using F64 = double;

#ifdef ASM_PROBE_IN_USE
#warning "LLVM-MCA assembly probe capability enabled, did you mean to do that?"
#define MCA_START __asm volatile("# LLVM-MCA-BEGIN");
#define MCA_END __asm volatile("# LLVM-MCA-END");
#else
#define MCA_START
#define MCA_END
#endif

/// @brief The Cobble video sharing platform
namespace cobble {}
#endif