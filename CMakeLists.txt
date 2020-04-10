# author: Ania Brown 
# author: Jacob Wilkins
# author: Balint Koczor (Windows compatibility)

# CMake initialisation.
cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

# Project name
project(QuEST_Project)

# -----------------------------------------------------------------------------
# ----- USER OPTIONS ----------------------------------------------------------
# -----------------------------------------------------------------------------

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O2")

option(TESTING "Enable test suite functionality -- requires test suite available" ON)
set(QuEST_UTIL_DIR_NAME "utilities" CACHE STRING
  "Name of the directory containing the QuEST utilities.")

# -----------------------------------------------------------------------------
# ----- QuEST LIBRARY ---------------------------------------------------------
# ----------------------------------------------------------------------------- 

# Build the QuEST library if the path to libQuEST.so is not specified
if (NOT DEFINED ${QuEST_LIB_PATH})
    # Build libQuEST.so
    set(QuEST_DIR "QuEST" CACHE STRING 
        "Name of the directory containing the QuEST library sources. It must be located in the same directory as the root CMakeLists.txt")
    add_subdirectory(${QuEST_DIR})
    set(QuEST_LIB_PATH "${CMAKE_CURRENT_BINARY_DIR}/${QuEST_DIR}")
    set(QuEST_LIB_EXACT "${QuEST_LIB_PATH}/libQuEST.so")
endif()


# -----------------------------------------------------------------------------
# ----- USER EXECUTABLE -------------------------------------------------------
# -----------------------------------------------------------------------------

message(STATUS "Compiling ${USER_SOURCE} to executable qq_sim")


# Create user executable
add_executable(qq_sim ${CMAKE_SOURCE_DIR}/qq/qq_sim.cc
        qq/methods.h
        qq/methods.cc
        qq/types.h
        qq/qasm_parser.h
        qq/qasm_parser.cc qq/types.cc qq/utils.cc qq/utils.h)

target_compile_definitions(qq_sim
    PRIVATE
    QuEST_PREC=${PRECISION}
)

# Link libraries to user executable, including QuEST library
if (WIN32)
    target_link_libraries(qq_sim QuEST)
else ()
    target_link_libraries(qq_sim QuEST m)
endif()


# -----------------------------------------------------------------------------
# ----- UTILS -----------------------------------------------------------------
# -----------------------------------------------------------------------------

set(QuEST_UTIL_DIR "${QuEST_Project_SOURCE_DIR}/${QuEST_UTIL_DIR_NAME}")
# Set dummy UTIL_ROOT location -- Temporary while transitioning to External Project
set(UTIL_ROOT ${QuEST_UTIL_DIR})
# Dummy targets for QuESTPy & QuESTTest due to later inclusion of Utils
add_custom_target( QuESTTest DEPENDS QuEST)
add_custom_target( QuESTPy DEPENDS QuEST)

# -----------------------------------------------------------------------------
# ----- TESTS -----------------------------------------------------------------
# -----------------------------------------------------------------------------

if (${TESTING})
  set(Python_ADDITIONAL_VERSIONS 3.4;3.5;3.6;3.7;3.8)
  find_package(PythonInterp)
  if ((NOT PYTHONINTERP_FOUND) OR (PYTHON_VERSION_MAJOR LESS 3)  OR (PYTHON_VERSION_MINOR LESS 4))
    set(TESTING OFF CACHE BOOL  "Enable test suite functionality -- requires test suite available" FORCE)
    message(WARNING "Usable Python 3.4+ not found on system -- Disabling testing")
  endif()
endif()

message(STATUS "Testing is ${TESTING}")

if (${TESTING})
  enable_testing()
  add_subdirectory(${QuEST_UTIL_DIR})
endif()

# -----------------------------------------------------------------------------
# ----- QuEST Python Interface ------------------------------------------------
# -----------------------------------------------------------------------------

set(QuESTPy_PATH "${QuEST_UTIL_DIR}/QuESTPy" CACHE STRING "Location of QuESTPy")


add_custom_target( update_QuEST_lib_loc
  # Set QuESTPy default dir to this build
  DEPENDS QuESTPy
  COMMAND ${CMAKE_COMMAND} -DQuEST_ROOT=${PROJECT_SOURCE_DIR} -DQuESTPy_PATH=${QuESTPy_PATH} -DQuEST_LIB_EXACT=${QuEST_LIB_EXACT} -DQuEST_LIB_PATH=${QuEST_LIB_PATH} -P ${PROJECT_SOURCE_DIR}/cmake/QuESTPyLib.cmake
  )

install(TARGETS qq_sim DESTINATION bin)
