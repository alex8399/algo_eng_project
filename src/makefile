CC = clang++
CFLAGS = -Iinc -O2 -std=c++20

TARGET = experiment.exe
BLD_DIR = bld
TARGET_DIR = ${BLD_DIR}
LIB_DIR = lib
INC_DIR = inc
OBJS = \
	${BLD_DIR}/ch_graph.o \
	${BLD_DIR}/experiment.o \
	${BLD_DIR}/file_facilities.o \
	${BLD_DIR}/timer.o
OBJ_MAIN = ${BLD_DIR}/main.o

TST_TARGET = test_experiment.exe
TST_BLD_DIR = bld_tst
TST_DIR = tst
TST_TARGET_DIR = ${TST_BLD_DIR}
TST_CFLAGS = -pthread -L/usr/lib -lgtest -lgtest_main
TST_OBJS = \
	${TST_BLD_DIR}/test_ch_graph.o \
 	${TST_BLD_DIR}/test_file_facilities.o \
 	${TST_BLD_DIR}/test_timer.o


clean:
	rm -rf ${BLD_DIR}/*.o ${TARGET_DIR}/*.exe ${TST_BLD_DIR}/*.o ${TST_TARGET_DIR}/*.exe tst/tmp/*.tmp

build: ${TARGET_DIR}/${TARGET} ${TST_BLD_DIR}/${TST_TARGET}

${TST_TARGET_DIR}/${TST_TARGET}: ${OBJS} ${TST_OBJS}
	${CC} ${CFLAGS} $^ ${TST_CFLAGS} -o $@

${TST_BLD_DIR}/%.o: ${TST_DIR}/%.cpp
	${CC} ${CFLAGS} -c $< -o $@

${TARGET_DIR}/${TARGET}: ${OBJS} ${OBJ_MAIN}
	${CC} ${CFLAGS} $^ -o $@

${BLD_DIR}/%.o: ${LIB_DIR}/%.cpp
	${CC} ${CFLAGS} -c $< -o $@

.PHONY: clean build