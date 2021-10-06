# StereoRocker: Edited to allow for building on Linux w/SDL
# TODO: Automate this somehow.
# NOTE: The "LINUX" build will probably work for other OS's too, it just needs to find SDL2.
set (PICO 1)
set (LINUX 0)

# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO)

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

        # Set a definition so we know this is a pico build
        add_definitions(-DPICO=1)

        # Include the includes!
        include_directories(include)

        # Find all the .c and .cpp files in source/
        file(GLOB SRC_C_FILES ${PROJECT_SOURCE_DIR}/source/*.c)
        file(GLOB SRC_CPP_FILES ${PROJECT_SOURCE_DIR}/source/*.cpp)

        # Find all the .c and .cpp files in source/pico/
        file(GLOB SRC_PLATFORM_C_FILES ${PROJECT_SOURCE_DIR}/source/pico/*.c)
        file(GLOB SRC_PLATFORM_CPP_FILES ${PROJECT_SOURCE_DIR}/source/pico/*.cpp)

        # Add executable and source files
        add_executable(picodoom
                ${SRC_C_FILES}
                ${SRC_CPP_FILES}
                ${SRC_PLATFORM_C_FILES}
                ${SRC_PLATFORM_CPP_FILES}
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

elseif (LINUX)

        # Define the project
        project(picodoom C CXX)

        # Include SDL2
        find_package(SDL2 REQUIRED)
        include_directories(${SDL2_INCLUDE_DIRS})

        # Set a definition so we know this isn't a pico build
        add_definitions(-DLINUX=1)

        # Include the includes!
        include_directories(include)

        # Find all the .c and .cpp files in source/
        file(GLOB SRC_C_FILES ${PROJECT_SOURCE_DIR}/source/*.c)
        file(GLOB SRC_CPP_FILES ${PROJECT_SOURCE_DIR}/source/*.cpp)

        # Find all the .c and .cpp files in source/linux/
        file(GLOB SRC_PLATFORM_C_FILES ${PROJECT_SOURCE_DIR}/source/linux/*.c)
        file(GLOB SRC_PLATFORM_CPP_FILES ${PROJECT_SOURCE_DIR}/source/linux/*.cpp)

        # Add executable and source files
        add_executable(picodoom
                ${SRC_C_FILES}
                ${SRC_CPP_FILES}
                ${SRC_PLATFORM_C_FILES}
                ${SRC_PLATFORM_CPP_FILES}
                )
        
        # Link SDL2
        target_link_libraries(picodoom ${SDL2_LIBRARIES})
endif()