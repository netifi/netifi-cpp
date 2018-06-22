cmake_minimum_required(VERSION 3.2)

# The PROTEUS_CC CMake variable specifies the C compiler, e.g. gcc-4.9.
# The C++ compiler name is obtained by replacing "gcc" with "g++" and "clang"
# with "clang++"". If PROTEUS_CC is not given, the compiler is detected
# automatically.
if (PROTEUS_CC)
  set(ENV{CC} ${PROTEUS_CC})
  if (${PROTEUS_CC} MATCHES clang)
    string(REPLACE clang clang++ CXX ${PROTEUS_CC})
  else ()
    string(REPLACE gcc g++ CXX ${PROTEUS_CC})
  endif ()
  set(ENV{CXX} ${CXX})
endif ()

project(Proteus)

# CMake modules.
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

# Joins arguments and stores the result in ${var}.
function(join var)
  unset(result)
  foreach (arg ${ARGN})
    if (DEFINED result)
      set(result "${result}${arg}")
    else ()
      set(result "${arg}")
    endif ()
  endforeach ()
  set(${var} "${result}" PARENT_SCOPE)
endfunction()

# Generate compilation database for use by YouCompleteMe.
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# make sure to bail on in-source builds for cleanliness
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory)"
  "and run CMake from there. You may need to remove CMakeCache.txt.")
endif()

# default built type is Debug
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWER)

if (BUILD_TYPE_LOWER MATCHES "debug")
  add_definitions(-DDEBUG)
endif ()

# Enable ASAN by default on debug macOS builds.
#if (APPLE)
#  set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
#  if ("${BUILD_TYPE_LOWER}" MATCHES "debug")
#    if (${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
#      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,integer -fno-sanitize=unsigned-integer-overflow")
#    elseif (${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
#      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
#    else ()
#        message(FATAL_ERROR "Unsupported compiler on macOS")
#    endif()
#  endif()
#endif()

# disable coverage mode by default
option(PROTEUS_BUILD_WITH_COVERAGE "Build with --coverage (gcov)" OFF)

# Add compiler-specific options.
if (CMAKE_COMPILER_IS_GNUCXX)
  if (PROTEUS_ASAN)
    set(ASAN_FLAGS -fsanitize=address,undefined)
  endif ()
  set(EXTRA_LINK_FLAGS ${EXTRA_LINK_FLAGS} -fuse-ld=gold)

elseif (${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
  if (PROTEUS_ASAN)
    set(ASAN_FLAGS
      -fsanitize=address,undefined,integer
      -fno-sanitize=unsigned-integer-overflow)
  endif ()
endif ()

# Enable code coverage, if the compiler is supported
if (PROTEUS_BUILD_WITH_COVERAGE)
  set(COVERAGE_INFO coverage.info)

  if (${CMAKE_SYSTEM_NAME} MATCHES Linux AND ${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    # clang and linux's lcov don't play nice together; don't attempt with linux/clang
    add_custom_command(
      OUTPUT ${COVERAGE_INFO}
      COMMAND echo "Coverage info omitted for clang/linux builds")

  else ()
    add_custom_command(
      OUTPUT ${COVERAGE_INFO}
      # Capture coverage info.
      COMMAND lcov --directory . --capture --output-file ${COVERAGE_INFO}
      # Filter out system and test code.
      COMMAND lcov --remove ${COVERAGE_INFO} '*/tests/*' '*/test/*' '*/tck-test/*' '*/usr/include/*' '/usr/*' '*/gmock/*' '*/folly/*' --output-file
                   ${COVERAGE_INFO}
      # Debug before upload.
      COMMAND lcov --list ${COVERAGE_INFO})
  endif ()

  if (CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-g -O0 --coverage)
    set(EXTRA_LINK_FLAGS ${EXTRA_LINK_FLAGS} --coverage)

  elseif (${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    add_compile_options(-g -O0 -fprofile-arcs -ftest-coverage)
    set(EXTRA_LINK_FLAGS ${EXTRA_LINK_FLAGS} -fprofile-arcs -ftest-coverage)

  else ()
    message(FATAL_ERROR "Code coverage not supported with this compiler/host combination")
  endif ()

  message(STATUS "Building with coverage")
  add_custom_target(coverage DEPENDS ${COVERAGE_INFO})
endif ()

if (DEFINED ASAN_FLAGS)
  add_compile_options(${ASAN_FLAGS})
  set(EXTRA_LINK_FLAGS ${EXTRA_LINK_FLAGS} ${ASAN_FLAGS})
endif ()

enable_testing()

include(ExternalProject)
include(CTest)

include(${CMAKE_SOURCE_DIR}/cmake/InstallFolly.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/InstallRSocket.cmake)

# gmock
ExternalProject_Add(
  gmock
  URL ${CMAKE_CURRENT_SOURCE_DIR}/googletest-release-1.8.0.zip
  INSTALL_COMMAND ""
)

ExternalProject_Get_Property(gmock source_dir)
set(GMOCK_SOURCE_DIR ${source_dir})
ExternalProject_Get_Property(gmock binary_dir)
set(GMOCK_BINARY_DIR ${binary_dir})

set(GMOCK_LIBS
  ${GMOCK_BINARY_DIR}/${CMAKE_CFG_INTDIR}/googlemock/${CMAKE_STATIC_LIBRARY_PREFIX}gmock${CMAKE_STATIC_LIBRARY_SUFFIX}
  ${GMOCK_BINARY_DIR}/${CMAKE_CFG_INTDIR}/googlemock/${CMAKE_STATIC_LIBRARY_PREFIX}gmock_main${CMAKE_STATIC_LIBRARY_SUFFIX}
  )

set(CMAKE_CXX_STANDARD 14)

# Common configuration for all build modes.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

set(EXTRA_CXX_FLAGS ${EXTRA_CXX_FLAGS} -Werror)

if("${BUILD_TYPE_LOWER}" MATCHES "debug")
  message("debug mode was set")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unreachable-code")
else()
  message("release mode was set")
endif()

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(TEST_CXX_FLAGS ${TEST_CXX_FLAGS} -Wno-inconsistent-missing-override)
endif()

find_library(DOUBLE-CONVERSION double-conversion)

find_package(OpenSSL REQUIRED)

# Find glog and gflags libraries specifically
find_path(GLOG_INCLUDE_DIR glog/logging.h)
find_path(GFLAGS_INCLUDE_DIR gflags/gflags.h)

find_library(GLOG_LIBRARY glog)
find_library(GFLAGS_LIBRARY gflags)

message("gflags include_dir <${GFLAGS_INCLUDE_DIR}> lib <${GFLAGS_LIBRARY}>")
message("glog include_dir <${GLOG_INCLUDE_DIR}> lib <${GLOG_LIBRARY}>")

include_directories(SYSTEM ${OPENSSL_INCLUDE_DIR})

include_directories(SYSTEM ${GFLAGS_INCLUDE_DIR})
include_directories(SYSTEM ${GLOG_INCLUDE_DIR})

include_directories(${CMAKE_SOURCE_DIR})

include_directories(${GMOCK_SOURCE_DIR}/googlemock/include)
include_directories(${GMOCK_SOURCE_DIR}/googletest/include)

add_library(
  Proteus
  proteus/framing/ErrorCode.cpp
  proteus/framing/ErrorCode.h
  proteus/framing/Frame.cpp
  proteus/framing/Frame.h
  proteus/framing/FrameFlags.cpp
  proteus/framing/FrameFlags.h
  proteus/framing/FrameHeader.cpp
  proteus/framing/FrameHeader.h
  proteus/framing/FrameSerializer.cpp
  proteus/framing/FrameSerializer.h
  proteus/framing/FrameSerializer_v1_0.cpp
  proteus/framing/FrameSerializer_v1_0.h
  proteus/framing/FrameType.cpp
  proteus/framing/FrameType.h
  proteus/framing/ProtocolVersion.cpp
  proteus/framing/ProtocolVersion.h)

target_link_libraries(Proteus ReactiveSocket yarpl folly ${GFLAGS_LIBRARY} ${GLOG_LIBRARY})

target_compile_options(
  Proteus
  PRIVATE ${EXTRA_CXX_FLAGS})

enable_testing()

install(TARGETS Proteus DESTINATION lib)
install(DIRECTORY proteus DESTINATION include FILES_MATCHING PATTERN "*.h")

add_executable(
  tests
  proteus/test/framing/FrameTest.cpp)

target_link_libraries(
  tests
  Proteus
  ReactiveSocket
  yarpl
  folly
  ${GMOCK_LIBS}
  ${GFLAGS_LIBRARY}
  ${GLOG_LIBRARY})

target_compile_options(
  tests
  PRIVATE ${TEST_CXX_FLAGS})

add_dependencies(tests gmock Proteus)

add_test(NAME ProteusTests COMMAND tests)
