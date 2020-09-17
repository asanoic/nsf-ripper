cmake_minimum_required(VERSION 3.13)

project("NsfRipper" LANGUAGES CXX)
set(CMAKE_VERBOSE_MAKEFILE OFF)

## start
set(pwd ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(flac)

list(APPEND gme-src
    "${pwd}/gme/Blip_Buffer.cpp"
    "${pwd}/gme/Classic_Emu.cpp"
    "${pwd}/gme/Data_Reader.cpp"
    "${pwd}/gme/Effects_Buffer.cpp"
    "${pwd}/gme/gme.cpp"
    "${pwd}/gme/Gme_File.cpp"
    "${pwd}/gme/Multi_Buffer.cpp"
    "${pwd}/gme/Music_Emu.cpp"
    "${pwd}/gme/Nes_Apu.cpp"
    "${pwd}/gme/Nes_Cpu.cpp"
    "${pwd}/gme/Nes_Fme7_Apu.cpp"
    "${pwd}/gme/Nes_Namco_Apu.cpp"
    "${pwd}/gme/Nes_Oscs.cpp"
    "${pwd}/gme/Nes_Vrc6_Apu.cpp"
    "${pwd}/gme/Nsf_Emu.cpp"
)

add_executable(${CMAKE_PROJECT_NAME} main.cpp ${gme-src})
target_compile_features(${CMAKE_PROJECT_NAME} PUBLIC cxx_std_17)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${pwd}/gme")
target_link_options(${CMAKE_PROJECT_NAME} PUBLIC "$<$<NOT:$<CONFIG:Debug>>:-s>" "-Wl,--gc-sections")

target_link_libraries(${CMAKE_PROJECT_NAME} FLAC)
