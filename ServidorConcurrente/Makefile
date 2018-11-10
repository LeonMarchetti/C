CC=gcc
CFLAGS=-Wall -pthread
DEBUG=-g

INCLUDES=-I /usr/include/json-c
LFLAGS=-L /usr/lib
LIBS=-l json-c

DEBUG_DIR=debug/
OBJ_DIR=object/
RUN_DIR=run/

PRJ_NOM=ServidorConcurrente
SOURCE=${PRJ_NOM}.c
TARGET=${RUN_DIR}${PRJ_NOM}.exe
DEBUG_TARGET=${DEBUG_DIR}${PRJ_NOM}.exe
OBJ_SRC=${SRV_OBJ} ${CLI_OBJ} ${SRV_OBJ}

# Cliente
CLI=cliente
CLI_SRC=${CLI}.c
CLI_OBJ=${OBJ_DIR}${CLI}.o

# Servidor
SRV=servidor
SRV_SRC=${SRV}.c
SRV_OBJ=${OBJ_DIR}${SRV}.o

all: ${RUN_DIR} ${OBJ_DIR} ${TARGET}

debug: ${DEBUG_DIR} ${OBJ_DIR} ${DEBUG_TARGET}

${DEBUG_DIR}:
	mkdir ${DEBUG_DIR}

${OBJ_DIR}:
	mkdir ${OBJ_DIR}

${RUN_DIR}:
	mkdir ${RUN_DIR}

${CLI_OBJ}: ${CLI_SRC}
	${CC} ${CFLAGS} ${INCLUDES} -o ${CLI_OBJ} -c ${CLI_SRC} ${LFLAGS} ${LIBS}

${SRV_OBJ}: ${SRV_SRC}
	${CC} ${CFLAGS} ${INCLUDES} -o ${SRV_OBJ} -c ${SRV_SRC} ${LFLAGS} ${LIBS}

${TARGET}: ${SOURCE} ${OBJ_SRC}
	${CC} ${CFLAGS} ${INCLUDES} -o ${TARGET} ${SOURCE} ${OBJ_SRC} ${LFLAGS} ${LIBS}

${DEBUG_TARGET}: ${SOURCE} ${OBJ_SRC}
	${CC} ${CFLAGS} ${DEBUG} ${INCLUDES} -o ${DEBUG_TARGET} ${SOURCE} ${OBJ_SRC} ${LFLAGS} ${LIBS}

clean:
	${RM} ./object/*.o ./run/*.exe
