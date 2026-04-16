cmake_minimum_required(VERSION 3.16)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_CXX_COMPILER avr-g++)
set(CMAKE_ASM_COMPILER avr-gcc) 

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(megaRTOS C CXX ASM)

set(CMAKE_CXX_STANDARD 17)         
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
set(CMAKE_CXX_EXTENSIONS OFF)

set(MCU atmega328p)
set(F_CPU 16000000UL)


add_library(megaRTOS STATIC
    src/kernel/Kernel.cpp
    src/kernel/Process.cpp
    src/drivers/UART.cpp
    src/procedures/switchContextASM.S
)

target_include_directories(megaRTOS 
    PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    INTERFACE $<INSTALL_INTERFACE:include>
)

target_compile_options(megaRTOS PUBLIC
    -mmcu=${MCU}
)

target_compile_options(megaRTOS PRIVATE
    -Os
    -Wall
    -ffunction-sections
    -fdata-sections
    -DF_CPU=${F_CPU}
)



add_executable(demo_rtos examples/main_demo.cpp)

target_link_libraries(demo_rtos PRIVATE megaRTOS)

target_compile_options(demo_rtos PRIVATE -Os)

target_link_options(demo_rtos PRIVATE
    -mmcu=${MCU}
    -Wl,--gc-sections
    -Wl,--relax
)
