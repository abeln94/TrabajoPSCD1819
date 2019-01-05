#*****************************************************************
# File:   Makefile_p5_mod
# Author: D. González
# Date:   diciembre 2018
# Coms:
#*****************************************************************

#---------------------------------------------------------------------
###### AQUI VA EL NOMBRE DEL ARCHIVO DEL CLIENTE QUE IMPLEMENTARA
###### LA LIBRERIA LINDADRIVER
CLIENT=mainLindaDriver
#---------------------------------------------------------------------
CC=g++

LD_DIR="LindaDriver(API)"
LD=${LD_DIR}/LindaDriver

SOCKET_DIR="LindaDriver(API)"/Socket
SOCKET=${SOCKET_DIR}/Socket
TUPLA_DIR="LindaDriver(API)"/Tupla
TUPLA=${TUPLA_DIR}/Tupla

# Flags compilacion
CPPFLAGS=-I. -I${SOCKET_DIR} -I${TUPLA_DIR} -I${LD_DIR} -O2 -std=c++11 -lsockets
# Flags linkado threads
LDFLAGS=-pthread -lnsl
SOCKETSFLAGS=-lsocket -lnsl


all:
	@echo " "
	@echo "    ## POSIBLES COMPILACIONES: ##"
	@echo "    #/ lcl: local              \#"
	@echo "    #| hdrx: hendrix           |#"
	@echo "    #| clean                   |#"
	@echo " "

#-----------------------------------------------------------
# Cliente
# Compilacion

lcl: ${TUPLA}.o ${SOCKET}.o ${LD}.o ${CLIENT}.o
	${CC} ${LDFLAGS} ${TUPLA}.o ${SOCKET}.o ${LD}.o ${CLIENT}.o -o ${CLIENT}

hdrx: ${TUPLA}.o ${SOCKET}.o ${LD}.o ${CLIENT}.o
	${CC} ${LDFLAGS} ${TUPLA}.o ${SOCKET}.o ${LD}.o ${CLIENT}.o -o ${CLIENT} ${SOCKETSFLAGS}

${CLIENT}.o: ${CLIENT}.cpp
	${CC} -c ${CPPFLAGS} ${CLIENT}.cpp

#-----------------------------------------------------------
# Cliente
# Compilacion
${LD}.o: ${TUPLA}.o ${SOCKET}.o ${LD}.hpp ${LD}.cpp
	${CC} -c ${CPPFLAGS} ${TUPLA}.o ${SOCKET}.o ${LD}.cpp -o ${LD}.o

#-----------------------------------------------------------
# TUPLA
# Compilacion Tupla
${TUPLA}.o: ${TUPLA}.hpp ${TUPLA}.cpp
	${CC} -c ${CPPFLAGS} ${TUPLA}.cpp -o ${TUPLA}.o

# SOCKETS
# Compilacion libreria de Sockets
${SOCKET}.o: ${SOCKET}.hpp ${SOCKET}.cpp
	${CC} -c ${CPPFLAGS} ${SOCKET}.cpp -o ${SOCKET}.o


#-----------------------------------------------------------
#-----------------------------------------------------------
# LIMPIEZA
clean:
	$(RM) ${SOCKET}.o
	$(RM) ${TUPLA}.o
	$(RM) ${LD}.o
	$(RM) ${CLIENT} ${CLIENT}.o
