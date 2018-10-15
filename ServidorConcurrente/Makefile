CC=gcc
CFLAGS=-Wall

INCLUDES=-I /usr/include/json-c
LFLAGS=-L /usr/lib
LIBS=-l json-c

OBJ_DIR=./object/
RUN_DIR=./run/

PRJ_NOM=ServidorConcurrente
SOURCE=${PRJ_NOM}.c
TARGET=${RUN_DIR}${PRJ_NOM}.exe

# Cliente
CLI=cliente
CLI_SRC=${CLI}.c
CLI_OBJ=${OBJ_DIR}${CLI}.o

# Servidor
SRV=servidor
SRV_SRC=${SRV}.c
SRV_OBJ=${OBJ_DIR}${SRV}.o

all: ${TARGET}

${CLI_OBJ}: ${CLI_SRC}
	${CC} ${CFLAGS} ${INCLUDES} -o ${CLI_OBJ} -c ${CLI_SRC} ${LFLAGS} ${LIBS}

${SRV_OBJ}: ${SRV_SRC}
	${CC} ${CFLAGS} ${INCLUDES} -o ${SRV_OBJ} -c ${SRV_SRC} ${LFLAGS} ${LIBS}

${TARGET}: ${SOURCE} ${CLI_OBJ} ${SRV_OBJ}
	${CC} ${CFLAGS} ${INCLUDES} -o ${TARGET} ${SOURCE} ${SRV_OBJ} ${CLI_OBJ} ${LFLAGS} ${LIBS}

clean:
	${RM} ./object/*.o ./run/*.exe
