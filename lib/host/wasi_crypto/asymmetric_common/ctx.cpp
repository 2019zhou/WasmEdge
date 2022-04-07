// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2019-2022 Second State INC

#include "host/wasi_crypto/ctx.h"
#include "host/wasi_crypto/asymmetric_common/factory.h"

namespace WasmEdge {
namespace Host {
namespace WasiCrypto {

WasiCryptoExpect<__wasi_array_output_t>
Context::publickeyExport(__wasi_publickey_t PkHandle,
                         __wasi_publickey_encoding_e_t Encoding) noexcept {
  return PublicKeyManager.get(PkHandle)
      .and_then([Encoding](auto &&Pk) {
        return AsymmetricCommon::pkExportData(Pk, Encoding);
      })
      .and_then([this](auto &&Data) {
        return ArrayOutputManager.registerManager(std::move(Data));
      });
}

WasiCryptoExpect<void>
Context::publickeyVerify(__wasi_publickey_t PkHandle) noexcept {
  return PublicKeyManager.get(PkHandle).and_then(AsymmetricCommon::pkVerify);
}

WasiCryptoExpect<void>
Context::publickeyClose(__wasi_publickey_t PkHandle) noexcept {
  return PublicKeyManager.close(PkHandle);
}

WasiCryptoExpect<__wasi_array_output_t>
Context::secretkeyExport(__wasi_secretkey_t SkHandle,
                         __wasi_secretkey_encoding_e_t Encoding) noexcept {
  return SecretKeyManager.get(SkHandle)
      .and_then([Encoding](auto &&Sk) {
        return AsymmetricCommon::skExportData(Sk, Encoding);
      })
      .and_then([this](auto &&Data) noexcept {
        return ArrayOutputManager.registerManager(std::move(Data));
      });
}

WasiCryptoExpect<void>
Context::secretkeyClose(__wasi_secretkey_t SkHandle) noexcept {
  return SecretKeyManager.close(SkHandle);
}

WasiCryptoExpect<__wasi_publickey_t>
Context::publickeyFromSecretkey(__wasi_secretkey_t SkHandle) noexcept {
  return SecretKeyManager.get(SkHandle)
      .and_then(AsymmetricCommon::skPublicKey)
      .and_then([this](auto &&Pk) noexcept {
        return PublicKeyManager.registerManager(std::move(Pk));
      });
}

WasiCryptoExpect<__wasi_array_output_t>
Context::keypairExport(__wasi_keypair_t KpHandle,
                       __wasi_keypair_encoding_e_t Encoding) noexcept {
  return KeyPairManager.get(KpHandle)
      .and_then([Encoding](auto &&Kp) noexcept {
        return AsymmetricCommon::kpExportData(Kp, Encoding);
      })
      .and_then([this](auto &&Data) noexcept {
        return ArrayOutputManager.registerManager(std::move(Data));
      });
}

WasiCryptoExpect<__wasi_publickey_t>
Context::keypairPublickey(__wasi_keypair_t KpHandle) noexcept {
  return KeyPairManager.get(KpHandle)
      .and_then(AsymmetricCommon::kpPublicKey)
      .and_then([this](auto &&Pk) noexcept {
        return PublicKeyManager.registerManager(std::move(Pk));
      });
}

WasiCryptoExpect<__wasi_secretkey_t>
Context::keypairSecretkey(__wasi_keypair_t KpHandle) noexcept {
  return KeyPairManager.get(KpHandle)
      .and_then(AsymmetricCommon::kpSecretKey)
      .and_then([this](auto &&Pk) noexcept {
        return SecretKeyManager.registerManager(std::move(Pk));
      });
}

WasiCryptoExpect<void>
Context::keypairClose(__wasi_keypair_t KpHandle) noexcept {
  return KeyPairManager.close(KpHandle);
}

WasiCryptoExpect<__wasi_keypair_t>
Context::keypairFromPkAndSk(__wasi_publickey_t PkHandle,
                            __wasi_secretkey_t SkHandle) noexcept {
  auto Pk = PublicKeyManager.get(PkHandle);
  if (!Pk) {
    return WasiCryptoUnexpect(Pk);
  }

  auto Sk = SecretKeyManager.get(SkHandle);
  if (!Sk) {
    return WasiCryptoUnexpect(Sk);
  }

  return AsymmetricCommon::kpFromPkAndSk(*Pk, *Sk).and_then(
      [this](auto &&Kp) noexcept {
        return KeyPairManager.registerManager(std::move(Kp));
      });
}

WasiCryptoExpect<__wasi_keypair_t>
Context::keypairGenerate(__wasi_algorithm_type_e_t AlgType,
                         std::string_view AlgStr,
                         __wasi_opt_options_t OptOptionsHandle) noexcept {
  return mapAndTransposeOptional(
             OptOptionsHandle,
             [this](__wasi_options_t OptionsHandle) noexcept {
               return OptionsManager.get(OptionsHandle);
             })
      .and_then([=](auto &&OptOptions) noexcept {
        return AsymmetricCommon::generateKp(AlgType, AlgStr,
                                            asOptionalRef(OptOptions));
      })
      .and_then([this](auto &&Keypair) noexcept {
        return KeyPairManager.registerManager(std::move(Keypair));
      });
}

WasiCryptoExpect<__wasi_keypair_t>
Context::keypairImport(__wasi_algorithm_type_e_t AlgType,
                       std::string_view AlgStr, Span<const uint8_t> Encoded,
                       __wasi_keypair_encoding_e_t Encoding) noexcept {
  return AsymmetricCommon::importKp(AlgType, AlgStr, Encoded, Encoding)
      .and_then([this](auto &&Kp) noexcept {
        return KeyPairManager.registerManager(std::move(Kp));
      });
}

WasiCryptoExpect<__wasi_publickey_t>
Context::publickeyImport(__wasi_algorithm_type_e_t AlgType,
                         std::string_view AlgStr, Span<const uint8_t> Encoded,
                         __wasi_publickey_encoding_e_t Encoding) noexcept {
  return AsymmetricCommon::importPk(AlgType, AlgStr, Encoded, Encoding)
      .and_then([this](auto &&Pk) noexcept {
        return PublicKeyManager.registerManager(std::move(Pk));
      });
}

WasiCryptoExpect<__wasi_secretkey_t>
Context::secretkeyImport(__wasi_algorithm_type_e_t AlgType,
                         std::string_view AlgStr, Span<const uint8_t> Encoded,
                         __wasi_secretkey_encoding_e_t Encoding) noexcept {
  return AsymmetricCommon::importSk(AlgType, AlgStr, Encoded, Encoding)
      .and_then([this](auto &&Sk) noexcept {
        return SecretKeyManager.registerManager(std::move(Sk));
      });
}

WasiCryptoExpect<__wasi_keypair_t>
Context::keypairGenerateManaged(__wasi_secrets_manager_t,
                                __wasi_algorithm_type_e_t, std::string_view,
                                __wasi_opt_options_t) noexcept {
  return WasiCryptoUnexpect(__WASI_CRYPTO_ERRNO_NOT_IMPLEMENTED);
}

WasiCryptoExpect<void> Context::keypairStoreManaged(__wasi_secrets_manager_t,
                                                    __wasi_keypair_t,
                                                    Span<uint8_t>) noexcept {
  return WasiCryptoUnexpect(__WASI_CRYPTO_ERRNO_NOT_IMPLEMENTED);
}

WasiCryptoExpect<__wasi_version_t>
Context::keypairReplaceManaged(__wasi_secrets_manager_t, __wasi_keypair_t,
                               __wasi_keypair_t) noexcept {
  return WasiCryptoUnexpect(__WASI_CRYPTO_ERRNO_NOT_IMPLEMENTED);
}

WasiCryptoExpect<std::tuple<size_t, __wasi_version_t>>
Context::keypairId(__wasi_keypair_t, Span<uint8_t>) noexcept {
  return WasiCryptoUnexpect(__WASI_CRYPTO_ERRNO_NOT_IMPLEMENTED);
}

WasiCryptoExpect<__wasi_keypair_t>
Context::keypairFromId(__wasi_secrets_manager_t, Span<const uint8_t>,
                       __wasi_version_t) noexcept {
  return WasiCryptoUnexpect(__WASI_CRYPTO_ERRNO_NOT_IMPLEMENTED);
}

} // namespace WasiCrypto
} // namespace Host
} // namespace WasmEdge
