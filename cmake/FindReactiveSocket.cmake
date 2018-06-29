cmake_minimum_required(VERSION 3.2)

include(FindPackageHandleStandardArgs)

if (RSOCKET_INSTALL_DIR)
  set(lib_paths ${RSOCKET_INSTALL_DIR}/lib)
  set(include_paths ${RSOCKET_INSTALL_DIR}/include)
endif ()

find_path(RSOCKET_INCLUDE_DIR "rsocket/RSocket.h" PATHS ${include_paths})
find_library(RSOCKET_LIBRARY ReactiveSocket PATHS ${lib_paths})

find_package_handle_standard_args(ReactiveSocket
  DEFAULT_MSG RSOCKET_INCLUDE_DIR RSOCKET_LIBRARY)
