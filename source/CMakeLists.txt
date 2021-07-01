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
include_directories(../include)

# Add executable. Default name is the project name, version 0.1

add_executable(picodoom
        i_main_pico.cpp
        i_main_pico_c.c
        i_system_e32.cpp
        i_system_gba.cpp
        ili9341.cpp
        am_map.c
        d_client.c
        d_items.c
        d_main.c
        doom_iwad.c
        f_finale.c
        f_wipe.c
        g_game.c
        global_data.c
        hu_lib.c
        hu_stuff.c
        i_audio.c
        #i_main_gba.c
        info.c
        i_system.c
        i_video.c
        lprintf.c
        m_bbox.c
        m_cheat.c
        m_menu.c
        m_random.c
        m_recip.c
        p_ceilng.c
        p_doors.c
        p_enemy.c
        p_floor.c
        p_genlin.c
        p_inter.c
        p_lights.c
        p_map.c
        p_maputl.c
        p_mobj.c
        p_plats.c
        p_pspr.c
        p_setup.c
        p_sight.c
        p_spec.c
        p_switch.c
        p_telept.c
        p_tick.c
        p_user.c
        r_data.c
        r_draw.c
        r_hotpath.iwram.c
        r_main.c
        r_patch.c
        r_plane.c
        r_things.c
        sounds.c
        s_sound.c
        st_gfx.c
        st_lib.c
        st_stuff.c
        tables.c
        version.c
        v_video.c
        wi_stuff.c
        w_wad.c
        z_bmalloc.c
        z_zone.c
        )

pico_set_program_name(picodoom "picodoom")
pico_set_program_version(picodoom "0.1")

pico_enable_stdio_uart(picodoom 1)
pico_enable_stdio_usb(picodoom 0)

# Add the standard library to the build
target_link_libraries(picodoom pico_stdlib)

# Add any user requested libraries
target_link_libraries(picodoom
        hardware_spi
        hardware_adc
        )

pico_add_extra_outputs(picodoom)

