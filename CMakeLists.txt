cmake_minimum_required(VERSION 3.6)
project(ACScript)
find_package(XP-CMake REQUIRED) #we need advanced cmake modules

set(CMAKE_CXX_STANDARD 20)

XPC_AutoBuildType()
XPC_OptForHost()
XPC_SetCompileDefinitions()

find_package(StdPlusPlus REQUIRED)

add_executable(acsvm src_vm/main.cpp src_vm/ExternalsManager.cpp src_vm/ExternalsManager.hpp src_vm/Module.hpp src_vm/VM.hpp src_vm/Module.cpp src_vm/RuntimeValue.hpp src_vm/VM.cpp src_vm/Opcode.hpp src_vm/externals/Print.cpp src_vm/externals/Arithmetical.cpp src_vm/externals/Logical.cpp)
target_link_libraries(acsvm Std++ Std++Static)
target_include_directories(acsvm PRIVATE "include")