#include "log.h"

/** Estado ----------------------------------------------------------------- */

static size_t n_colors;
static pthread_mutex_t log_mutex;
static FILE* fh;

/** Declaraciones de funciones auxiliares ---------------------------------- */

void _set_thread_color(pthread_t tid);
void _reset_color();

/** Funciones Auxiliares --------------------------------------------------- */

void log_init() {

    pthread_mutex_init(&log_mutex, NULL);

    fh = fopen("log.txt", "w");

    n_colors = 0;
    while (log_colors[n_colors]) n_colors++;

}

void log(const char * format, ...) {
    pthread_mutex_lock(&log_mutex);

        pthread_t tid = pthread_self();

        va_list vargs; // Estos son los args que vienen en el `...`
        va_start(vargs, format);
        _set_thread_color(tid);
        fprintf(fh, "%lx: ", tid);
        _reset_color();
        vfprintf(fh, format, vargs);
        fprintf(fh, ".\n");
        va_end(vargs);

    pthread_mutex_unlock(&log_mutex);
}

/** Funciones Auxiliares --------------------------------------------------- */

void _set_thread_color(pthread_t tid) {
    size_t i = (size_t) tid % n_colors;
    fprintf(fh, "%s", log_colors[i]);
}

void _reset_color() {
    fprintf(fh, "%s", "\033[0m");
}
