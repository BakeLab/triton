set(PROTON_TEST_UTIL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../util/")
add_compile_definitions(PROTON_TEST_UTIL_PATH="${PROTON_TEST_UTIL_PATH}")

add_triton_ut(
	NAME TraceDataIO
	SRCS ByteSpanTest.cpp DecoderTest.cpp CircularLayoutParserTest.cpp ChromeTraceWriterTest.cpp
	LIBS ProtonTraceDataIO
)

target_include_directories(TraceDataIO
PRIVATE
    "${PROTON_COMMON_DIR}/include"
    "${PROTON_SRC_DIR}/include"
)
# Vendored nlohmann/json is not warning-clean under newer toolchains; treat it
# as a system include.
target_include_directories(TraceDataIO SYSTEM PRIVATE
    "${JSON_INCLUDE_DIR}"
)
