# TestMembar exercises the NVIDIA membar analysis; only build it when the
# nvidia backend is part of this build.
list(FIND TRITON_CODEGEN_BACKENDS "nvidia" _test_analysis_nvidia_index)
set(_triton_test_membar_srcs TestMembar.cpp)
if(_test_analysis_nvidia_index EQUAL -1)
  set(_triton_test_membar_srcs "")
endif()

add_library(TritonTestAnalysis
  TestAlias.cpp
  TestAxisInfo.cpp
  TestAllocation.cpp
  TestBufferRegion.cpp
  ${_triton_test_membar_srcs}
)
target_link_libraries(TritonTestAnalysis PUBLIC MLIRPass TritonAnalysis TritonNvidiaGPUTransforms)
target_compile_options(TritonTestAnalysis PRIVATE ${TRITON_DISABLE_EH_RTTI_FLAGS})
