# Vendor-specific GPU-to-LLVM transforms are only linked when the
# corresponding backend is part of this build.
list(FIND TRITON_CODEGEN_BACKENDS "nvidia" _gpu2llvm_nvidia_index)
list(FIND TRITON_CODEGEN_BACKENDS "amd" _gpu2llvm_amd_index)
set(_gpu2llvm_nvidia_libs MLIRGPUToNVVMTransforms NVGPUIR)
set(_gpu2llvm_amd_libs MLIRGPUToROCDLTransforms)
if(_gpu2llvm_nvidia_index EQUAL -1)
  set(_gpu2llvm_nvidia_libs "")
endif()
if(_gpu2llvm_amd_index EQUAL -1)
  set(_gpu2llvm_amd_libs "")
endif()

add_triton_library(TritonGPUToLLVM
    DotOpToLLVM/FMA.cpp
    DotOpToLLVM/FMADotUtility.cpp
    AllocateSharedMemory.cpp
    AllocateSharedMemoryUtility.cpp
    AllocateWarpGroups.cpp
    AssertOpToLLVM.cpp
    CanonicalizeLLVMIR.cpp
    ControlFlowOpToLLVM.cpp
    ConvertLayoutOpToLLVM.cpp
    ElementwiseOpToLLVM.cpp
    Fp4ToFpOpToLLVMBase.cpp
    FuncOpToLLVM.cpp
    GatherOpToLLVM.cpp
    GlobalScratchMemoryAllocation.cpp
    HistogramOpToLLVM.cpp
    MakeRangeOpToLLVM.cpp
    MemoryOpToLLVM.cpp
    PrintOpToLLVM.cpp
    ReduceOpToLLVM.cpp
    ScanOpToLLVM.cpp
    SPMDOpToLLVM.cpp
    TypeConverter.cpp
    Utility.cpp
    ViewOpToLLVM.cpp
    WarpSpecializeUtility.cpp

    DEPENDS
    TritonGPUConversionPassIncGen

    LINK_LIBS PUBLIC
    MLIRIR
    MLIRPass
    MLIRGPUDialect
    MLIRGPUTransforms
    ${_gpu2llvm_nvidia_libs}
    ${_gpu2llvm_amd_libs}
    TritonAnalysis
    TritonIR
    TritonGPUIR
    TritonGPUTransforms
    TritonNvidiaGPUTransforms
)
