cmake_minimum_required(VERSION 3.16.3)

project(sitk_example)

find_package(SimpleITK)
add_executable (lkz-network Program.cpp)
target_link_libraries ( sitk_example ${SimpleITK_LIBRARIES} )