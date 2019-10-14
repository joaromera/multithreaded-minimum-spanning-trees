#! /usr/bin/env bash

set -u

readonly PROGRAM="./TP1"
readonly DIR_CASOS_TEST="test/correctitud"

# Corre el programa del TP para un caso de test dado.
correr() {
    local readonly NOMBRE_TEST="$1"
    local readonly N_THREADS="$2"
    "$PROGRAM" "${DIR_CASOS_TEST}/${NOMBRE_TEST}.txt" sarasa "$N_THREADS" \
        | grep -v '^\s'
}

get_correct_answer() {
    local readonly NOMBRE_TEST="$1"
    cat "${DIR_CASOS_TEST}/${NOMBRE_TEST}.ans" \
        | grep -v '^\s'
}

correr_diff() {
    local readonly NOMBRE_TEST="$1"
    local readonly N_THREADS="$2"

    echo "corriendo $RANDOM"
    correr "$NOMBRE_TEST" "$N_THREADS" 1>nuestro.txt 2> nuestro.log
    get_correct_answer "$NOMBRE_TEST" 1>suyo.txt

    diff <( correr "$NOMBRE_TEST" "$N_THREADS" ) \
         <( get_correct_answer "$NOMBRE_TEST" ) \
         > /dev/null
}

correr_test() {
    local readonly NOMBRE_TEST="$1"
    local readonly N_THREADS="$2"
    if ! correr_diff "$NOMBRE_TEST" "$N_THREADS"; then
        echo "Falló test $NOMBRE_TEST, para $N_THREADS threads"
        exit 1
    else
        echo "Test OK para $NOMBRE_TEST con $N_THREADS"
    fi
}

make clean && make -j
#correr_test "k3" 1
#correr_test "otro_test" 1
#for i in {1..100}; do correr_test "k3"        2; done
#for i in {1..100}; do correr_test "otro_test" 2; done
# for i in {1..1000}; do correr_test "k3"        3; done
# for i in {1..1000}; do correr_test "otro_test" 3; done
for i in {1..3000}; do correr_test "k3"        4; done
for i in {1..3000}; do correr_test "otro_test" 4; done
