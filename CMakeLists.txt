cmake_minimum_required(VERSION 3.20)
project(keyboard_control_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)

add_executable(${PROJECT_NAME} src/keyboard_control.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include
)
# THIS is the correct way for message packages:
target_link_libraries(${PROJECT_NAME}
  rclcpp::rclcpp
  geometry_msgs::geometry_msgs__rosidl_typesupport_cpp
)

install(TARGETS
  ${PROJECT_NAME}
  DESTINATION lib/${PROJECT_NAME}
)


