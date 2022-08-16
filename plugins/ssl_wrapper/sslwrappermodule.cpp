// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2019-2022 Second State INC

#include "sslwrappermodule.h"
#include "sslwrapperfunc.h"

namespace WasmEdge {
namespace Host {

/// Register your functions in module.
SslWrapperModule::SslWrapperModule() : ModuleInstance("sslwrapper") {
  addHostFunc("set_ssl", std::make_unique<SetSSL>(Env));
}

} // namespace Host
} // namespace WasmEdge