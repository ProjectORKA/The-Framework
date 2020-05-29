cmake_minimum_required(VERSION 3.15)
project(ORKA-Framework C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)
add_subdirectory(LCLib-CXX)

include_directories(include LCLib-CXX/include)
add_executable(orka include/Framework.h src/Framework.cpp src/ModuleFormat.hpp src/ModuleFormat.cpp)
target_link_libraries(orka lc-c++)