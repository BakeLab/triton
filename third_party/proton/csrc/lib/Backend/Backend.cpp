#include "Backend/Backend.h"
#include "Profiler/Instrumentation/InstrumentationProfiler.h"
#if PROTON_HAS_NVIDIA_BACKEND
#include "Driver/GPU/CudaApi.h"
#include "Profiler/Cupti/CuptiProfiler.h"
#include "Runtime/CudaRuntime.h"
#endif
#if PROTON_HAS_AMD_BACKEND
#include "Driver/GPU/HipApi.h"
#include "Profiler/RocprofSDK/RocprofSDKProfiler.h"
#include "Profiler/Roctracer/RoctracerProfiler.h"
#include "Runtime/HipRuntime.h"
#endif
#include <vector>

namespace proton {

const std::vector<ProfilerRegistration> getProfilerRegistrations() {
  std::vector<ProfilerRegistration> registeredProfilers = {
#if PROTON_HAS_NVIDIA_BACKEND
      {"cupti", "cuda", []() { return &CuptiProfiler::instance(); }},
#endif
#if PROTON_HAS_AMD_BACKEND
      {"rocprofiler", "hip", []() { return &RocprofSDKProfiler::instance(); }},
      {"roctracer", {}, []() { return &RoctracerProfiler::instance(); }},
#endif
      {"instrumentation",
       {},
       []() { return &InstrumentationProfiler::instance(); }},
  };
  for (const auto &backend : getBackendRegistrations()) {
    const auto &profiler = backend.getProfiler();
    if (profiler)
      registeredProfilers.push_back(*profiler);
  }
  return registeredProfilers;
}

const std::vector<DeviceRegistration> getDeviceRegistrations() {
  std::vector<DeviceRegistration> registeredDevices = {
#if PROTON_HAS_NVIDIA_BACKEND
      {"CUDA", DeviceType::CUDA,
       [](uint64_t index) { return cuda::getDevice(index); }},
#endif
#if PROTON_HAS_AMD_BACKEND
      {"HIP", DeviceType::HIP,
       [](uint64_t index) { return hip::getDevice(index); }},
#endif
  };
  for (const auto &backend : getBackendRegistrations()) {
    const auto &device = backend.getDevice();
    if (device)
      registeredDevices.push_back(*device);
  }
  return registeredDevices;
}

const std::vector<RuntimeRegistration> getRuntimeRegistrations() {
  std::vector<RuntimeRegistration> registeredRuntimes = {
#if PROTON_HAS_NVIDIA_BACKEND
      {"CUDA", []() { return &CudaRuntime::instance(); }},
#endif
#if PROTON_HAS_AMD_BACKEND
      {"HIP", []() { return &HipRuntime::instance(); }},
#endif
  };
  for (const auto &backend : getBackendRegistrations()) {
    const auto &runtime = backend.getRuntime();
    if (runtime) {
      registeredRuntimes.push_back(*runtime);
    }
  }
  return registeredRuntimes;
}

const std::vector<std::string> getRegisteredProfilerNames() {
  const auto profilers = getProfilerRegistrations();
  std::vector<std::string> profilerNames(profilers.size());
  std::transform(
      profilers.begin(), profilers.end(), profilerNames.begin(),
      [](const ProfilerRegistration &entry) { return entry.getName(); });
  return profilerNames;
}

const std::optional<std::string>
getProfilerForTritonBackend(const std::string &tritonBackend) {
  const auto profilers = getProfilerRegistrations();
  auto itr = std::find_if(profilers.begin(), profilers.end(),
                          [&](const ProfilerRegistration &entry) {
                            return proton::toLower(tritonBackend) ==
                                   proton::toLower(
                                       entry.getTritonBackend().value_or(""));
                          });
  if (itr == profilers.end()) {
    return {};
  }
  return itr->getName();
}

} // namespace proton
