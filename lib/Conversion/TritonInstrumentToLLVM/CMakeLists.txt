# GSan lowering emits NVIDIA PTX; the GSan runtime lives in the nvidia
# backend plugin, so the file is only needed for nvidia builds.
list(FIND TRITON_CODEGEN_BACKENDS "nvidia" _instr_nvidia_backend_index)
set(_triton_gsan_srcs GSanToLLVM.cpp)
set(_triton_nvgpu_link_libs NVGPUIR)
if(_instr_nvidia_backend_index EQUAL -1)
  set(_triton_gsan_srcs "")
  set(_triton_nvgpu_link_libs "")
endif()

add_triton_library(TritonInstrumentToLLVM
    InstrumentationToLLVM.cpp
    FpSanToLLVM.cpp
    ${_triton_gsan_srcs}

    LINK_LIBS PUBLIC
    MLIRIR
    MLIRPass
    TritonIR
    TritonGPUIR
    TritonInstrumentIR
    TritonNvidiaGPUIR
    ${_triton_nvgpu_link_libs}
)
