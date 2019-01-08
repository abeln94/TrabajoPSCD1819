#!/bin/bash

set -e #Exit on first error

# prueba cliente local (sin servidor) con test supersencillo
echo -----Compilando cliente-----
gmake cliente
echo -----Ejecutando cliente-----
./build$(uname)/bin/cliente/cliente local 0 1 myexample .

