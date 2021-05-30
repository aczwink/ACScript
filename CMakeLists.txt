cmake_minimum_required(VERSION 3.6)
project(ACScript)
find_package(XP-CMake REQUIRED) #we need advanced cmake modules

set(CMAKE_CXX_STANDARD 20)

XPC_AutoBuildType()
XPC_OptForHost()
XPC_SetCompileDefinitions()

find_package(StdPlusPlus REQUIRED)

add_subdirectory(include/acsb)

add_subdirectory(src_vm)
add_executable(acsvm ${SOURCE_FILES_VM})
target_link_libraries(acsvm Std++ Std++Static)
target_include_directories(acsvm PRIVATE "include")



#add_subdirectory(src_compiler)

#find_package(FLEX REQUIRED)
#find_package(BISON REQUIRED)
#FLEX_TARGET(ACScriptLexer parser/lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
#BISON_TARGET(ACScriptParser parser/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)

#find_package(LLVM REQUIRED CONFIG)

#ADD_FLEX_BISON_DEPENDENCY(ACScriptLexer ACScriptParser)
#add_executable(acsc ${SOURCE_FILES_COMPILER} ${FLEX_ACScriptLexer_OUTPUTS} ${BISON_ACScriptParser_OUTPUTS})
#target_link_libraries(acsc Std++ Std++Static)

#execute_process(COMMAND "llvm-config" "--components" OUTPUT_VARIABLE llvm_components OUTPUT_STRIP_TRAILING_WHITESPACE)
#string(REPLACE " " ";" llvm_components ${llvm_components})
#message(${llvm_components})
#set(llvm_components core x86asmparser x86codegen)
#llvm_map_components_to_libnames(llvm_libs ${llvm_components})

#target_include_directories(acsc PRIVATE "include")
#target_include_directories(acsc PRIVATE ${LLVM_INCLUDE_DIRS})
#target_compile_definitions(acsc PRIVATE ${LLVM_DEFINITIONS})
#target_link_libraries(acsc ${llvm_libs})