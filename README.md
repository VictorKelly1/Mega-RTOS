# 🧠 megaRTOS v1.0.0

megaRTOS is a lightweight, open source, preemptive Real-Time Operating System (RTOS) designed specifically for the ATmega328P (Arduino Uno). Built from the ground up in C++17 and AVR Assembly, it provides a robust multitasking environment for embedded developers who need concurrency, synchronization, and efficient I/O without the overhead of larger OSs.

# 🚀 Key Features

1- Preemptive Multitasking: Round-robin scheduling driven by hardware timer interrupts.

2- Context Switching in ASM: Optimized context saving/restoring for the AVR architecture.

3- Interrupt-Driven Blocking I/O: A UART driver that suspends tasks while waiting for data, maximizing CPU efficiency.

4- Resource Synchronization: Built-in Mutex support to prevent race conditions on shared peripherals.

5- Static Library Distribution: Optimized for easy integration as a precompiled .a file.

# 📂 Project Structure
```text
.
├── include/           # Public Header files (The API)
│   ├── kernel/        # Kernel, Mutex, and Process headers
│   ├── drivers/       # Hardware abstraction (UART)
│   └── utils/         # Generic data structures (Circular Buffer)
├── lib/               # Precompiled static library (libmegaRTOS.a)
├── src/               # RTOS Source code
├── examples/          # Demo applications
└── CMakeLists.txt     # Library build system
```

# 🛠️ Getting Started (Using the Precompiled Library)
If you only need the implementation...

There is a Realease. v1.0.0 
📥 How to Download and Install
Option 1: Direct Download (Recommended for Users)
Go to the Releases page of this repository.

Under Assets, download the libmegaRTOS.a file.

Download the Source code (zip) to get the necessary header files.

Extract the headers from the include/ folder and place them in your project.

Option 2: Via Terminal (Recommended for Developers)
If you want to have the full environment and examples, clone the repository:

```bash
git clone https://github.com/tu-usuario/RTOS.git
cd RTOS
```
If you want all the code, you don't need to compile the entire RTOS kernel to use it. You can link your project against the precompiled libmegaRTOS.a.

# 1. Requirements
-avr-gcc toolchain
-avrdude (for flashing)

# 2. Manual Compilation
To compile your own main.cpp using the megaRTOS library, run:

```Bash

#Compile and link
avr-g++ -mmcu=atmega328p -Os -I./include main.cpp -L./lib -lmegaRTOS -o firmware.elf
#Generate HEX file
avr-objcopy -O ihex firmware.elf firmware.hex
#Flash to Arduino Uno
avrdude -c arduino -p m328p -P /dev/ttyUSB0 -b 115200 -U flash:w:firmware.hex
```
# 🐞 Integrating with Microchip Studio
Follow these steps to link the precompiled megaRTOS library to your project:

1. File Setup
-Place the library files in your project directory as follows:
-Copy libmegaRTOS.a into a /lib folder.
-Copy all .h files into an /include folder.

2. Header Configuration
-Right-click your project in Solution Explorer → Properties.
-Navigate to Toolchain → AVR/GNU Compiler → Directories.
-Add the path to your /include folder.

3. Linker Configuration
-In the same Properties window, go to Toolchain → AVR/GNU Linker → Libraries:
-Libraries (-l): Add megaRTOS (do not include the lib prefix or .a extension).
-Library search path (-L): Add the path to your /lib folder.

4. Code Implementation
-Ensure your project is set to ATmega328P and the F_CPU matches your hardware (typically 16000000L).
-Was compiled for the avr5 architecture.

# 📋 Basic Usage Example in:
├── examples/          # Demo applications

# ⚙️ Technical Specifications

-Architecture	AVR 8-bit (ATmega328P)
-Language	C++17 / AVR ASM
-Default Frequency	16 MHz (You can modify)
-Task Stack Size	128 Bytes (Default)
-Scheduling	Preemptive Round-Robin
-Binary Size	~2-4 KB (depending on usage)

# 🏗️ Building the Library from Source
If you wish to modify the kernel and rebuild the static library you can:
```bash
mkdir build && cd build
cmake ..
make
```
# 👨‍💻 Author
Victor Computer Science Egineer Specializing in Embedded Systems and Low-Level C++ Development.

# ▶️ Link to example video here
https://www.youtube.com/watch?v=oiDaVDYx4aQ
# 📑 Documentation soon... 

