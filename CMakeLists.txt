# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})

# Pull in Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(picodoom C CXX)

# Initialise the Pico SDK
pico_sdk_init()

# Do not panic on OOM
add_definitions(-DPICO_MALLOC_PANIC=0)

# Include the includes!
include_directories(include)

# Find all the .c and .cpp files in source/
file(GLOB_RECURSE SRC_C_FILES ${PROJECT_SOURCE_DIR}/source/*.c)
file(GLOB_RECURSE SRC_CPP_FILES ${PROJECT_SOURCE_DIR}/source/*.cpp)

# Add executable and source files
add_executable(picodoom
        ${SRC_C_FILES}
        ${SRC_CPP_FILES}
        )

pico_set_program_name(picodoom "picodoom")
pico_set_program_version(picodoom "0.1")

pico_enable_stdio_uart(picodoom 1)
pico_enable_stdio_usb(picodoom 0)

# Add the standard library to the build
target_link_libraries(picodoom pico_stdlib)

# Add relevant hardware libraries for RP2040
target_link_libraries(picodoom
        hardware_spi
        hardware_adc
        hardware_clocks
        hardware_pwm
        hardware_irq
        )

pico_add_extra_outputs(picodoom)

