CPP = g++
CCFLAGS = -std=c++14
SRCDIR = src
BUILDDIR = build
BINDIR = bin
CPP_FILES := $(wildcard ${SRCDIR}/*.cpp)
OBJS := $(addprefix build/,$(notdir $(CPP_FILES:.cpp=.o)))

program: ${OBJS}
	${CPP} ${OBJS} -o ${BINDIR}/rolling_median


${BUILDDIR}/%.o: ${SRCDIR}/%.cpp
	${CPP} ${CCFLAGS} -c -o $@ $<
