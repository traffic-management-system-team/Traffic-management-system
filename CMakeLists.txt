cmake_minimum_required(VERSION 3.16)

if(POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "Embedded")
endif()

project(TrafficManagementSystem VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

option(BUILD_API_SERVER "Build the Crow-based REST API server" ON)
set(TMS_HTTP_MAX_REQUEST_BODY_BYTES 65536)

add_library(tms_core
    src/TrafficManager.cpp
    src/TrafficSignal.cpp
    src/simulation_model.cpp
    src/simulation_service.cpp
    src/simulation_statistics_manager.cpp
    src/input_validator.cpp
    src/config_controller.cpp
    src/simulation_control_api.cpp
    src/queue_manager.cpp
    src/vehicle.cpp
    src/vehicle_manager.cpp
)
target_include_directories(tms_core PUBLIC include src)
target_link_libraries(tms_core PUBLIC Threads::Threads)

if(BUILD_API_SERVER)
    include(FetchContent)
    find_package(Git REQUIRED)

    set(JSON_BuildTests OFF CACHE INTERNAL "")
    set(JSON_Install OFF CACHE INTERNAL "")
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG 55f93686c01528224f448c19128836e7df245f72
        GIT_SHALLOW FALSE
    )
    FetchContent_MakeAvailable(nlohmann_json)

    FetchContent_Declare(
        asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
        GIT_TAG 231cb29bab30f82712fcd54faaea42424cc6e710
        GIT_SHALLOW FALSE
    )
    FetchContent_MakeAvailable(asio)
    set(ASIO_INCLUDE_DIR "${asio_SOURCE_DIR}/asio/include")

    set(CROW_BUILD_EXAMPLES OFF)
    set(CROW_BUILD_TESTS OFF)
    set(CROW_BUILD_FUZZER OFF)
    set(CROW_INSTALL OFF)
    set(CROW_USE_BOOST OFF)
    set(CROW_ENABLE_SSL OFF)
    set(CROW_ENABLE_COMPRESSION OFF)

    FetchContent_Declare(
        crow
        GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
        GIT_TAG fcf0c360b15fc245d884c71340b08ac20403ba36
        GIT_SHALLOW FALSE
        PATCH_COMMAND
            "${GIT_EXECUTABLE}" -c
            core.whitespace=cr-at-eol,blank-at-eol,blank-at-eof,space-before-tab
            apply --ignore-space-change --unidiff-zero --whitespace=error
            "${CMAKE_CURRENT_LIST_DIR}/patches/crow-request-body-limit.patch"
    )
    FetchContent_MakeAvailable(crow)

    add_library(tms_http
        src/simulation_http_adapter.cpp
        src/simulation_http_response.cpp
        src/simulation_http_server.cpp
    )
    target_include_directories(tms_http PUBLIC include)
    target_link_libraries(tms_http PUBLIC tms_core Crow::Crow)
    target_compile_definitions(tms_http PUBLIC
        TMS_HTTP_MAX_REQUEST_BODY_BYTES=${TMS_HTTP_MAX_REQUEST_BODY_BYTES}
    )

    if(MSVC)
        target_compile_definitions(tms_http PUBLIC _WIN32_WINNT=0x0601)
        target_compile_options(tms_http PRIVATE /W4)
    else()
        target_compile_options(tms_http PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    add_executable(tms_api_server src/api_server_main.cpp)
    target_link_libraries(tms_api_server PRIVATE tms_http)

    add_library(tms_state_serializer
        src/simulation_state_serializer.cpp
    )
    target_include_directories(tms_state_serializer PUBLIC include)
    target_link_libraries(tms_state_serializer
        PUBLIC tms_core nlohmann_json::nlohmann_json
    )

    if(MSVC)
        target_compile_options(tms_state_serializer PRIVATE /W4)
    else()
        target_compile_options(tms_state_serializer PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    target_link_libraries(tms_http PRIVATE tms_state_serializer)
endif()

if(MSVC)
    target_compile_options(tms_core PRIVATE /W4)
else()
    target_compile_options(tms_core PRIVATE -Wall -Wextra -Wpedantic)
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
    add_tms_test(input_validator_test tests/input_validator_test.cpp)
    add_tms_test(simulation_service_test tests/simulation_service_test.cpp)
    add_tms_test(simulation_statistics_manager_test tests/simulation_statistics_manager_test.cpp)
    add_tms_test(simulation_control_api_test tests/simulation_control_api_test.cpp)

    if(BUILD_API_SERVER)
        add_tms_test(simulation_http_server_test tests/simulation_http_server_test.cpp)
        target_link_libraries(simulation_http_server_test PRIVATE tms_http tms_state_serializer)

        add_tms_test(
            simulation_state_serializer_test
            tests/simulation_state_serializer_test.cpp
        )
        target_link_libraries(
            simulation_state_serializer_test
            PRIVATE tms_state_serializer
        )

        add_tms_test(
            simulation_api_contract_test
            tests/simulation_api_contract_test.cpp
        )
        target_link_libraries(
            simulation_api_contract_test
            PRIVATE tms_http tms_state_serializer
        )
    endif()
endif()
