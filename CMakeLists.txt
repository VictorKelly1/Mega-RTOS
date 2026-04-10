cmake_minimum_required(VERSION 3.16)

# Configuración de Cross-Compilation para AVR
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_CXX_COMPILER avr-g++)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(megaRTOS C CXX ASM)

set(CMAKE_CXX_STANDARD 17)          
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
set(CMAKE_CXX_EXTENSIONS OFF)

set(MCU atmega328p)
set(F_CPU 16000000UL)

add_compile_definitions(F_CPU=${F_CPU})

# --- CONFIGURACIÓN DE LA LIBRERÍA ---

# Definimos la librería estática (se generará libmegaRTOS.a)
add_library(megaRTOS STATIC
    src/kernel/Kernel.cpp
    src/kernel/Process.cpp
    src/drivers/UART.cpp
    src/procedures/switchContextASM.S
)

# Definimos los headers como PUBLIC para que cualquiera que use la lib los encuentre
target_include_directories(megaRTOS PUBLIC
    include
)

# Opciones de compilación para la librería
target_compile_options(megaRTOS PRIVATE
    -mmcu=${MCU}
    -Os
    -Wall
    -ffunction-sections
    -fdata-sections
)

# --- CONFIGURACIÓN DEL EJEMPLO (OPCIONAL) ---

# Esto crea el binario de prueba usando la librería recién creada
add_executable(demo_rtos examples/main_demo.cpp)

target_compile_options(demo_rtos PRIVATE
    -mmcu=${MCU}
    -Os
)

target_link_options(demo_rtos PRIVATE
    -mmcu=${MCU}
    -Wl,--gc-sections
)

# Enlazamos el ejemplo con tu librería megaRTOS
target_link_libraries(demo_rtos megaRTOS)
