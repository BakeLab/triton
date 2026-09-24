set(PROTON_PROFILER_SRCS
  Profiler.cpp
  GPUProfiler.cpp
  Graph.cpp
  Instrumentation/InstrumentationProfiler.cpp
  Instrumentation/Metadata.cpp
)
if(PROTON_BUILD_NVIDIA)
  list(APPEND PROTON_PROFILER_SRCS
    Cupti/CuptiPCSampling.cpp
    Cupti/CuptiProfiler.cpp
  )
endif()
if(PROTON_BUILD_AMD)
  list(APPEND PROTON_PROFILER_SRCS
    RocprofSDK/RocprofSDKPCSampling.cpp
    RocprofSDK/RocprofSDKPCSamplingSourceLocations.cpp
    RocTracer/RoctracerProfiler.cpp
    RocprofSDK/RocprofSDKProfiler.cpp
  )
endif()
add_proton_library(ProtonProfiler ${PROTON_PROFILER_SRCS})
