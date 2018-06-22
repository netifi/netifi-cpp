cmake_minimum_required(VERSION 3.2)

include(FindPackageHandleStandardArgs)

if (RSOCKET_INSTALL_DIR)
  set(lib_paths ${RSOCKET_INSTALL_DIR}/lib)
  set(include_paths ${RSOCKET_INSTALL_DIR}/include)
endif ()

find_library(RSOCKET_LIBRARY ReactiveSocket PATHS ${lib_paths})
find_library(YARPL_LIBRARY yarpl PATHS ${lib_paths})
find_path(RSOCKET_INCLUDE_DIR "rsocket/RSocket.h" PATHS ${include_paths})
find_path(YARPL_INCLUDE_DIR "yarpl/Flowable.h" PATHS ${include_paths})

find_package_handle_standard_args(RSocket
  DEFAULT_MSG RSOCKET_LIBRARY YARPL_LIBRARY RSOCKET_INCLUDE_DIR YARPL_INCLUDE_DIR)
