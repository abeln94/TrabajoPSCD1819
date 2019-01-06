#*****************************************************************
# File:   Makefile_p5_mod
# Author: Abel Naya
# Date:   diciembre 2018
# Coms:
#*****************************************************************

#-----------------------------------------------------------
# detectar sistema en el que se ejecuta
# SOLO FUNCIONA CON gmake!!!!

UNAME := ${shell uname}
# Unexpected end of line? Ejecuta con 'gmake'!!!!

#---------------------------------------------------------------------
### directorios

SRC_DIR=src

BIBLIOTECAS_DIR=bibliotecas

LINDADRIVER_DIR=${BIBLIOTECAS_DIR}/LindaDriver
LINDADRIVER=${LINDADRIVER_DIR}/LindaDriver

SCOREBOARD_DIR=${BIBLIOTECAS_DIR}/Scoreboard
SCOREBOARD=${SCOREBOARD_DIR}/Scoreboard

SOCKET_DIR=${BIBLIOTECAS_DIR}/Socket
SOCKET=${SOCKET_DIR}/Socket

TUPLA_DIR=${BIBLIOTECAS_DIR}/Tupla
TUPLA=${TUPLA_DIR}/Tupla


CLIENTE_DIR=cliente
CLIENTE=${CLIENTE_DIR}/cliente
CLIENTE_EXTRA=${CLIENTE_DIR}/tests

SERVER_DIR=server
SERVER=${SERVER_DIR}/server
SERVER_EXTRA_DIR=${SERVER_DIR}/ServerControl
SERVER_EXTRA=${SERVER_EXTRA_DIR}/ServerControl

SUBSERVER_DIR=subserver
SUBSERVER=${SUBSERVER_DIR}/subserver


GENERATED_DIR=build${UNAME}
O_DIR=${GENERATED_DIR}/object
BIN_DIR=${GENERATED_DIR}/bin

#---------------------------------------------------------------------
## FLAGS
CC=g++

# Flags compilacion
CPPFLAGS=-I. -I${SRC_DIR}/${LINDADRIVER_DIR} -I${SRC_DIR}/${SCOREBOARD_DIR} -I${SRC_DIR}/${SOCKET_DIR} -I${SRC_DIR}/${TUPLA_DIR} -I${SRC_DIR}/${SERVER_EXTRA_DIR} -O2 -std=c++11 -lsockets -D ALLOW_LOCAL
# Flags linkado threads
LDFLAGS=-pthread -lnsl


ifeq (${UNAME},SunOS)
# flags hendrix
SOCKETSFLAGS=-lsocket -lnsl
endif

#-----------------------------------------------------------
# tareas

all: cliente server subserver

#-----------------------------------------------------------
# Compilacion cliente

cliente: ${O_DIR}/${LINDADRIVER}.o ${O_DIR}/${SCOREBOARD}.o ${O_DIR}/${SOCKET}.o ${O_DIR}/${TUPLA}.o ${O_DIR}/${CLIENTE}.o
	mkdir -p ${BIN_DIR}/${CLIENTE_DIR}
	${CC} ${LDFLAGS} ${O_DIR}/${LINDADRIVER}.o ${O_DIR}/${SCOREBOARD}.o ${O_DIR}/${SOCKET}.o ${O_DIR}/${TUPLA}.o ${O_DIR}/${CLIENTE}.o -o ${BIN_DIR}/${CLIENTE} ${SOCKETSFLAGS}
	
${O_DIR}/${CLIENTE}.o: ${SRC_DIR}/${CLIENTE}.cpp ${SRC_DIR}/${CLIENTE_EXTRA}.cpp
	mkdir -p ${O_DIR}/${CLIENTE_DIR}
	${CC} -c ${CPPFLAGS} ${SRC_DIR}/${CLIENTE}.cpp -o ${O_DIR}/${CLIENTE}.o
	
#-----------------------------------------------------------
# Compilacion servidor

server: ${O_DIR}/${LINDADRIVER}.o ${O_DIR}/${SCOREBOARD}.o ${O_DIR}/${SOCKET}.o ${O_DIR}/${TUPLA}.o ${O_DIR}/${SERVER}.o ${O_DIR}/${SERVER_EXTRA}.o
	mkdir -p ${BIN_DIR}/${SERVER_DIR}
	${CC} ${LDFLAGS} ${O_DIR}/${LINDADRIVER}.o ${O_DIR}/${SCOREBOARD}.o ${O_DIR}/${SOCKET}.o ${O_DIR}/${TUPLA}.o ${O_DIR}/${SERVER}.o ${O_DIR}/${SERVER_EXTRA}.o -o ${BIN_DIR}/${SERVER} ${SOCKETSFLAGS}
	
${O_DIR}/${SERVER}.o: ${SRC_DIR}/${SERVER}.cpp
	mkdir -p ${O_DIR}/${SERVER_DIR}
	${CC} -c ${CPPFLAGS} ${SRC_DIR}/${SERVER}.cpp -o ${O_DIR}/${SERVER}.o
	
${O_DIR}/${SERVER_EXTRA}.o: ${SRC_DIR}/${SERVER_EXTRA}.cpp ${SRC_DIR}/${SERVER_EXTRA}.hpp
	mkdir -p ${O_DIR}/${SERVER_EXTRA_DIR}
	${CC} -c ${CPPFLAGS} ${SRC_DIR}/${SERVER_EXTRA}.cpp -o ${O_DIR}/${SERVER_EXTRA}.o
	
#-----------------------------------------------------------
# Compilacion subservidor

subserver: ${O_DIR}/${LINDADRIVER}.o ${O_DIR}/${SCOREBOARD}.o ${O_DIR}/${SOCKET}.o ${O_DIR}/${TUPLA}.o ${O_DIR}/${SUBSERVER}.o
	mkdir -p ${BIN_DIR}/${SUBSERVER_DIR}
	${CC} ${LDFLAGS} ${O_DIR}/${LINDADRIVER}.o ${O_DIR}/${SCOREBOARD}.o ${O_DIR}/${SOCKET}.o ${O_DIR}/${TUPLA}.o ${O_DIR}/${SUBSERVER}.o -o ${BIN_DIR}/${SUBSERVER} ${SOCKETSFLAGS}
	
${O_DIR}/${SUBSERVER}.o: ${SRC_DIR}/${SUBSERVER}.cpp
	mkdir -p ${O_DIR}/${SUBSERVER_DIR}
	${CC} -c ${CPPFLAGS} ${SRC_DIR}/${SUBSERVER}.cpp -o ${O_DIR}/${SUBSERVER}.o
	
#-----------------------------------------------------------
# compilacion LindaDriver
${O_DIR}/${LINDADRIVER}.o: ${SRC_DIR}/${LINDADRIVER}.cpp ${SRC_DIR}/${LINDADRIVER}.hpp
	mkdir -p ${O_DIR}/${LINDADRIVER_DIR}
	${CC} -c ${CPPFLAGS} ${SRC_DIR}/${LINDADRIVER}.cpp -o ${O_DIR}/${LINDADRIVER}.o

#-----------------------------------------------------------
# compilacion Scoreboard
${O_DIR}/${SCOREBOARD}.o: ${SRC_DIR}/${SCOREBOARD}.cpp ${SRC_DIR}/${SCOREBOARD}.hpp
	mkdir -p ${O_DIR}/${SCOREBOARD_DIR}
	${CC} -c ${CPPFLAGS} ${SRC_DIR}/${SCOREBOARD}.cpp -o ${O_DIR}/${SCOREBOARD}.o

#-----------------------------------------------------------
# compilacion Socket
${O_DIR}/${SOCKET}.o: ${SRC_DIR}/${SOCKET}.cpp ${SRC_DIR}/${SOCKET}.hpp
	mkdir -p ${O_DIR}/${SOCKET_DIR}
	${CC} -c ${CPPFLAGS} ${SRC_DIR}/${SOCKET}.cpp -o ${O_DIR}/${SOCKET}.o

#-----------------------------------------------------------
# compilacion Tupla
${O_DIR}/${TUPLA}.o: ${SRC_DIR}/${TUPLA}.cpp ${SRC_DIR}/${TUPLA}.hpp
	mkdir -p ${O_DIR}/${TUPLA_DIR}
	${CC} -c ${CPPFLAGS} ${SRC_DIR}/${TUPLA}.cpp -o ${O_DIR}/${TUPLA}.o

#-----------------------------------------------------------
#-----------------------------------------------------------
# LIMPIEZA
clean:
	$(RM) -r ${GENERATED_DIR}
