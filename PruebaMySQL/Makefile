CC=gcc
CFLAGS=-Wall
DEBUG=-g

INCLUDES=-I /usr/include/json-c -I /usr/include/mysql
LFLAGS=-L /usr/lib
LIBS=-l json-c -l mysqlclient

DEBUG_DIR=./debug/
OBJ_DIR=./object/
RUN_DIR=./run/

PRJ_NOM=PruebaMySQL
SOURCE=${PRJ_NOM}.c
TARGET=${RUN_DIR}${PRJ_NOM}.exe
DEBUG_TARGET=${DEBUG_DIR}${PRJ_NOM}.exe

# Utilidades JSON
JSON-UTIL=json-util
JSON-UTIL_SRC=${JSON-UTIL}.c
JSON-UTIL_OBJ=${OBJ_DIR}${JSON-UTIL}.o

all: ${TARGET}

debug: ${DEBUG_TARGET}

${JSON-UTIL_OBJ}: ${JSON-UTIL_SRC}
	${CC} ${CFLAGS} ${INCLUDES} -o ${JSON-UTIL_OBJ} -c ${JSON-UTIL_SRC} ${LFLAGS} ${LIBS}

${TARGET}: ${SOURCE} ${JSON-UTIL_OBJ}
	${CC} ${CFLAGS} ${INCLUDES} -o ${TARGET} ${SOURCE} ${JSON-UTIL_OBJ} ${LFLAGS} ${LIBS}

${DEBUG_TARGET}: ${SOURCE} ${JSON-UTIL_OBJ}
	${CC} ${CFLAGS} ${DEBUG} ${INCLUDES} -o ${DEBUG_TARGET} ${SOURCE} ${JSON-UTIL_OBJ} ${LFLAGS} ${LIBS}

clean:
	${RM} ./object/*.o ./run/*.exe
