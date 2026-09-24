set(PROTON_DRIVER_SRCS
  Device.cpp
)
if(PROTON_BUILD_NVIDIA)
  list(APPEND PROTON_DRIVER_SRCS
    GPU/CudaApi.cpp
    GPU/CuptiApi.cpp
    GPU/NvtxApi.cpp
  )
endif()
if(PROTON_BUILD_AMD)
  list(APPEND PROTON_DRIVER_SRCS
    GPU/HipApi.cpp
    GPU/HsaApi.cpp
    GPU/RoctracerApi.cpp
    GPU/RocprofApi.cpp
  )
endif()
add_proton_library(ProtonDriver ${PROTON_DRIVER_SRCS})
