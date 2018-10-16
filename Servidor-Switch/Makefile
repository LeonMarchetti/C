CC=gcc
CFLAGS=-Wall -pthread

INCLUDES=-I /usr/include/json-c -I /usr/include/mysql
LFLAGS=-L /usr/lib
LIBS=-l json-c -l mysqlclient

OBJ_DIR=./object/
TARGET_OBJ=${OBJ_DIR}*.o
RUN_DIR=./run/

PRJ_NOM=Servidor-Switch
SOURCE=${PRJ_NOM}.c
TARGET=${RUN_DIR}${PRJ_NOM}.exe

# Servidor
SRV=servidor-concurrente
SRV_SRC=${SRV}.c
SRV_OBJ=${OBJ_DIR}${SRV}.o

# Utilidades JSON
JSN=json-util
JSN_SRC=${JSN}.c
JSN_OBJ=${OBJ_DIR}${JSN}.o

# Módulo MySQL
MySQL=modulo-mysql
MySQL_SRC=${MySQL}.c
MySQL_OBJ=${OBJ_DIR}${MySQL}.o

all: ${TARGET}

${SRV_OBJ}: ${SRV_SRC}
	${CC} ${CFLAGS} ${INCLUDES}  -o ${SRV_OBJ} -c ${SRV_SRC} ${LFLAGS} ${LIBS}

${JSN_OBJ}: ${JSN_SRC}
	${CC} ${CFLAGS} ${INCLUDES}  -o ${JSN_OBJ} -c ${JSN_SRC} ${LFLAGS} ${LIBS}

${MySQL_OBJ}: ${MySQL_SRC}
	${CC} ${CFLAGS} ${INCLUDES}  -o ${MySQL_OBJ} -c ${MySQL_SRC} ${LFLAGS} ${LIBS}

${TARGET}: ${SOURCE} ${SRV_OBJ} ${JSN_OBJ} ${MySQL_OBJ}
	${CC} ${CFLAGS} ${INCLUDES} -o ${TARGET} ${SOURCE} ${SRV_OBJ} ${JSN_OBJ} ${MySQL_OBJ} ${LFLAGS} ${LIBS}

clean:
	${RM} ./object/*.o ./run/*.exe
