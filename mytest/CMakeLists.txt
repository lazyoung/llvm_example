cmake_minimum_required (VERSION 3.12)

project(llvmdis)

set(LLVM_DIR ${PROJECT_BINARY_DIR}/../../llvm_install/)
find_package(LLVM REQUIRED CONFIG HINTS "${LLVM_DIR}")
message(STATUS "LLVM_INCLUDE_DIRS: " ${LLVM_INCLUDE_DIRS})
message(STATUS "LLVM_DEFINITIONS_LIST: " ${LLVM_DEFINITIONS_LIST})
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS_LIST})
add_definitions(-Wall)

add_executable(llvmdiscpp llvmdis.cpp)
target_link_libraries(llvmdiscpp LLVM)