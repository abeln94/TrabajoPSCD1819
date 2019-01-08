#*****************************************************************
# File:   Makefile_p5_mod
# Author: D. González
# Date:   diciembre 2018
# Coms:
#*****************************************************************

#---------------------------------------------------------
CC=g++

SERVER=subserver
SOCKET_DIR=Socket
SOCKET=${SOCKET_DIR}/Socket
SBRD_DIR=Scoreboard
SBRD=${SBRD_DIR}/Scoreboard
TUPLA_DIR=Tuplas
TUPLA=${TUPLA_DIR}/Tuplas


CPPFLAGS=-I. -I${SOCKET_DIR} -I${SBRD_DIR} -I${TUPLA_DIR} -O2 -std=c++11 -lsockets # Flags compilacion
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

lcl: ${SOCKET}.o ${TUPLA}.o ${SBRD}.o ${SERVER}.o
	${CC} ${LDFLAGS} ${SOCKET}.o ${SCTRL}.o ${SERVER}.o -o ${SERVER}

hdrx: ${SOCKET}.o ${TUPLA}.o ${SBRD}.o ${SERVER}.o
	${CC} ${LDFLAGS} ${SOCKET}.o ${SCTRL}.o ${SERVER}.o -o ${SERVER} ${SOCKETSFLAGS}

${SERVER}.o: ${SERVER}.cpp
	${CC} -c ${CPPFLAGS} ${SERVER}.cpp

#-----------------------------------------------------------

# SBRD
# Compilacion Scoreboard
${SBRD}.o: ${TUPLA}.o ${SBRD}.hpp ${SBRD}.cpp
	${CC} -c ${CPPFLAGS} ${TUPLA}.o ${SBRD}.cpp -o ${SBRD}.o

#-----------------------------------------------------------
# TUPLA
# Compilacion Tuplas
${TUPLA}.o: ${TUPLA}.hpp ${TUPLA}.cpp
	${CC} -c ${CPPFLAGS} ${TUPLA}.cpp -o ${TUPLA}.o

# SOCKETS
# Compilacion libreria de Sockets
${SOCKET}.o: ${SOCKET}.hpp ${SOCKET}.cpp
	${CC} -c ${CPPFLAGS} ${SOCKET}.cpp -o ${SOCKET}.o


#-----------------------------------------------------------
# LIMPIEZA
clean:
	$(RM) ${SOCKET}.o
	$(RM) ${TUPLA}.o
	$(RM) ${SBRD}.o
	$(RM) ${SERVER} ${SERVER}.o
