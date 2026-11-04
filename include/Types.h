#pragma once

#include <stddef.h>
#include <stdint.h>

// Signed integer types (Rust / LLVM style)
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

// Unsigned integer types (Rust / LLVM style)
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

// Floating-point types (Rust / LLVM style)
using f32 = float;
using f64 = double;

// Pointer-sized types
using usize = size_t;
using isize = ptrdiff_t;
