#pragma once

#include <concepts>
#include <cstdint>

using u1 = bool;
using u8 = uint8_t;
using u32 = uint32_t;
using s32 = int32_t;
using s32 = int32_t;
using f32 = float;
using u64 = uintptr_t;
using f64 = double;

template <typename T>
concept ToStringable =
requires(T t) {
    { t.to_string() };
};