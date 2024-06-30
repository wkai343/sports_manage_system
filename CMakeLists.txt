cmake_minimum_required(VERSION 3.20)
project(Sports LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# aux_source_directory("${PROJECT_SOURCE_DIR}/src" SRC)
file(GLOB SRC "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/*.ui")
file(GLOB INCLUDE "${PROJECT_SOURCE_DIR}/include/*.h")
file(GLOB RES "${PROJECT_SOURCE_DIR}/res/*.qrc")
find_package(Qt6 COMPONENTS Widgets Sql REQUIRED)
qt_add_executable(${PROJECT_NAME}
    MANUAL_FINALIZATION
    # WIN32
    ${INCLUDE} ${SRC} ${RES}
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Sql)
target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/include")
qt_finalize_executable(${PROJECT_NAME})
