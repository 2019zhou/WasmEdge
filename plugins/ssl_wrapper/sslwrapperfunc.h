// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2019-2022 Second State INC

#pragma once

#include "common/defines.h"
#include "sslwrapperbase.h"

#include <stdio.h>

namespace WasmEdge {
namespace Host {

class SetSSL : public SslWrapper<SetSSL> {
public:
  SetSSL(SslWrapperEnvironment &HostEnv) : SslWrapper(HostEnv) {}
  Expect<void> body(Runtime::Instance::MemoryInstance *, uint32_t Sfd);
};

} // namespace Host
} // namespace WasmEdge