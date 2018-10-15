CC=gcc
CFLAGS=-Wall

INCLUDES=-I /usr/include/json-c
LFLAGS=-L /usr/lib
LIBS=-l json-c

OBJ_DIR=./object/
RUN_DIR=./run/

PRJ_NOM=PruebaJSON
SOURCE=${PRJ_NOM}.c
TARGET=${RUN_DIR}${PRJ_NOM}.exe

all: ${TARGET}

${TARGET}: ${SOURCE}
	${CC} ${CFLAGS} ${INCLUDES} -o ${TARGET} ${SOURCE} ${LFLAGS} ${LIBS}

clean:
	${RM} ./object/*.o ./run/*.exe
