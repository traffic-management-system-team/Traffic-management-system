cmake_minimum_required(VERSION 3.16)

project(TrafficManagementSystem VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_GUI "Build the Qt 6 Widgets desktop application" ON)

add_library(tms_core
    src/TrafficManager.cpp
    src/TrafficSignal.cpp
    src/gui_model.cpp
    src/queue_manager.cpp
    src/vehicle.cpp
    src/vehicle_manager.cpp
)
target_include_directories(tms_core PUBLIC include src)

if(MSVC)
    target_compile_options(tms_core PRIVATE /W4)
else()
    target_compile_options(tms_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(BUILD_GUI)
    find_package(Qt6 REQUIRED COMPONENTS Widgets)
    add_executable(TrafficManagementSystem WIN32 src/gui_main.cpp)
    target_link_libraries(TrafficManagementSystem PRIVATE tms_core Qt6::Widgets)
endif()

include(CTest)
if(BUILD_TESTING)
    function(add_tms_test target)
        add_executable(${target} ${ARGN})
        target_link_libraries(${target} PRIVATE tms_core)
        if(MSVC)
            target_compile_options(${target} PRIVATE /W4)
        else()
            target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
        endif()
        add_test(NAME ${target} COMMAND ${target})
    endfunction()

    add_tms_test(vehicle_manager_test tests/vehicle_manager_test.cpp)
    add_tms_test(queue_manager_test tests/queue_manager_test.cpp)
    add_tms_test(queue_empty_safety_test tests/queue_empty_safety_test.cpp)
    add_tms_test(queue_length_test tests/queue_length_test.cpp)
    add_tms_test(traffic_signal_test tests/test_traffic.cpp)
    add_tms_test(traffic_manager_test tests/test_traffic_manager.cpp)
    add_tms_test(gui_model_test tests/gui_model_test.cpp)
endif()
