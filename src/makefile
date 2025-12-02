CC = clang++
CFLAGS = -I./inc -O2 -std=c++20
TARGET = experiment.exe
TARGET_DIR = bld
BLD_DIR = bld
LIB_DIR = lib
INC_DIR = inc
OBJS = \
	${BLD_DIR}/ch_graph.o \
	${BLD_DIR}/experiment.o \
	${BLD_DIR}/file_facilities.o \
	${BLD_DIR}/main.o \
	${BLD_DIR}/timer.o

clean:
	rm -rf ${BLD_DIR}/*.o ${TARGET_DIR}/*.exe

build: ${TARGET_DIR}/${TARGET}

${TARGET_DIR}/${TARGET}: ${OBJS}
	${CC} ${CFLAGS} $^ -o $@

${BLD_DIR}/%.o: ${LIB_DIR}/%.cpp
	${CC} ${CFLAGS} -c $< -o $@

.PHONY: clean build
