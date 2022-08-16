// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2019-2022 Second State INC

#include "sslwrapperfunc.h"
#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <resolv.h>
#include <string.h>
#include <unistd.h>

namespace WasmEdge {
namespace Host {

Expect<void> SetSSL::body(Runtime::Instance::MemoryInstance *MemInst,
                          uint32_t Sfd) {
  if (MemInst == nullptr) {
    return Unexpect(ErrCode::ExecutionFailed);
  }

  Env.Sfd = Sfd;

  const SSL_METHOD *Method = TLS_client_method();
  SSL_CTX *Ctx = SSL_CTX_new(Method);

  if (Ctx == nullptr) {
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
  }

  SSL *Ssl = SSL_new(Ctx);
  if (Ssl == nullptr) {
    fprintf(stderr, "[SSL WRAPPER] SSL_new() failed\n");
    exit(EXIT_FAILURE);
  }

  SSL_set_fd(Ssl, Sfd);

  return {};
}

} // namespace Host
} // namespace WasmEdge