/*!
 * Copyright (c) Alibaba, Inc. and its affiliates.
 * @file    bfloat16_impl.hpp
 */

#ifndef UTILS_BFLOAT16_IMPL_HPP_
#define UTILS_BFLOAT16_IMPL_HPP_

#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
// #include "common.h"
#ifdef ENABLE_CUDA
#include <cuda/cudabfloat16_impl.hpp>
#endif

#define ROUND_MODE_TO_NEAREST_EVEN

#ifndef __BF16_IMPL
#define __BF16_IMPL

#define __BF16_DEVICE_FUNC inline
#define __BF16_DEVICE_FUNC_DECL
#ifndef DISABLE_COPY_AND_ASSIGN
#define DISABLE_COPY_AND_ASSIGN(classname) \
  classname(const classname&) = delete;    \
  classname& operator=(const classname&) = delete
#endif

#ifndef DISABLE_MOVE_COPY_ASSIGN
#define DISABLE_MOVE_COPY_ASSIGN(classname)        \
  classname(classname const&) = delete;            \
  classname(classname&&) = delete;                 \
  classname& operator=(classname const&) = delete; \
  classname& operator=(classname&&) = delete
#endif

#ifndef LIKELY
#if __GNUC__ > 2 || __GNUC_MINOR__ >= 96
#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#define EXPECTED(x, y) __builtin_expect((x), (y))
#else  //__GNUC__ > 2 || __GNUC_MINOR__ >= 96
#define LIKELY(x) (x)
#define UNLIKELY(x) (x)
#define EXPECTED(x, y) (x)
#endif  //__GNUC__ > 2 || __GNUC_MINOR__ >= 96
#endif

#ifdef __GNUC__
#define HIE_DEPRECATED __attribute__((deprecated))
#define HIE_ALIGN(x) __attribute__((aligned(x)))
#elif defined(_MSC_VER)
#define HIE_DEPRECATED __declspec(deprecated)
#define HIE_ALIGN(x) __declspec(align(x))
#else
#define HIE_DEPRECATED
#endif

#ifdef _WIN32
#define __restrict__ __restrict
#ifdef _MSC_VER
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif  // _MSC_VER
#endif  // _WIN32

#define TENSOR_ALIGN_IN_BYTES (256)
#define WORKSPACE_MEMORY_LIMIT (0x17fffffff)  // 6GB

#define HIE_EMPTY_TENSOR_NAME ("HIE_EMPTY_TENSOR")

namespace __hie_buildin {

// #ifdef ENABLE_FP16
// struct half;
// #endif

struct HIE_ALIGN(2) __Bf16Impl {
  std::uint16_t __x;

  __Bf16Impl() { __x = 0; }

  static __Bf16Impl from_bits(uint16_t bits) { return __Bf16Impl(bits); }

  // #ifdef ENABLE_FP16
  //     static __Bf16Impl half2bfloat16(half v);
  // #endif

  // from float to bf16, round to nearest even
  static __Bf16Impl float2bfloat16(float v) {
    uint32_t bits;
    // avoid using uninitialized val due to strict aliasing
    std::memcpy(&bits, &v, sizeof(bits));
    if ((bits & 0x7fffffff) > 0x7f800000) {
      return __Bf16Impl::from_bits(0x7fffU);
    } else {
      uint32_t lsb = (bits >> 16) & 1;
      uint32_t rounding_bias = 0x7fffU + lsb;
      bits += rounding_bias;
      uint16_t value = static_cast<uint16_t>(bits >> 16);
      return __Bf16Impl::from_bits(value);
    }
  }

  static __Bf16Impl double2bfloat16(double v) {
    return float2bfloat16(static_cast<float>(v));
  }

  static __Bf16Impl ll2bfloat16(long long v) {
    return float2bfloat16(static_cast<float>(v));
  }

  static __Bf16Impl ull2bfloat16(unsigned long long v) {
    return float2bfloat16(static_cast<float>(v));
  }

  static __Bf16Impl int2bfloat16(int v) {
    return float2bfloat16(static_cast<float>(v));
  }

  static __Bf16Impl uint2bfloat16(unsigned int v) {
    return float2bfloat16(static_cast<float>(v));
  }

  static __Bf16Impl short2bfloat16(short v) {
    return float2bfloat16(static_cast<float>(v));
  }

  static __Bf16Impl ushort2bfloat16(unsigned short v) {
    return float2bfloat16(static_cast<float>(v));
  }

  // from bf16 to float
  static float bfloat162float(__Bf16Impl v) {
    std::uint32_t val = static_cast<uint32_t>(v.__x) << 16;
    // avoid using uninitialized val due to strict aliasing
    float flt;
    std::memcpy(&flt, &val, sizeof(flt));
    return flt;
  }

  static double bfloat162double(__Bf16Impl v) {
    return static_cast<double>(bfloat162float(v));
  }

  static long long bfloat162ll(__Bf16Impl v) {
    return static_cast<long long>(bfloat162float(v));
  }

  static unsigned long long bfloat162ull(__Bf16Impl v) {
    return static_cast<unsigned long long>(bfloat162float(v));
  }

  static int bfloat162int(__Bf16Impl v) {
    return static_cast<int>(bfloat162float(v));
  }

  static unsigned int bfloat162uint(__Bf16Impl v) {
    return static_cast<unsigned int>(bfloat162float(v));
  }

  static short bfloat162short(__Bf16Impl v) {
    return static_cast<short>(bfloat162float(v));
  }

  static unsigned short bfloat162ushort(__Bf16Impl v) {
    return static_cast<unsigned short>(bfloat162float(v));
  }

  // + - * /
  static __Bf16Impl bf16add(__Bf16Impl a, __Bf16Impl b) {
    float val = bfloat162float(a) + bfloat162float(b);
    return float2bfloat16(val);
  }
  static __Bf16Impl bf16sub(__Bf16Impl a, __Bf16Impl b) {
    float val = bfloat162float(a) - bfloat162float(b);
    return float2bfloat16(val);
  }
  static __Bf16Impl bf16mul(__Bf16Impl a, __Bf16Impl b) {
    float val = bfloat162float(a) * bfloat162float(b);
    return float2bfloat16(val);
  }
  static __Bf16Impl bf16div(__Bf16Impl a, __Bf16Impl b) {
    float val = bfloat162float(a) / bfloat162float(b);
    return float2bfloat16(val);
  }

  // == != > < >= <=
  static bool bf16eq(__Bf16Impl a, __Bf16Impl b) {
    return bfloat162float(a) == bfloat162float(b);
  }
  static bool bf16ne(__Bf16Impl a, __Bf16Impl b) {
    return bfloat162float(a) != bfloat162float(b);
  }
  static bool bf16gt(__Bf16Impl a, __Bf16Impl b) {
    return bfloat162float(a) > bfloat162float(b);
  }
  static bool bf16lt(__Bf16Impl a, __Bf16Impl b) {
    return bfloat162float(a) < bfloat162float(b);
  }
  static bool bf16ge(__Bf16Impl a, __Bf16Impl b) {
    return bfloat162float(a) >= bfloat162float(b);
  }
  static bool bf16le(__Bf16Impl a, __Bf16Impl b) {
    return bfloat162float(a) <= bfloat162float(b);
  }

 private:
  // from bits
  explicit __Bf16Impl(std::uint16_t v) { __x = v; }
};

}  // namespace __hie_buildin
namespace std {
template <>
class numeric_limits<__hie_buildin::__Bf16Impl> {
  using bf16_impl_t = __hie_buildin::__Bf16Impl;

 public:
  static const bool is_specialized = true;
  static const bool is_signed = true;
  static const bool is_integer = false;
  static const bool is_exact = false;
  static const bool is_modulo = false;
  static const bool is_bounded = true;
  static const bool is_iec559 = false;
  static const bool has_infinity = true;
  static const bool has_quiet_NaN = true;
  static const bool has_signaling_NaN = false;
  static const float_denorm_style has_denorm = denorm_present;
  static const bool has_denorm_loss = false;
  static const bool traps = false;
  static const bool tinyness_before = false;
  static const float_round_style round_style = round_to_nearest;
  static const int digits = 8;
  static const int digits10 = 2;
  static const int max_digits10 = 4;
  static const int radix = 2;
  static const int min_exponent = -125;
  static const int min_exponent10 = -37;
  static const int max_exponent = 128;
  static const int max_exponent10 = 38;
  static bf16_impl_t min() { return bf16_impl_t::from_bits(0x0080); }
  static bf16_impl_t lowest() { return bf16_impl_t::from_bits(0xff7f); }
  static bf16_impl_t max() { return bf16_impl_t::from_bits(0x7f7f); }
  static bf16_impl_t epsilon() { return bf16_impl_t::from_bits(0x3c00); }
  static bf16_impl_t round_error() { return bf16_impl_t::from_bits(0x3f00); }
  static bf16_impl_t infinity() { return bf16_impl_t::from_bits(0x7F80); }
  static bf16_impl_t quiet_NaN() { return bf16_impl_t::from_bits(0x7fff); }
  static bf16_impl_t signaling_NaN() { return bf16_impl_t::from_bits(0x7fff); }
  static bf16_impl_t denorm_min() { return bf16_impl_t::from_bits(0x0001); }
};
}  // namespace std
#endif  // __BF16_IMPL

#endif  // UTILS_BFLOAT16_IMPL_HPP_
