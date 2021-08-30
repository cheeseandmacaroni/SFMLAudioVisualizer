cmake_minimum_required(VERSION 3.5)
project(AudioVisualizer VERSION 1.0.0 DESCRIPTION "Very nice project" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)

# Look for the SFML components we're interested in
set(SFML_DIR "SFML path here!!!!!")
find_package(SFML 2.5 COMPONENTS audio graphics window system REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release)

file(GLOB CPP_SOURCES "src/*.cpp")
file(GLOB HEADER_FILES "include/*.hpp")

add_executable(${PROJECT_NAME} ${CPP_SOURCES} ${HEADER_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Link our executable to SFML and its dependencies
target_link_libraries(${PROJECT_NAME} sfml-audio sfml-system sfml-window sfml-graphics)

#copy images,fonts and dll's
file(GLOB IMAGES "resources/images/*.jpg")
file(GLOB FONTS "resources/fonts/*.otf")
file(GLOB DLLS "bin/*.dll")

add_custom_target(build-time-make-fonts-directory ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/fonts")
add_custom_target(build-time-make-images-directory ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/images")
add_custom_target(build-time-make-sounds-directory ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/sounds")
add_custom_target(build-time-copy-binaries ALL)

add_custom_command(TARGET build-time-make-fonts-directory PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/resources/fonts    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/fonts)
add_custom_command(TARGET build-time-make-images-directory PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/resources/images    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/images)
add_custom_command(TARGET build-time-make-sounds-directory PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/resources/sounds    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/sounds)
add_custom_command(TARGET build-time-copy-binaries PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/bin    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})