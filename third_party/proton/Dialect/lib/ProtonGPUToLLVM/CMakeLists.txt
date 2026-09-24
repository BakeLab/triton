add_triton_library(ProtonGPUToLLVM
    AllocateProtonSharedMemory.cpp
    PatternProtonGPUOpToLLVM.cpp
    Utility.cpp

    DEPENDS
    ProtonGPUConversionPassIncGen

    LINK_LIBS PUBLIC
    ProtonIR
    ProtonGPUIR
    ProtonAnalysis
)

list(FIND TRITON_CODEGEN_BACKENDS "nvidia" _proton_nvidia_backend_index)
if(NOT _proton_nvidia_backend_index EQUAL -1)
  add_subdirectory(ProtonNvidiaGPUToLLVM)
endif()
list(FIND TRITON_CODEGEN_BACKENDS "amd" _proton_amd_backend_index)
if(NOT _proton_amd_backend_index EQUAL -1)
  add_subdirectory(ProtonAMDGPUToLLVM)
endif()
