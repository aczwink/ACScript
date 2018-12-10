cmake_minimum_required(VERSION 3.6)
project(ACScript)
find_package(XP-CMake REQUIRED) #we need advanced cmake modules

set(CMAKE_CXX_STANDARD 17)

XPC_AutoBuildType()
XPC_OptForHost()
XPC_SetCompileDefinitions()

find_package(StdPlusPlus REQUIRED)

add_executable(ACScript src/ast/Statement.hpp src/Lexer.cpp src/Lexer.hpp src/main.cpp src/Parser.cpp src/Parser.hpp)
target_link_libraries(ACScript Std++ Std++Static)