project(Proton LANGUAGES CXX)

include(Backend.cmake)
include(CheckCXXSourceCompiles)

set(PROTON_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/csrc")
set(PROTON_COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/common")

# ============ Vendor gating =============
# Proton builds vendor-specific profiling paths (CUPTI for NVIDIA,
# rocprofiler-sdk/roctracer for AMD) alongside the portable instrumentation
# profiler.  When the enclosing Triton build selects a strict subset of
# codegen backends, restrict the vendor paths to the selected vendors.
# Standalone proton builds (no TRITON_CODEGEN_BACKENDS) keep everything.
set(PROTON_BUILD_NVIDIA ON)
set(PROTON_BUILD_AMD ON)
if(DEFINED TRITON_CODEGEN_BACKENDS AND NOT TRITON_CODEGEN_BACKENDS STREQUAL "")
  list(FIND TRITON_CODEGEN_BACKENDS "nvidia" PROTON_NVIDIA_BACKEND_INDEX)
  list(FIND TRITON_CODEGEN_BACKENDS "amd" PROTON_AMD_BACKEND_INDEX)
  if(PROTON_NVIDIA_BACKEND_INDEX EQUAL -1)
    set(PROTON_BUILD_NVIDIA OFF)
  endif()
  if(PROTON_AMD_BACKEND_INDEX EQUAL -1)
    set(PROTON_BUILD_AMD OFF)
  endif()
endif()

# ============ Check for includes =============
if(PROTON_BUILD_NVIDIA AND NOT CUPTI_INCLUDE_DIR)
  message(FATAL_ERROR "CUPTI include directory not defined")
endif()
if(PROTON_BUILD_AMD AND NOT ROCM_INCLUDE_DIR)
  message(FATAL_ERROR "ROCM include directory not defined")
endif()
if(NOT JSON_INCLUDE_DIR)
  message(FATAL_ERROR "JSON include directory not defined")
endif()
if(NOT ROCPROFILER_SDK_INCLUDE_DIR)
  set(ROCPROFILER_SDK_INCLUDE_DIR "${ROCM_INCLUDE_DIR}")
endif()

unset(PROTON_ROCPROFILER_SDK_HAS_HIP_GRAPH CACHE)
if(PROTON_BUILD_AMD)
  set(_proton_saved_cmake_required_includes "${CMAKE_REQUIRED_INCLUDES}")
  set(CMAKE_REQUIRED_INCLUDES "${ROCPROFILER_SDK_INCLUDE_DIR};${ROCM_INCLUDE_DIR}")
  check_cxx_source_compiles([=[
    #include <rocprofiler-sdk/fwd.h>
    int main() {
      auto op = ROCPROFILER_HIP_GRAPH_OPERATION_EXEC_LAUNCH;
      return static_cast<int>(op);
    }
  ]=] PROTON_ROCPROFILER_SDK_HAS_HIP_GRAPH)
  set(CMAKE_REQUIRED_INCLUDES "${_proton_saved_cmake_required_includes}")
endif()

if(PROTON_ROCPROFILER_SDK_HAS_HIP_GRAPH)
  message(STATUS "Proton ROCProfiler-SDK HIP graph tracing: enabled")
else()
  message(STATUS "Proton ROCProfiler-SDK HIP graph tracing: disabled")
endif()

unset(PROTON_ROCPROFILER_SDK_HAS_PC_SAMPLING CACHE)
set(PROTON_ROCPROFILER_SDK_HAS_PC_SAMPLING OFF)
if(PROTON_BUILD_AMD AND EXISTS "${ROCPROFILER_SDK_INCLUDE_DIR}/rocprofiler-sdk/pc_sampling.h")
  set(PROTON_ROCPROFILER_SDK_HAS_PC_SAMPLING ON)
endif()

if(PROTON_ROCPROFILER_SDK_HAS_PC_SAMPLING)
  message(STATUS "Proton ROCProfiler-SDK PC sampling: enabled")
else()
  message(STATUS "Proton ROCProfiler-SDK PC sampling: disabled")
endif()

if(PROTON_BUILD_AMD)
  get_filename_component(_PROTON_ROCM_ROOT "${ROCM_INCLUDE_DIR}" DIRECTORY)

  find_library(PROTON_AMD_COMGR_LIBRARY
    NAMES amd_comgr
    HINTS "${_PROTON_ROCM_ROOT}/lib"
    NO_DEFAULT_PATH)
  find_library(PROTON_DW_LIBRARY
    NAMES dw
    HINTS "${_PROTON_ROCM_ROOT}/lib/rocm_sysdeps/lib")
  find_path(PROTON_DWARF_INCLUDE_DIR
    NAMES dwarf.h
    HINTS "${_PROTON_ROCM_ROOT}/lib/rocm_sysdeps/include")
endif()

unset(PROTON_ROCPROFILER_SDK_HAS_CODEOBJ_ADDRESS_TRANSLATE CACHE)
set(PROTON_ROCPROFILER_SDK_HAS_CODEOBJ_ADDRESS_TRANSLATE OFF)
if(PROTON_BUILD_AMD AND PROTON_ROCPROFILER_SDK_HAS_PC_SAMPLING AND
   EXISTS
     "${ROCPROFILER_SDK_INCLUDE_DIR}/rocprofiler-sdk/cxx/codeobj/code_printing.hpp"
   AND
   PROTON_AMD_COMGR_LIBRARY AND PROTON_DW_LIBRARY AND
   PROTON_DWARF_INCLUDE_DIR)
  set(PROTON_ROCPROFILER_SDK_HAS_CODEOBJ_ADDRESS_TRANSLATE ON)
endif()

# ============ Dependencies =============
find_package(Python3 REQUIRED Interpreter Development.Module)
if(NOT TARGET nanobind-static)
  if(NOT TARGET Python::Module)
    add_library(Python::Module ALIAS Python3::Module)
    set(Python_EXECUTABLE "${Python3_EXECUTABLE}")
    set(Python_INCLUDE_DIRS "${Python3_INCLUDE_DIRS}")
    set(Python_SOABI "${Python3_SOABI}")
    set(Python_VERSION "${Python3_VERSION}")
    set(Python_INTERPRETER_ID "${Python3_INTERPRETER_ID}")
  endif()

  find_package(nanobind CONFIG REQUIRED HINTS "${Python3_SITELIB}")
  nanobind_build_library(nanobind-static)
  target_link_libraries(nanobind-static PRIVATE Python3::Module)
endif()

# ============ Define a GLOBAL property to store object-libraries ============
set_property(GLOBAL PROPERTY PROTON_LIBS "")

# ============ Define a function to create object libraries ============
function(add_proton_library name)
  add_library(${name} OBJECT ${ARGN})

  target_link_libraries(${name} PRIVATE Python3::Module nanobind-static)

  # Use system to skip warnings caused by legacy clang compilers
  if(PROTON_BUILD_AMD)
    target_include_directories(${name}
      SYSTEM PRIVATE
        ${ROCPROFILER_SDK_INCLUDE_DIR}
        "${ROCM_INCLUDE_DIR}"
    )
  endif()

  target_include_directories(${name}
    PRIVATE
      "${PROTON_COMMON_DIR}/include"
      "${PROTON_SRC_DIR}/include"
  )
  # Vendored nlohmann/json is not warning-clean under newer toolchains
  # (e.g. GCC 16 deprecates std::is_trivial); treat it as a system include.
  target_include_directories(${name} SYSTEM PRIVATE
      "${JSON_INCLUDE_DIR}"
  )
  if(PROTON_BUILD_NVIDIA)
    target_include_directories(${name} PRIVATE "${CUPTI_INCLUDE_DIR}")
  endif()
  if(PROTON_ROCPROFILER_SDK_HAS_CODEOBJ_ADDRESS_TRANSLATE)
    target_include_directories(${name} SYSTEM PRIVATE
      "${PROTON_DWARF_INCLUDE_DIR}")
  endif()

  if(PROTON_BUILD_NVIDIA)
    target_compile_definitions(${name} PRIVATE PROTON_HAS_NVIDIA_BACKEND=1)
  else()
    target_compile_definitions(${name} PRIVATE PROTON_HAS_NVIDIA_BACKEND=0)
  endif()
  if(PROTON_BUILD_AMD)
    target_compile_definitions(${name} PRIVATE PROTON_HAS_AMD_BACKEND=1)
  else()
    target_compile_definitions(${name} PRIVATE PROTON_HAS_AMD_BACKEND=0)
  endif()

  # If HIP is AMD-based
  if(PROTON_BUILD_AMD)
    target_compile_definitions(${name} PRIVATE __HIP_PLATFORM_AMD__)
  endif()
  if(PROTON_ROCPROFILER_SDK_HAS_HIP_GRAPH)
    target_compile_definitions(${name} PRIVATE
      PROTON_ROCPROFILER_SDK_HAS_HIP_GRAPH=1)
  else()
    target_compile_definitions(${name} PRIVATE
      PROTON_ROCPROFILER_SDK_HAS_HIP_GRAPH=0)
  endif()
  if(PROTON_ROCPROFILER_SDK_HAS_PC_SAMPLING)
    target_compile_definitions(${name} PRIVATE
      PROTON_ROCPROFILER_SDK_HAS_PC_SAMPLING=1)
  else()
    target_compile_definitions(${name} PRIVATE
      PROTON_ROCPROFILER_SDK_HAS_PC_SAMPLING=0)
  endif()
  if(PROTON_ROCPROFILER_SDK_HAS_CODEOBJ_ADDRESS_TRANSLATE)
    target_compile_definitions(${name} PRIVATE
      PROTON_ROCPROFILER_SDK_HAS_CODEOBJ_ADDRESS_TRANSLATE=1)
  else()
    target_compile_definitions(${name} PRIVATE
      PROTON_ROCPROFILER_SDK_HAS_CODEOBJ_ADDRESS_TRANSLATE=0)
  endif()

  # Append this library name to the GLOBAL property "PROTON_LIBS"
  set_property(GLOBAL APPEND PROPERTY PROTON_LIBS ${name})
endfunction()

# ============ Add subdirectory with actual code that calls add_proton_library ============
add_subdirectory("${PROTON_COMMON_DIR}")
add_subdirectory("${PROTON_SRC_DIR}")

# ============ Add subdirectory with proton tests ============
add_subdirectory(test)

# ============ Discover any Triton backends registering Proton backends ============
discover_proton_backends()

# ============ Codegen the templates allowing for Proton backend registration ============
codegen_proton_backend_templates()

# ============ Possibly handle macOS specifics ============
if(APPLE)
  set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
  # Other platforms build with -flto, but we found that this adds significant overhead to our macos CI without providing a major benefit.
  set(PROTON_PYTHON_LDFLAGS "-undefined dynamic_lookup")
endif()

# ============ Collect all object libraries from property and build final shared lib ============
get_property(_proton_obj_libs GLOBAL PROPERTY PROTON_LIBS)

if(NOT _proton_obj_libs)
  message(WARNING "No object libraries were defined in 'PROTON_LIBS'!")
endif()

set(_proton_obj_sources "")
foreach(_lib IN LISTS _proton_obj_libs)
  list(APPEND _proton_obj_sources $<TARGET_OBJECTS:${_lib}>)
  message(STATUS "Collecting object files from ${_lib}")
endforeach()

add_library(proton SHARED ${_proton_obj_sources})

target_link_libraries(proton PRIVATE Python3::Module nanobind-static)
if(PROTON_ROCPROFILER_SDK_HAS_CODEOBJ_ADDRESS_TRANSLATE)
  target_link_libraries(proton PRIVATE
    "${PROTON_AMD_COMGR_LIBRARY}"
    "${PROTON_DW_LIBRARY}")
endif()
# Apply any macOS linker flags or extra link options
if(PROTON_PYTHON_LDFLAGS)
  target_link_options(proton PRIVATE ${PROTON_PYTHON_LDFLAGS})
endif()

# ============ Link any external libs required by Proton backends ============
link_proton_backend_external_libs()
