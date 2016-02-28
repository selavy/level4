# src/CMakeLists.txt

add_executable(
  level4
  main.cpp
  )

target_link_libraries(level4 ${Boost_LIBRARIES})