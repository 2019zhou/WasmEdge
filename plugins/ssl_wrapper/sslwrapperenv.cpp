// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2019-2022 Second State INC

#include "sslwrapperenv.h"
#include "sslwrappermodule.h"

namespace WasmEdge {
namespace Host {

namespace {

Runtime::Instance::ModuleInstance *create(void) noexcept {
  return new SslWrapperModule;
}

Plugin::Plugin::PluginDescriptor Descriptor{
    .Name = "ssl_wrapper",
    .Description = "",
    .APIVersion = Plugin::Plugin::CurrentAPIVersion,
    .Version = {0, 10, 1, 0},
    .ModuleCount = 1,
    .ModuleDescriptions =
        (Plugin::PluginModule::ModuleDescriptor[]){
            {
                .Name = "ssl_wrapper",
                .Description = "",
                .Create = create,
            },
        },
    .AddOptions = nullptr,
};

} // namespace

Plugin::PluginRegister SslWrapperEnvironment::Register(&Descriptor);

} // namespace Host
} // namespace WasmEdge
