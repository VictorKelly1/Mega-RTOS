cmake_minimum_required(VERSION 3.16)
project(RTOS)

set(CMAKE_CXX_COMPILER avr-g++)
set(CMAKE_C_COMPILER avr-gcc)

set(MCU atmega328p)
set(F_CPU 16000000UL)

add_definitions(-DF_CPU=${F_CPU})

add_executable(RTOS
    src/main.cpp
)

target_include_directories(RTOS PRIVATE include)

target_compile_options(RTOS PRIVATE
    -mmcu=${MCU}
    -Os
)

target_link_options(RTOS PRIVATE
    -mmcu=${MCU}
)
