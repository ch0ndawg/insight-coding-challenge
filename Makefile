CPP = g++
CCFLAGS = -std=c++14 -O2
SRCDIR = src
BUILDDIR = build
BINDIR = bin
CPP_FILES := $(wildcard ${SRCDIR}/*.cpp)
OBJS := $(addprefix build/,$(notdir $(CPP_FILES:.cpp=.o)))

program: ${OBJS} ${BINDIR}
	${CPP} ${OBJS} -o ${BINDIR}/rolling_median

${BINDIR}:
	mkdir bin

${BUILDDIR}:
	mkdir build

${BUILDDIR}/%.o: ${SRCDIR}/%.cpp ${BUILDDIR}
	${CPP} ${CCFLAGS} -c -o $@ $<

clean:
	rm -rf ${BUILDDIR}
	rm -rf ${BINDIR}
