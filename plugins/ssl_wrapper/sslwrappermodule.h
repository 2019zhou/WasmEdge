// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2019-2022 Second State INC

#pragma once

#include "sslwrapperenv.h"
#include "runtime/instance/module.h"
#include <cstdint>

namespace WasmEdge {
namespace Host {

class SslWrapperModule : public Runtime::Instance::ModuleInstance {
public:
  SslWrapperModule();

  SslWrapperEnvironment &getEnv() { return Env; }

private:
  SslWrapperEnvironment Env;
};

} // namespace Host
} // namespace WasmEdge