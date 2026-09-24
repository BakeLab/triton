#pragma once
#if TRITON_HAS_AMD_BACKEND
#include "amd/include/Dialect/TritonAMDGPU/IR/Dialect.h"
#include "amd/include/TritonAMDGPUTransforms/Passes.h"
#include "amd/lib/TritonAMDGPUToLLVM/TargetInfo.h"
#endif
#if TRITON_HAS_NVIDIA_BACKEND
#include "nvidia/include/Dialect/NVGPU/IR/Dialect.h"
#include "nvidia/include/Dialect/NVWS/IR/Dialect.h"
#include "nvidia/lib/TritonNVIDIAGPUToLLVM/TargetInfo.h"
#endif
#include "proton/Dialect/include/Conversion/ProtonGPUToLLVM/Passes.h"
#if TRITON_HAS_AMD_BACKEND
#include "proton/Dialect/include/Conversion/ProtonGPUToLLVM/ProtonAMDGPUToLLVM/Passes.h"
#endif
#if TRITON_HAS_NVIDIA_BACKEND
#include "proton/Dialect/include/Conversion/ProtonGPUToLLVM/ProtonNvidiaGPUToLLVM/Passes.h"
#endif
#include "proton/Dialect/include/Conversion/ProtonToProtonGPU/Passes.h"
#include "proton/Dialect/include/Dialect/Proton/IR/Dialect.h"
#include "proton/Dialect/include/Dialect/ProtonGPU/IR/Dialect.h"
#include "proton/Dialect/include/Dialect/ProtonGPU/Transforms/Passes.h"
#include "triton/Dialect/Gluon/Transforms/Passes.h"
#include "triton/Dialect/Triton/IR/Dialect.h"
#include "triton/Dialect/TritonGPU/IR/Dialect.h"
#include "triton/Dialect/TritonInstrument/IR/Dialect.h"
#include "triton/Dialect/TritonNvidiaGPU/IR/Dialect.h"

// Below headers will allow registration to ROCm passes
#if TRITON_HAS_AMD_BACKEND
#include "TritonAMDGPUToLLVM/Passes.h"
#include "TritonAMDGPUTransforms/Passes.h"
#include "TritonAMDGPUTransforms/TritonGPUConversion.h"
#endif

#include "triton/Dialect/Triton/Transforms/Passes.h"
#include "triton/Dialect/TritonGPU/Transforms/Passes.h"
#include "triton/Dialect/TritonInstrument/Transforms/Passes.h"
#include "triton/Dialect/TritonNvidiaGPU/Transforms/Passes.h"

#if TRITON_HAS_NVIDIA_BACKEND
#include "nvidia/hopper/include/Transforms/Passes.h"
#include "nvidia/include/Dialect/NVWS/Transforms/Passes.h"
#include "nvidia/include/NVGPUToLLVM/Passes.h"
#include "nvidia/include/TritonNVIDIAGPUToLLVM/Passes.h"
#endif
#include "triton/Conversion/TritonGPUToLLVM/Passes.h"
#include "triton/Conversion/TritonToTritonGPU/Passes.h"
#include "triton/Target/LLVMIR/Passes.h"

#include "mlir/Dialect/LLVMIR/NVVMDialect.h"
#include "mlir/Dialect/LLVMIR/ROCDLDialect.h"
#include "mlir/Dialect/LLVMIR/Transforms/InlinerInterfaceImpl.h"
#include "mlir/InitAllPasses.h"

#include "mlir/Conversion/ArithToLLVM/ArithToLLVM.h"
#include "mlir/Conversion/ControlFlowToLLVM/ControlFlowToLLVM.h"
#include "mlir/Conversion/MathToLLVM/MathToLLVM.h"
#include "mlir/Conversion/NVVMToLLVM/NVVMToLLVM.h"
#include "mlir/Conversion/UBToLLVM/UBToLLVM.h"

#include "triton/Tools/PluginUtils.h"
#include "triton/Tools/Sys/GetEnv.h"

namespace mlir {
namespace test {
void registerTestAliasPass();
void registerTestAlignmentPass();
void registerTestAllocationPass();
void registerTestBufferRegionPass();
void registerTestLoopPeelingPass();
#if TRITON_HAS_NVIDIA_BACKEND
void registerTestMembarPass();
#endif
#if TRITON_HAS_AMD_BACKEND
void registerAMDTestAlignmentPass();
void registerTestAMDGPUMembarPass();
void registerTestTritonAMDGPURangeAnalysis();
#endif
namespace proton {
void registerTestScopeIdAllocationPass();
} // namespace proton
} // namespace test
} // namespace mlir

inline void registerTritonDialects(mlir::DialectRegistry &registry) {
#if TRITON_HAS_NVIDIA_BACKEND
  mlir::triton::NVIDIA::registerTargetInfo();
#endif
#if TRITON_HAS_AMD_BACKEND
  mlir::triton::AMD::registerTargetInfo();
#endif
  mlir::registerAllPasses();
  mlir::triton::registerTritonPasses();
  mlir::triton::gpu::registerTritonGPUPasses();
  mlir::triton::nvidia_gpu::registerTritonNvidiaGPUPasses();
  mlir::triton::nvidia_gpu::registerConSanNVIDIAHooks();
  mlir::triton::instrument::registerTritonInstrumentPasses();
  mlir::triton::gluon::registerGluonPasses();
  mlir::test::registerTestAliasPass();
  mlir::test::registerTestAlignmentPass();
  mlir::test::registerTestAllocationPass();
  mlir::test::registerTestBufferRegionPass();
#if TRITON_HAS_NVIDIA_BACKEND
  mlir::test::registerTestMembarPass();
#endif
  mlir::test::registerTestLoopPeelingPass();
#if TRITON_HAS_AMD_BACKEND
  mlir::test::registerAMDTestAlignmentPass();
  mlir::test::registerTestAMDGPUMembarPass();
  mlir::test::registerTestTritonAMDGPURangeAnalysis();
#endif
  mlir::triton::registerConvertTritonToTritonGPUPass();
  mlir::triton::registerRelayoutTritonGPUPass();
  mlir::triton::gpu::registerAllocateSharedMemoryPass();
  mlir::triton::gpu::registerTritonGPUAllocateWarpGroups();
  mlir::triton::gpu::registerTritonGPUGlobalScratchAllocationPass();
  mlir::triton::gpu::registerCanonicalizeLLVMIR();
  mlir::triton::registerConvertWarpSpecializeToLLVM();
  mlir::triton::registerInitializeWSClusterBarriers();
  mlir::triton::registerTritonNvidiaGPUMembar();
  mlir::triton::registerConvertTritonGPUToLLVMPass();
#if TRITON_HAS_NVIDIA_BACKEND
  mlir::triton::registerConvertNVGPUToLLVMPass();
  mlir::triton::registerAllocateSharedMemoryNvPass();
  mlir::triton::registerSetMinimumSharedMemoryPass();
#endif
  mlir::registerLLVMDIScope();
  mlir::LLVM::registerInlinerInterface(registry);
  mlir::registerLLVMDILocalVariable();

#if TRITON_HAS_AMD_BACKEND
  // TritonAMDGPUToLLVM passes
  mlir::triton::registerAllocateAMDGPUSharedMemory();
  mlir::triton::registerTritonAMDGPUMembar();
  mlir::triton::registerTritonAMDGPUConvertWarpSpecializeToLLVM();
  mlir::triton::registerConvertTritonAMDGPUToLLVM();
  mlir::triton::registerConvertBuiltinFuncToLLVM();
  mlir::triton::registerConvertWarpPipeline();
#endif

  mlir::ub::registerConvertUBToLLVMInterface(registry);
  mlir::registerConvertNVVMToLLVMInterface(registry);
  mlir::registerConvertMathToLLVMInterface(registry);
  mlir::cf::registerConvertControlFlowToLLVMInterface(registry);
  mlir::arith::registerConvertArithToLLVMInterface(registry);

#if TRITON_HAS_AMD_BACKEND
  // TritonAMDGPUTransforms passes
  mlir::registerTritonAMDGPUAccelerateMatmul();
  mlir::registerTritonAMDGPUOptimizeDescriptorEncoding();
  mlir::registerTritonAMDGPUOptimizeEpilogue();
  mlir::registerTritonAMDGPUHoistLayoutConversions();
  mlir::registerTritonAMDGPUSinkLayoutConversions();
  mlir::registerTritonAMDGPUPrepareIfCombining();
  mlir::registerTritonAMDGPUMoveUpPrologueLoads();
  mlir::registerTritonAMDGPUBlockPingpong();
  mlir::registerTritonAMDGPUPipeline();
  mlir::registerTritonAMDGPUScheduleLoops();
  mlir::registerTritonAMDGPUCanonicalizePointers();
  mlir::registerTritonAMDGPUConvertToBufferOps();
  mlir::registerTritonAMDGPUConvertToTensorOps();
  mlir::registerTritonAMDGPUOptimizeBufferOpPtr();
  mlir::registerTritonAMDGPUInThreadTranspose();
  mlir::registerTritonAMDGPUCoalesceAsyncCopy();
  mlir::registerTritonAMDGPUUpdateAsyncWaitCount();
  mlir::registerTritonAMDGPUWarpPipeline();
  mlir::registerTritonAMDFoldTrueCmpI();
  mlir::registerTritonAMDGPUFpSanitizer();
  mlir::triton::amdgpu::registerTritonAMDGPUOptimizeDotOperands();
  mlir::registerConSanAMDHooks();
#endif

#if TRITON_HAS_NVIDIA_BACKEND
  // NVWS passes
  mlir::triton::registerNVWSTransformsPasses();

  // NVGPU transform passes
  mlir::registerNVHopperTransformsPasses();
#endif

  // Proton passes
  mlir::test::proton::registerTestScopeIdAllocationPass();
  mlir::triton::proton::registerConvertProtonToProtonGPU();
#if TRITON_HAS_NVIDIA_BACKEND
  mlir::triton::proton::gpu::registerConvertProtonNvidiaGPUToLLVM();
#endif
#if TRITON_HAS_AMD_BACKEND
  mlir::triton::proton::gpu::registerConvertProtonAMDGPUToLLVM();
#endif
  mlir::triton::proton::gpu::registerAllocateProtonSharedMemoryPass();
  mlir::triton::proton::gpu::registerScheduleBufferStorePass();
#if TRITON_HAS_AMD_BACKEND
  // The pass inserts ROCDL sched barriers and is implemented in the
  // AMD-specific proton lowering library.
  mlir::triton::proton::gpu::registerAddSchedBarriersPass();
#endif

  // Register plugin passes and dialects.
  for (const auto &plugin : mlir::triton::plugin::loadPlugins()) {
    plugin.registerPasses();
    plugin.registerDialects(registry);
  }

  registry.insert<
      mlir::triton::TritonDialect, mlir::cf::ControlFlowDialect,
      mlir::triton::nvidia_gpu::TritonNvidiaGPUDialect,
      mlir::triton::gpu::TritonGPUDialect,
      mlir::triton::instrument::TritonInstrumentDialect,
      mlir::math::MathDialect, mlir::arith::ArithDialect, mlir::scf::SCFDialect,
      mlir::gpu::GPUDialect, mlir::LLVM::LLVMDialect, mlir::NVVM::NVVMDialect,
#if TRITON_HAS_NVIDIA_BACKEND
      mlir::triton::nvgpu::NVGPUDialect, mlir::triton::nvws::NVWSDialect,
#endif
      mlir::triton::proton::ProtonDialect,
      mlir::triton::proton::gpu::ProtonGPUDialect, mlir::ROCDL::ROCDLDialect,
      mlir::triton::gluon::GluonDialect>();
}
