# CMakeList.txt: QHarmonicSubtraction 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

set(EXECUTABLE_NAME "QHarmonicSubtraction")

# 将源代码添加到此项目的可执行文件。
add_executable(${EXECUTABLE_NAME} "QHarmonicSubtraction.cpp")

message("========Printing EXECUTABLE_NAME...")
message("${EXECUTABLE_NAME}")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${EXECUTABLE_NAME} PROPERTY CXX_STANDARD 17)
  target_compile_options(${EXECUTABLE_NAME} PUBLIC -Wall)
endif()

# TODO: 如有需要，请添加测试并安装目标。

# If you would like to see the default list, run root-config --libs on the command line.
# Linking to needed ROOT libraries
target_link_libraries(${EXECUTABLE_NAME}
    PUBLIC ROOT::Core
    PUBLIC ROOT::Gpad
    PUBLIC ROOT::Graf
    PUBLIC ROOT::Hist
    PUBLIC ROOT::MathCore
    PUBLIC ROOT::RIO
    PUBLIC ROOT::Matrix
    PUBLIC ROOT::Imt
    PUBLIC ROOT::Thread
    PUBLIC ROOT::MultiProc
    PUBLIC ROOT::Net
    PUBLIC ROOT::Tree
)

# 包含相关的头文件
target_include_directories(${EXECUTABLE_NAME} 
    PUBLIC "../../include"
)

unset(EXECUTABLE_NAME)

