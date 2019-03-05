cmake_minimum_required(VERSION 3.6)
project(ACScript)
find_package(XP-CMake REQUIRED) #we need advanced cmake modules

set(CMAKE_CXX_STANDARD 17)

XPC_AutoBuildType()
XPC_OptForHost()
XPC_SetCompileDefinitions()

find_package(StdPlusPlus REQUIRED)

add_subdirectory(src)
add_executable(ACScript ${SOURCE_FILES})
target_link_libraries(ACScript Std++ Std++Static)