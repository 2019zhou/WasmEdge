// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2019-2022 Second State INC

//===-- wasi_crypto/signatures/eddsa.h - Eddsa algorithm implement --------===//
//
// Part of the WasmEdge Project.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the definition of eddsa relative algorithm
///
//===----------------------------------------------------------------------===//
#pragma once

#include "common/span.h"
#include "host/wasi_crypto/signatures/options.h"
#include "host/wasi_crypto/utils/error.h"
#include "host/wasi_crypto/utils/evp_wrapper.h"
#include "host/wasi_crypto/utils/optional.h"
#include "host/wasi_crypto/utils/secret_vec.h"

#include <memory>
#include <shared_mutex>
#include <vector>

namespace WasmEdge {
namespace Host {
namespace WasiCrypto {
namespace Signatures {

class Eddsa {
public:
  class Signature {
  public:
    Signature(std::vector<uint8_t> &&Data) noexcept : Data(std::move(Data)) {}

    static WasiCryptoExpect<Signature>
    import(Span<const uint8_t> Encoded,
           __wasi_signature_encoding_e_t Encoding) noexcept;

    WasiCryptoExpect<std::vector<uint8_t>>
    exportData(__wasi_signature_encoding_e_t Encoding) const noexcept;

    const std::vector<uint8_t> &ref() const { return Data; }

  private:
    std::vector<uint8_t> Data;
  };

  class SignState {
  public:
    SignState(EvpMdCtxPtr Ctx) noexcept
        : Ctx(std::make_shared<Inner>(std::move(Ctx))) {}

    WasiCryptoExpect<void> update(Span<uint8_t const> Input) noexcept;

    WasiCryptoExpect<Signature> sign() noexcept;

  private:
    struct Inner {
      Inner(EvpMdCtxPtr Ctx) noexcept : RawCtx(std::move(Ctx)) {}
      std::mutex Mutex;
      std::vector<uint8_t> Data;
      EvpMdCtxPtr RawCtx;
    };
    std::shared_ptr<Inner> Ctx;
  };

  class VerificationState {
  public:
    VerificationState(EvpMdCtxPtr Ctx) noexcept
        : Ctx(std::make_shared<Inner>(std::move(Ctx))) {}

    WasiCryptoExpect<void> update(Span<const uint8_t> Input) noexcept;

    WasiCryptoExpect<void> verify(const Signature &Sig) noexcept;

  private:
    struct Inner {
      Inner(EvpMdCtxPtr Ctx) noexcept : RawCtx(std::move(Ctx)) {}
      std::mutex Mutex;
      std::vector<uint8_t> Data;
      EvpMdCtxPtr RawCtx;
    };
    std::shared_ptr<Inner> Ctx;
  };

  class KeyPair;

  class PublicKey {
  public:
    PublicKey(EvpPkeyPtr Ctx) noexcept : Ctx(std::move(Ctx)) {}

    PublicKey(std::shared_ptr<EVP_PKEY> Ctx) noexcept : Ctx(std::move(Ctx)) {}

    static WasiCryptoExpect<PublicKey>
    import(Span<const uint8_t> Encoded,
           __wasi_publickey_encoding_e_t Encoding) noexcept;

    WasiCryptoExpect<void> verify() const noexcept;

    WasiCryptoExpect<std::vector<uint8_t>>
    exportData(__wasi_publickey_encoding_e_t Encoding) const noexcept;

    WasiCryptoExpect<VerificationState> openVerificationState() const noexcept;

  private:
    std::shared_ptr<EVP_PKEY> Ctx;
  };

  class SecretKey {
  public:
    SecretKey(EvpPkeyPtr Ctx) noexcept : Ctx(std::move(Ctx)) {}

    SecretKey(std::shared_ptr<EVP_PKEY> Ctx) noexcept : Ctx(std::move(Ctx)) {}

    static WasiCryptoExpect<SecretKey>
    import(Span<const uint8_t> Encoded,
           __wasi_secretkey_encoding_e_t Encoding) noexcept;

    WasiCryptoExpect<PublicKey> publicKey() const noexcept;

    WasiCryptoExpect<KeyPair> toKeyPair(const PublicKey &Pk) const noexcept;

    WasiCryptoExpect<SecretVec>
    exportData(__wasi_secretkey_encoding_e_t Encoding) const noexcept;

  private:
    std::shared_ptr<EVP_PKEY> Ctx;
  };

  class KeyPair {
  public:
    KeyPair(EvpPkeyPtr Ctx) noexcept : Ctx(std::move(Ctx)) {}
    
    KeyPair(std::shared_ptr<EVP_PKEY> Ctx) noexcept : Ctx(std::move(Ctx)) {}

    static WasiCryptoExpect<KeyPair>
    generate(OptionalRef<const Options> Options) noexcept;

    static WasiCryptoExpect<KeyPair>
    import(Span<const uint8_t> Encoded,
           __wasi_keypair_encoding_e_t Encoding) noexcept;

    WasiCryptoExpect<SecretVec>
    exportData(__wasi_keypair_encoding_e_t Encoding) const noexcept;

    WasiCryptoExpect<PublicKey> publicKey() const noexcept;

    WasiCryptoExpect<SecretKey> secretKey() const noexcept;

    WasiCryptoExpect<SignState> openSignState() const noexcept;

  private:
    std::shared_ptr<EVP_PKEY> Ctx;
  };
};
} // namespace Signatures
} // namespace WasiCrypto
} // namespace Host
} // namespace WasmEdge
