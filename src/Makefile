CPP = g++
CCFLAGS = -std=c++14 -O2
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
	${CPP} ${CCFLAGS} -c -o $@ $<

clean:
	rm -rf ${BUILDDIR}
	rm -rf ${BINDIR}
