cmake_minimum_required(VERSION 2.8.3)
project(takeoff_and_land)

find_package(MAVSDK REQUIRED)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
)

catkin_package()

include_directories(
  ${catkin_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME}_node src/GPS.cpp)
add_dependencies(${PROJECT_NAME}_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(${PROJECT_NAME}_node
   ${catkin_LIBRARIES}
   MAVSDK::mavsdk_telemetry
   MAVSDK::mavsdk_action
   MAVSDK::mavsdk
 )

