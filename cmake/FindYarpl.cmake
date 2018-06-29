cmake_minimum_required(VERSION 3.2)

include(FindPackageHandleStandardArgs)

if (YARPL_INSTALL_DIR)
  set(lib_paths ${YARPL_INSTALL_DIR}/lib)
  set(include_paths ${YARPL_INSTALL_DIR}/include)
endif ()

find_path(YARPL_INCLUDE_DIR "yarpl/Flowable.h" PATHS ${include_paths})
find_library(YARPL_LIBRARY yarpl PATHS ${lib_paths})

find_package_handle_standard_args(yarpl
  DEFAULT_MSG YARPL_INCLUDE_DIR YARPL_LIBRARY)
