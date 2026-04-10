cmake_minimum_required(VERSION 3.16)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_CXX_COMPILER avr-g++)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(RTOS C CXX ASM)

set(CMAKE_CXX_STANDARD 17)          
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
set(CMAKE_CXX_EXTENSIONS OFF)

set(MCU atmega328p)
set(F_CPU 16000000UL)

add_compile_definitions(F_CPU=${F_CPU})

add_executable(RTOS
    src/main.cpp
    src/kernel/Kernel.cpp
    src/kernel/Process.cpp
    src/procedures/switchContextASM.S
    src/drivers/UART.cpp 
)

target_include_directories(RTOS PRIVATE
    include
)

target_compile_options(RTOS PRIVATE
    -mmcu=${MCU}
    -Os
)

target_link_options(RTOS PRIVATE
    -mmcu=${MCU}
)
