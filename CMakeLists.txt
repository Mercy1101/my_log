# 设置CMAKE最小版本
cmake_minimum_required(VERSION 3.1)

# 可执行文件名的名称
set(EXECUTABLE_EXE_NAME my_log)
set(PROJECT_NAME my_log)

# 可执行文件生成路径
set(EXECUTABLE_OUTPUT_PATH ../bin)
# 测试可执行文件运行路径
# set(UNIT_TEST_BIN_OUTPUT_DIR ${EXECUTABLE_OUTPUT_PATH})

# 设置工程名称
project (${EXECUTABLE_EXE_NAME})

# 添加静态库的连接路径
#link_directories(
                 # Boost库的路径
                 # $ENV{BOOST_LIB_DIR})

# main函数
set(SOURCES
    src/main.cc
)

# 单元测试文件
set(UNITEST_SOURCES
  # test/file_helper_unittest.cc
  test/log_wrapper_unittest.cc
  # test/profiler_unittest.cc
)

# 把源文件添加进工程中
add_executable(${EXECUTABLE_EXE_NAME} ${SOURCES}
                                      #${UNITEST_SOURCES}
)

target_link_libraries(${EXECUTABLE_EXE_NAME} PUBLIC pthread)
# 设置包含路径
target_include_directories(${EXECUTABLE_EXE_NAME}
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/thirdparty
)


#target_link_libraries(${EXECUTABLE_EXE_NAME} ${DONGJIN_API_LIB})

# 设置VS警告等级为Warning4
if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()

# 设置VS运行库为/MD和/MTD
if (MSVC)
set(CompilerFlags
    CMAKE_CXX_FLAGS
    CMAKE_CXX_FLAGS_DEBUG
    CMAKE_CXX_FLAGS_RELEASE
    CMAKE_C_FLAGS
    CMAKE_C_FLAGS_DEBUG
    CMAKE_C_FLAGS_RELEASE
    )
foreach(CompilerFlag ${CompilerFlags})
    string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()
endif(MSVC)

# 设置GCC最高警告级别
if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")
endif()
# 设置MSVC最高警告级别
if ( MSVC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4") # 设置警告等级为4
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /WX") # 设置警告视为错误
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /std:c++latest") # 设置使用最新C++标准
endif()
