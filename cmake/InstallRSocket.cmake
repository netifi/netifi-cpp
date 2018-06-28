if (NOT RSOCKET_INSTALL_DIR)
  set(RSOCKET_INSTALL_DIR $ENV{HOME}/rsocket)
endif ()

# Check if the correct version of rsocket is already installed.
set(RSOCKET_VERSION master)
set(RSOCKET_VERSION_FILE ${RSOCKET_INSTALL_DIR}/${RSOCKET_VERSION})
if (PROTEUS_INSTALL_DEPS)
  if (NOT EXISTS ${RSOCKET_VERSION_FILE})
    # Remove the old version of rsocket.
    file(REMOVE_RECURSE ${RSOCKET_INSTALL_DIR})
    set(INSTALL_RSOCKET True)
  endif ()
endif ()

if (INSTALL_RSOCKET)
  # Build and install rsocket.
  ExternalProject_Add(
    rsocket-ext
    GIT_REPOSITORY https://github.com/rsocket/rsocket-cpp
    GIT_TAG ${RSOCKET_VERSION}
    CMAKE_ARGS += -DCMAKE_INSTALL_PREFIX=${RSOCKET_INSTALL_DIR} -DBUILD_SHARED_LIBS=ON
    TEST_AFTER_INSTALL 1
    TEST_COMMAND ${CMAKE_COMMAND} -E touch ${RSOCKET_VERSION_FILE}
    DEPENDS folly)

  set(RSOCKET_INCLUDE_DIR ${RSOCKET_INSTALL_DIR}/include)
  set(rsocketLib ${CMAKE_SHARED_LIBRARY_PREFIX}ReactiveSocket${CMAKE_SHARED_LIBRARY_SUFFIX})
  set(yarplLib ${CMAKE_SHARED_LIBRARY_PREFIX}yarpl${CMAKE_SHARED_LIBRARY_SUFFIX})
  set(RSOCKET_LIBRARY ${RSOCKET_INSTALL_DIR}/lib/${rsocketLib})
  set(YARPL_LIBRARY ${RSOCKET_INSTALL_DIR}/lib/${yarplLib})

  # CMake requires directories listed in INTERFACE_INCLUDE_DIRECTORIES to exist.
  file(MAKE_DIRECTORY ${RSOCKET_INCLUDE_DIR})
else ()
  # Use installed rsocket.
  find_package(RSocket REQUIRED)
endif ()

find_package(Folly)

add_library(ReactiveSocket SHARED IMPORTED)
set_property(TARGET ReactiveSocket PROPERTY IMPORTED_LOCATION ${RSOCKET_LIBRARY})
set_property(TARGET ReactiveSocket
  APPEND PROPERTY INTERFACE_LINK_LIBRARIES
  ${EXTRA_LINK_FLAGS} ${EVENT_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
if (TARGET rsocket-ext)
  add_dependencies(ReactiveSocket rsocket-ext)
endif ()

add_library(yarpl SHARED IMPORTED)
set_property(TARGET yarpl PROPERTY IMPORTED_LOCATION ${YARPL_LIBRARY})
set_property(TARGET yarpl
  APPEND PROPERTY INTERFACE_LINK_LIBRARIES
  ${EXTRA_LINK_FLAGS} ${EVENT_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
if (TARGET rsocket-ext)
  add_dependencies(yarpl rsocket-ext)
endif ()

# RSocket includes are marked as system to prevent errors on non-standard
# extensions when compiling with -pedantic and -Werror.
set_property(TARGET ReactiveSocket
  APPEND PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES ${RSOCKET_INCLUDE_DIR})
set_property(TARGET ReactiveSocket
  APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${RSOCKET_INCLUDE_DIR})
set_property(TARGET yarpl
  APPEND PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES ${RSOCKET_INCLUDE_DIR})
set_property(TARGET yarpl
  APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${RSOCKET_INCLUDE_DIR})
