# CPP = g++
# CFLAGS = -std=c++14 -O2
# OPTS = -O2
SRCDIR = .
BUILDDIR = ../build
BINDIR = ../bin
CPP_FILES := $(wildcard ${SRCDIR}/*.cpp)
OBJS := $(addprefix ${BUILDDIR}/,$(notdir $(CPP_FILES:.cpp=.o)))

program: ${OBJS} ${BINDIR} ${BUILDDIR}
	${CPP} ${OBJS} -o ${BINDIR}/rolling_median

${BINDIR}:
	mkdir ${BINDIR}

${BUILDDIR}:
	mkdir ${BUILDDIR}

${BUILDDIR}/%.o: ${SRCDIR}/%.cpp ${BUILDDIR}
	${CPP} ${CFLAGS} ${OPTS} -c -o $@ $<

clean:
	rm -rf ${BUILDDIR}
	rm -rf ${BINDIR}
