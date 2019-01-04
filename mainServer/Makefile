#*****************************************************************
# File:   Makefile_p5_mod
# Author: D. González
# Date:   diciembre 2018
# Coms:
#*****************************************************************

#---------------------------------------------------------
CC=g++

SERVER=servidor
SOCKET_DIR=Socket
SOCKET=${SOCKET_DIR}/Socket
SCTRL_DIR=ServerControl
SCTRL=${SCTRL_DIR}/ServerControl

CPPFLAGS=-I. -I${SOCKET_DIR} -I${SCTRL_DIR} -O2 -std=c++11 -lsockets # Flags compilacion
LDFLAGS=-pthread -lnsl# Flags linkado threads
SOCKETSFLAGS=-lsocket -lnsl

all:
	@echo " "
	@echo "    ## POSIBLES COMPILACIONES: ##"
	@echo "    #/ lcl: local              \#"
	@echo "    #| hdrx: hendrix           |#"
	@echo "    #| clean                   |#"
	@echo " "

#-----------------------------------------------------------
# SERVIDOR
# Compilacion

lcl: ${SOCKET}.o ${SCTRL}.o ${SERVER}.o
	${CC} ${LDFLAGS} ${SOCKET}.o ${SCTRL}.o ${SERVER}.o -o ${SERVER}

hdrx: ${SOCKET}.o ${SCTRL}.o ${SERVER}.o
	${CC} ${LDFLAGS} ${SOCKET}.o ${SCTRL}.o ${SERVER}.o -o ${SERVER} ${SOCKETSFLAGS}

${SERVER}.o: ${SERVER}.cpp
	${CC} -c ${CPPFLAGS} ${SERVER}.cpp

#-----------------------------------------------------------

# SCTRL
# Compilacion ServerControl
${SCTRL}.o: ${SCTRL}.hpp ${SCTRL}.cpp
	${CC} -c ${CPPFLAGS} ${SCTRL}.cpp -o ${SCTRL}.o

# SOCKETS
# Compilacion libreria de Sockets
${SOCKET}.o: ${SOCKET}.hpp ${SOCKET}.cpp
	${CC} -c ${CPPFLAGS} ${SOCKET}.cpp -o ${SOCKET}.o


#-----------------------------------------------------------
# LIMPIEZA
clean:
	$(RM) ${SOCKET}.o
	$(RM) ${SCTRL}.o
	$(RM) ${SERVER} ${SERVER}.o
