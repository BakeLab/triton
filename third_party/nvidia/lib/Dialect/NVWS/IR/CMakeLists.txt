add_triton_library(NVWSIR
  Dialect.cpp
  Ops.cpp

  DEPENDS
  NVWSTableGen
  NVWSAttrDefsIncGen

  LINK_LIBS PUBLIC
  TritonIR
  TritonGPUIR
)
# The NVWS sources are also built outside the nvidia backend (see the root
# CMakeLists), where the backend's parent include_directories never ran.
target_include_directories(NVWSIR PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../../include
  ${CMAKE_CURRENT_BINARY_DIR}/../../../../include)
