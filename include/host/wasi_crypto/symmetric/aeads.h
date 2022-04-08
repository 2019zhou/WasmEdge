// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2019-2022 Second State INC

//===-- wasi_crypto/symmetric/aeads.h - Aeads relative declaration --------===//
//
// Part of the WasmEdge Project.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of Aeads relative class.
///
//===----------------------------------------------------------------------===//
#pragma once

#include "host/wasi_crypto/symmetric/options.h"
#include "host/wasi_crypto/symmetric/tag.h"
#include "host/wasi_crypto/utils/evp_wrapper.h"
#include "host/wasi_crypto/utils/optional.h"
#include "host/wasi_crypto/utils/secret_vec.h"
#include "wasi_crypto/api.hpp"

namespace WasmEdge {
namespace Host {
namespace WasiCrypto {
namespace Symmetric {

/// Aeads invalid operation, every Aeads state should inherent from this class
///
/// More detailed:
/// https://github.com/WebAssembly/wasi-crypto/blob/main/docs/wasi-crypto.md#aeads
template <typename Key> class AEADsState {
public:
  WasiCryptoExpect<uint64_t> optionsGetU64(std::string_view) const noexcept {
    return WasiCryptoUnexpect(__WASI_CRYPTO_ERRNO_UNSUPPORTED_OPTION);
  }

  WasiCryptoExpect<void> squeeze(Span<uint8_t>) noexcept {
    return WasiCryptoUnexpect(__WASI_CRYPTO_ERRNO_INVALID_OPERATION);
  }

  WasiCryptoExpect<void> ratchet() noexcept {
    return WasiCryptoUnexpect(__WASI_CRYPTO_ERRNO_INVALID_OPERATION);
  }

  WasiCryptoExpect<Key> squeezeKey() noexcept {
    return WasiCryptoUnexpect(__WASI_CRYPTO_ERRNO_INVALID_OPERATION);
  }

  WasiCryptoExpect<Tag> squeezeTag() noexcept {
    return WasiCryptoUnexpect(__WASI_CRYPTO_ERRNO_INVALID_OPERATION);
  }
};

template <int CipherNid> class Cipher {
  static inline constexpr size_t NonceSize = 12;

public:
  class Key {
  public:
    Key(std::shared_ptr<SecretVec> Data) noexcept : Data(std::move(Data)) {}

    static WasiCryptoExpect<Key> import(Span<const uint8_t> Data) noexcept;

    static WasiCryptoExpect<Key>
    generate(OptionalRef<const Options> Options) noexcept;

    std::vector<uint8_t> exportData() const noexcept { return Data->raw(); }

    const std::vector<uint8_t> &ref() const noexcept { return Data->raw(); }

  private:
    const std::shared_ptr<SecretVec> Data;
  };

  // Nonce = IV,
  class State : public AEADsState<Key> {
  public:
    /// There are four inputs for authenticated encryption:
    /// @param[in] Key The secret key for encrypt
    /// @param[in] OptOption `Must` contain an Nonce(Initialization vector).
    static WasiCryptoExpect<State>
    open(const Key &Key, OptionalRef<const Options> OptOption) noexcept;

    State(EvpCipherCtxPtr Ctx, std::array<uint8_t, NonceSize> Nonce) noexcept
        : Ctx(std::make_shared<Inner>(std::move(Ctx), Nonce)) {}

    WasiCryptoExpect<size_t> optionsGet(std::string_view Name,
                                        Span<uint8_t> Value) const noexcept;

    /// Absorbs additional data. Multiple calls to absorb() MUST be equivalent
    /// to a single call with a concatenation of the inputs.
    ///
    /// @param Data Additional data
    /// @return Nothing or WasiCrypto error
    WasiCryptoExpect<void> absorb(Span<const uint8_t> Data) noexcept;

    /// Return the length required to encode the authentication tag
    /// and optional padding bytes. The returned length MUST be constant for a
    /// given algorithm. Guest applications are expected to provide an output
    /// buffer whose size is the size of the message, plus the max_tag_len()
    /// output value.
    ///
    /// @return the length required to encode the authentication tag
    /// and optional padding bytes.
    WasiCryptoExpect<size_t> maxTagLen() const noexcept;

    /// Check Out.size() == Data.size() + maxTagLen(), then call
    /// encryptUnchecked(Out, Data) or return error if not equal
    ///
    /// @param Out The encrypted data text
    /// @param Data The data to be encrypted
    /// @return Tag's size or
    /// `__WASI_CRYPTO_ERRNO_OVERFLOW`/`__WASI_CRYPTO_ERRNO_INVALID_LENGTH`
    WasiCryptoExpect<size_t> encrypt(Span<uint8_t> Out,
                                     Span<const uint8_t> Data) noexcept;

    /// Check Out.size() == Data.size(), then call
    /// encryptDetachedUnchecked(Out, Data) or error if not equal
    ///
    /// @param Out The encrypted data text
    /// @param Data The data to be encrypted
    /// @return Tag
    /// or `__WASI_CRYPTO_ERRNO_OVERFLOW`/`__WASI_CRYPTO_ERRNO_INVALID_LENGTH`
    WasiCryptoExpect<Tag> encryptDetached(Span<uint8_t> Out,
                                          Span<const uint8_t> Data) noexcept;

    /// Check Out.size() = Data.size() + maxTagLen(), then call
    /// decryptDetachedUnchecked(Out, Data) or error if not equal
    ///
    /// @param Out The decrypted data text
    /// @param Data The data to be decrypted
    /// @return Size or
    /// `__WASI_CRYPTO_ERRNO_OVERFLOW`/`__WASI_CRYPTO_ERRNO_INVALID_LENGTH`
    WasiCryptoExpect<size_t> decrypt(Span<uint8_t> Out,
                                     Span<const uint8_t> Data) noexcept;

    /// Check Out.size() == Data.size(), then call
    /// encryptDetachedUnchecked(Out, Data) or error if not equal
    ///
    /// @param Out The decrypted data text
    /// @param Data The data to be decrypted
    /// @return Size or
    /// `__WASI_CRYPTO_ERRNO_OVERFLOW`/`__WASI_CRYPTO_ERRNO_INVALID_LENGTH`
    WasiCryptoExpect<size_t>
    decryptDetached(Span<uint8_t> Out, Span<const uint8_t> Data,
                    Span<const uint8_t> RawTag) noexcept;

  private:
    WasiCryptoExpect<size_t> encryptImpl(Span<uint8_t> Out, Span<uint8_t> Tag,
                                         Span<const uint8_t> Data) noexcept;

    WasiCryptoExpect<size_t> decryptImpl(Span<uint8_t> Out,
                                         Span<const uint8_t> Data,
                                         Span<const uint8_t> RawTag) noexcept;
    struct Inner {
      Inner(EvpCipherCtxPtr RawCtx,
            std::array<uint8_t, NonceSize> Nonce) noexcept
          : RawCtx(std::move(RawCtx)), Nonce(Nonce) {}
      EvpCipherCtxPtr RawCtx;
      std::array<uint8_t, NonceSize> Nonce;
    };
    const std::shared_ptr<Inner> Ctx;
  };

private:
  enum Mode { Unchanged = -1, Decrypt = 0, Encrypt = 1 };

  constexpr static size_t getKeySize() noexcept;

  constexpr static size_t getTagSize() noexcept;
};

using Aes128Gcm = Cipher<NID_aes_128_gcm>;
using Aes256Gcm = Cipher<NID_aes_256_gcm>;
using ChaCha20Poly1305 = Cipher<NID_chacha20_poly1305>;

} // namespace Symmetric
} // namespace WasiCrypto
} // namespace Host
} // namespace WasmEdge
