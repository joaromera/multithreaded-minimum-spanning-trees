#ifndef LOG_H
#define LOG_H

#include <pthread.h>
#include <stdio.h>
#include <stdarg.h>

/** Array de colores para los logs */
const char* const log_colors[] = {"\033[31;1m", "\033[32;1m", "\033[33;1m",
    "\033[34;1m", "\033[35;1m", "\033[36;1m", "\033[37;1m", NULL};

/** Inicializa estructuras de logging */
void log_init();

/** Función de logging */
void log(const char * format, ...);

#endif // LOG_H
