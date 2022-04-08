// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2019-2022 Second State INC

//===-- wasi_crypto/kx/kx.h - Key Exchange func -------------------------===//
//
// Part of the WasmEdge Project.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the key exchange relative function,
///
//===----------------------------------------------------------------------===//
#pragma once

#include "host/wasi_crypto/kx/registed.h"
#include "host/wasi_crypto/utils/error.h"

#include <cstdint>
#include <type_traits>
#include <vector>

namespace WasmEdge {
namespace Host {
namespace WasiCrypto {
namespace Kx {

using PkVariant = RegistedAlg::PkVariant;
using SkVariant = RegistedAlg::SkVariant;

/// Diffie-Hellman based key agreement
///
/// More detailed:
/// https://github.com/WebAssembly/wasi-crypto/blob/main/docs/wasi-crypto.md#diffie-hellman-based-key-agreement
WasiCryptoExpect<std::vector<uint8_t>> dh(PkVariant &Pk,
                                          SkVariant &Sk) noexcept;

/// Key encapsulation mechanisms
///
/// More detailed
/// https://github.com/WebAssembly/wasi-crypto/blob/main/docs/wasi-crypto.md#key-encapsulation-mechanisms
struct EncapsulatedSecret {
  std::vector<uint8_t> EncapsulatedSecretData;
  std::vector<uint8_t> Secret;
};

WasiCryptoExpect<EncapsulatedSecret> encapsulate(PkVariant &Pk) noexcept;

WasiCryptoExpect<std::vector<uint8_t>>
decapsulate(SkVariant &SkVariant,
            Span<const uint8_t> EncapsulatedSecret) noexcept;

} // namespace Kx
} // namespace WasiCrypto
} // namespace Host
} // namespace WasmEdge