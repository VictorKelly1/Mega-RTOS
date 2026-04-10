#ifndef RTOS_CONFIG_HPP
#define RTOS_CONFIG_HPP

//16MHz dafault 
#ifndef F_CPU
    #define F_CPU 16000000L
#endif

#define SYS_TICK_MS 1

#ifndef MAX_PROCESSES
    #define MAX_PROCESSES 7
#endif

#endif
