// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2019-2022 Second State INC

#include "host/wasi_crypto/common/array_output.h"

#include <algorithm>
#include <climits>
#include <mutex>
#include <vector>

namespace WasmEdge {
namespace Host {
namespace WasiCrypto {
namespace Common {

std::tuple<size_t, bool> ArrayOutput::pull(Span<uint8_t> Buf) noexcept {
  std::scoped_lock Lock{Mutex};

  using DataPosT = decltype(Data)::difference_type;

  size_t OutputSize = std::min(Buf.size(), Data.size() - Pos);

  std::copy(
      std::next(Data.begin(),
                static_cast<DataPosT>(Pos)),
      std::next(Data.begin(),
                static_cast<DataPosT>(Pos + OutputSize)),
      Buf.begin());
  Pos += OutputSize;

  return {OutputSize, Pos + OutputSize == Data.size()};
}

} // namespace Common
} // namespace WasiCrypto
} // namespace Host
} // namespace WasmEdge