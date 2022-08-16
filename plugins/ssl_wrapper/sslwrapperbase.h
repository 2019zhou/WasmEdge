// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2019-2022 Second State INC

#pragma once

#include "common/errcode.h"
#include "sslwrapperenv.h"
#include "runtime/hostfunc.h"

namespace WasmEdge {
namespace Host {

template <typename T> class SslWrapper : public Runtime::HostFunction<T> {
public:
  SslWrapper(SslWrapperEnvironment &HostEnv)
      : Runtime::HostFunction<T>(0), Env(HostEnv) {}

protected:
  SslWrapperEnvironment &Env;
};

} // namespace Host
} // namespace WasmEdge