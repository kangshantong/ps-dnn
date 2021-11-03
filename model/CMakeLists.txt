cmake_minimum_required (VERSION 3.0)
project (ps-dnn)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_definitions(-std=c++11 -msse2 -fPIC -O3 -ggdb -finline-functions)
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lstdc++ -lprofiler")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lstdc++")

include_directories ("${PROJECT_SOURCE_DIR}/third_party/eigen")
include_directories ("${PROJECT_SOURCE_DIR}/third_party/ps-lite/include")
link_libraries("${PROJECT_SOURCE_DIR}/third_party/ps-lite/build/libps.a")
link_libraries("${PROJECT_SOURCE_DIR}/third_party/ps-lite/deps/lib/libprotobuf-lite.so")
link_libraries("${PROJECT_SOURCE_DIR}/third_party/ps-lite/deps/lib/libzmq.so")

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(./dataset DIR_LIB_SRCS)
aux_source_directory(./emb DIR_LIB_SRCS)
aux_source_directory(./dist DIR_LIB_SRCS)
aux_source_directory(./layer DIR_LIB_SRCS)
aux_source_directory(./loss DIR_LIB_SRCS)
aux_source_directory(./network DIR_LIB_SRCS)
aux_source_directory(./optimizer DIR_LIB_SRCS)
aux_source_directory(./procedure DIR_LIB_SRCS)
aux_source_directory(./utils DIR_LIB_SRCS)

# 生成链接库
add_library (DNNLib ${DIR_LIB_SRCS})

# 指定生成目标 
add_executable(local local.cc)
add_executable(dist_multi_process dist_multi_process.cc)
add_executable(dist_multi_thread dist_multi_thread.cc)

# 添加链接库
target_link_libraries(local DNNLib)
target_link_libraries(dist_multi_process DNNLib)
target_link_libraries(dist_multi_thread DNNLib)
