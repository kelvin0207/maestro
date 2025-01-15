cmake_minimum_required(VERSION 3.10)
project(Maestro)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含目录
include_directories(
    .
    ./cost-model/include
    ./cost-model/include/base
    ./cost-model/include/tools
    ./cost-model/include/user-api
    ./cost-model/include/dataflow-analysis
    ./cost-model/include/dataflow-specification-language
    ./cost-model/include/design-space-exploration
    ./cost-model/include/cost-analysis
    ./cost-model/include/abstract-hardware-model
    ./cost-model/src
)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
# 查找Boost库
find_package(Boost REQUIRED program_options filesystem system)

# 添加源文件
add_executable(maestro
    maestro-top.cpp
    cost-model/src/BASE_base-objects.cpp
)

# 设置链接标志
target_link_libraries(maestro PRIVATE
    Boost::program_options
    Boost::filesystem
    Boost::system
)

# 如果需要生成库文件，可以取消注释以下行
# add_library(maestro STATIC
#     maestro-top.cpp
#     cost-model/src/BASE_base-objects.cpp
# )