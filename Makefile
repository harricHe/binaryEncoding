#
# Copyright (C) 2011 yongs2 <http://github.com/yongs2/binaryEncoding>
#
#

CC = gcc
CXX = g++
LD = ld
RM = rm -rf
INSTALL = install -t

PROFILING_OPT = -pg 

CFLAGS			=	-g -Wall -D_REENTRANT
LDFLAGS			=	-lnsl 

INCDIR			= 	-I.

LIBDIR			=	-L.

TARGET_PATH		= ./

OBJECT_PREFIX	= ${HOME}/.objs
OBJECT_PATH		= ${OBJECT_PREFIX}/binaryEncoding
EXEC			= unittest

SRCS = testmain.c binaryEncoding.c

# Implict rules
.SUFFIXES: .cc .cpp .c
#.PHONY: clean force subdirs

# Ȯ���� ��Ģ (������ ���� ��Ģ)
.c.o :
	${CC} -c $< ${CFLAGS} ${INCDIR}
.cc.o :
	${CXX} -c $(CXXFLAGS} ${INCDIR} -o $@ $<

#all : ${EXEC}

#-------------------------------------
# ����
# <Target> : <Depend>
# <�ǹ���><Command>
# $@ ; Target�� �ǹ�
# $^ ; Depend ��ü�� �ǹ�
#-------------------------------------
#${EXEC} : ${SRCS}
#	${CC} -o $@ $^ ${CFLAGS} ${INCDIR} ${LDFLAGS} ${LIBDIR}

OBJS		= ${SRCS:%.c=${OBJECT_PATH}/%.o}
TEST_OBJS	= ${TEST_SRCS:%.c=${OBJECT_PATH}/%.o}

all : dir ${SUBDIRS} ${EXEC}

dir:
	if [ ! -f ${OBJECT_PATH} ] ; then mkdir -p ${OBJECT_PATH}; fi

${EXEC}: ${OBJS}
	${CC} -o ${EXEC} ${OBJS} ${LIBDIR} ${CFLAGS} ${LDFLAGS}

force:

${OBJECT_PATH}/%.o:%.c
	${CC} -c $< ${CFLAGS} ${INCDIR} -o $@

install: ${EXEC}
	${INSTALL} ${TARGET_PATH} -m 755 ${EXEC} ; \
	${RM} ${EXEC}

clean : 
	${RM} ${OBJS}
	${RM} ${EXEC} core
