# The NVWS dialect libraries back the warp-specialization passes and are
# only linked when the nvidia backend is part of this build.
list(FIND TRITON_CODEGEN_BACKENDS "nvidia" _tg_transforms_nvidia_index)
set(_tg_transforms_nvidia_libs NVWSIR NVWSTransforms)
if(_tg_transforms_nvidia_index EQUAL -1)
  set(_tg_transforms_nvidia_libs "")
endif()

add_triton_library(TritonGPUTransforms
  AccelerateMatmul.cpp
  Coalesce.cpp
  F32DotTC.cpp
  FuseNestedLoops.cpp
  CombineTensorSelectAndIf.cpp
  DecomposeScaledBlocked.cpp
  DescriptorMemoryLayouts.cpp
  HoistTMEMAlloc.cpp
  ReduceDataDuplication.cpp
  OptimizeAccumulatorInit.cpp
  OptimizeDotOperands.cpp
  OptimizeThreadLocality.cpp
  Pipeliner/AssignLatencies.cpp
  Pipeliner/LowerLoops.cpp
  Pipeliner/MMAv5PipelineUtility.cpp
  Pipeliner/ScheduleLoops.cpp
  Pipeliner/WGMMAPipeline.cpp
  Pipeliner/PipelineExpander.cpp
  Pipeliner/TestPipelineLowerLoop.cpp
  Pipeliner/SoftwarePipeliner.cpp
  Pipeliner/TMAStoresPipeline.cpp
  Pipeliner/MMAv5PipelineUtility.cpp
  Pipeliner/PipeliningUtility.cpp
  Pipeliner/Schedule.cpp
  Prefetch.cpp
  RemoveLayoutConversions.cpp
  ReorderInstructions.cpp
  CoalesceAsyncCopy.cpp
  TargetInfo.cpp
  Utility.cpp
  CoalesceUtils.cpp
  LayoutPropagationUtility.cpp
  WarpSpecialization/AutomaticWarpSpecialization.cpp
  WarpSpecialization/NormalizeWSWhileLoops.cpp
  WarpSpecialization/Partition.cpp
  WarpSpecialization/OptimizePartitionWarps.cpp
  WarpSpecialization/PartitionBuilder.cpp
  WarpSpecialization/PartitionLoops.cpp
  WarpSpecialization/PartitionScheduling.cpp
  WarpSpecialization/PartitionSchedulingUtility.cpp

  DEPENDS
  TritonGPUTransformsIncGen

  LINK_LIBS PUBLIC
  MLIRTransforms
  MLIRTransformUtils
  TritonAnalysis
  TritonIR
  TritonTransforms
  TritonGPUIR
  TritonNvidiaGPUIR
  ${_tg_transforms_nvidia_libs}
  TritonToTritonGPU
  TritonInstrumentIR
  MLIRTransformUtils
)
