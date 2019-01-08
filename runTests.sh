#!/bin/bash

set -e #Exit on first error

# prueba cliente local (sin servidor) con test supersencillo
echo -----Compilando cliente-----
make cliente
echo -----Ejecutando cliente-----
./build${UNAME}/bin/cliente/cliente local 0 1 myexample .

