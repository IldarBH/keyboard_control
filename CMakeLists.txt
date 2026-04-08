cmake_minimum_required(VERSION 3.20)
project(keyboard_control)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(yaml-cpp REQUIRED)

add_executable(${PROJECT_NAME}_node src/keyboard_control.cpp src/keyboard_control_node.cpp)
target_include_directories(${PROJECT_NAME}_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(
  ${PROJECT_NAME}_node
  ${rclcpp_TARGETS}
  ${geometry_msgs_TARGETS}
  ${YAML_CPP_LIBRARIES}
)
# suppress pedantic warnings
target_compile_options(${PROJECT_NAME}_node PRIVATE -Wno-pedantic)

install(TARGETS ${PROJECT_NAME}_node
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  config
  launch
  DESTINATION share/${PROJECT_NAME}
)

ament_package()