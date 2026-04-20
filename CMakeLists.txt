cmake_minimum_required(VERSION 3.16)
project(emu C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

# 设置C标准为C11（或根据需要调整）
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# 设置C++标准为C++23（或根据需要调整）
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# 设置编译器为clang（如果未指定，使用默认的clang/clang++）
if(NOT DEFINED CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER "clang")
endif()
if(NOT DEFINED CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER "clang++")
endif()

# 设置默认构建类型为Release（如果没有指定）
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

# 包含头文件目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# 查找所有源文件
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/src/*.c" "${CMAKE_SOURCE_DIR}/src/*.cc" "${CMAKE_SOURCE_DIR}/src/*.cxx")

if(NOT SOURCES)
    message(WARNING "No source files found in ${CMAKE_SOURCE_DIR}/src/")
endif()

# 添加根目录C和C++源文件支持（兼容现有C代码和未来C++代码）
file(GLOB ROOT_C_SOURCES "${CMAKE_SOURCE_DIR}/*.c")
file(GLOB ROOT_CPP_SOURCES "${CMAKE_SOURCE_DIR}/*.cpp" "${CMAKE_SOURCE_DIR}/*.cc" "${CMAKE_SOURCE_DIR}/*.cxx")
if(ROOT_C_SOURCES OR ROOT_CPP_SOURCES)
    list(APPEND SOURCES ${ROOT_C_SOURCES} ${ROOT_CPP_SOURCES})
    message(STATUS "Found root directory sources: C=${ROOT_C_SOURCES}, C++=${ROOT_CPP_SOURCES}")
endif()

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 链接ncurses库
target_link_libraries(${PROJECT_NAME} curses)

# 设置编译选项
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # 调试模式：-g3, -O0，并添加内存检查器
    target_compile_options(${PROJECT_NAME} PRIVATE
        -g3
        -O0
        -fno-omit-frame-pointer
        -pipe
        -pg
    )

    # 添加内存检查器（AddressSanitizer, UndefinedBehaviorSanitizer等）
    # target_compile_options(${PROJECT_NAME} PRIVATE
      # -fsanitize=address
        # -fsanitize=undefined
        # -fsanitize=leak
        # )
    # target_link_options(${PROJECT_NAME} PRIVATE
      # -fsanitize=address
        # -fsanitize=undefined
        # -fsanitize=leak
        # -pg
        # )

    message(STATUS "Building in Debug mode with memory sanitizers enabled")
else()
    # 发布模式：-O3，添加Makefile中的选项
    target_compile_options(${PROJECT_NAME} PRIVATE
        -O3
        -pipe
        -pg
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        -pg
    )
    message(STATUS "Building in Release mode with optimizations enabled")
endif()

# 添加额外的警告
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    # -Wextra
    # -Wpedantic
    # -Wconversion
    # -Wsign-conversion
)

# 设置目标属性
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# 安装目标（可选）
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

