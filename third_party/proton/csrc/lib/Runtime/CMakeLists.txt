set(PROTON_RUNTIME_SRCS
)
if(PROTON_BUILD_NVIDIA)
  list(APPEND PROTON_RUNTIME_SRCS
    CudaRuntime.cpp
  )
endif()
if(PROTON_BUILD_AMD)
  list(APPEND PROTON_RUNTIME_SRCS
    HipRuntime.cpp
  )
endif()
add_proton_library(ProtonRuntime ${PROTON_RUNTIME_SRCS})
