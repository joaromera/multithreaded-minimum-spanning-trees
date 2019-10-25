#! /bin/sh

# Corre test -e y genera un CSV con los resultados y mediciones. El script es
# horrible, pero da la solución mínima al problema. Total ya re fue, YOLO.
#
# USO: ./benchmark.sh

run() {
    echo "peso,instancia,tipo,threads,time"
    ./TP1 -e | grep 'ralo\|completo\|arbol' | sed 'N;s/\n/,/' | sed 's/^Peso del arbol: //'
    #          ^lineas relevantes            ^junta cada 2 lineas
}

# make clean && make -j
rm -f experimentación.csv
run | tee experimentación.csv
