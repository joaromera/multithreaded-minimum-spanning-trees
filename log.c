#include "log.h"

/** Estado ----------------------------------------------------------------- */

static size_t n_colors;
static pthread_mutex_t log_mutex;

/** Declaraciones de funciones auxiliares ---------------------------------- */

void _set_thread_color(pthread_t tid);
void _reset_color();

/** Funciones Auxiliares --------------------------------------------------- */

void log_init() {

    pthread_mutex_init(&log_mutex, NULL);

    n_colors = 0;
    while (log_colors[n_colors]) n_colors++;

}


void log(const char * format, ...) {
    pthread_mutex_lock(&log_mutex);

        pthread_t tid = pthread_self();

        va_list vargs; // Estos son los args que vienen en el `...`
        va_start(vargs, format);
        _set_thread_color(tid);
        fprintf(stderr, "%lu: ", tid);
        _reset_color();
        vfprintf(stderr, format, vargs);
        fprintf(stderr, ".\n");
        va_end(vargs);

    pthread_mutex_unlock(&log_mutex);
}

/** Funciones Auxiliares --------------------------------------------------- */

void _set_thread_color(pthread_t tid) {
    size_t i = tid % n_colors;
    fprintf(stderr, "%s", log_colors[i]);
}

void _reset_color() {
    fprintf(stderr, "%s", "\033[0m");
}
