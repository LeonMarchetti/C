CC=gcc
CFLAGS=-Wall

INCLUDES=-I /usr/include/json-c -I /usr/include/postgresql
LFLAGS=-L /usr/lib
LIBS=-l json-c -l pq

OBJ_DIR=./object/
RUN_DIR=./run/

PRJ_NOM=PruebaPostgres
SOURCE=${PRJ_NOM}.c
TARGET=${RUN_DIR}${PRJ_NOM}.exe

JSON-UTIL=json-util
JSON-UTIL_SRC=${JSON-UTIL}.c
JSON-UTIL_OBJ=${OBJ_DIR}${JSON-UTIL}.o

all: ${TARGET}

${JSON-UTIL_OBJ}: ${JSON-UTIL_SRC}
	${CC} ${CFLAGS} ${INCLUDES} -o ${JSON-UTIL_OBJ} -c ${JSON-UTIL_SRC} ${LFLAGS} ${LIBS}

${TARGET}: ${SOURCE} ${JSON-UTIL_OBJ}
	${CC} ${CFLAGS} ${INCLUDES} -o ${TARGET} ${SOURCE} ${JSON-UTIL_OBJ} ${LFLAGS} ${LIBS}

clean:
	${RM} ./object/*.o ./run/*.exe
