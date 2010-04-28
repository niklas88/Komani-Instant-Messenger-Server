OS= $(shell echo `uname`)

ifeq ($(OS),Darwin)

	LDFLAGS=-L/sw/lib -lmysqlclient
	INCLUDEPATH=-I/sw/include -I.. 
	CXXFLAGS+=-O3 $(INCLUDEPATH)  -mtune=pentium-m -MD 
endif
ifeq ($(OS),Linux)

	LDFLAGS=-lmysqlclient -lpthread
	INCLUDEPATH=-I../../include
	CXXFLAGS+=-O3 $(INCLUDEPATH)  -MD
endif

SRCS=Database.cpp Query.cpp SysLog.cpp enum_t.cpp Mutex.cpp StderrLog.cpp  set_t.cpp 
OBJS=$(patsubst %.cpp, %.o, $(SRCS))
HDIR=../../include
DEPS= $(patsubst %.o,%.d,$(OBJS))

all: libdb.a

libdb.a: $(OBJS) Makefile 
	ar cq $@  $(OBJS) 
Query.o: 	Query.cpp Makefile 
Database.o: 	Database.cpp Makefile
SysLog.o: 	SysLog.cpp  Makefile
enum_t.o: 	enum_t.cpp Makefile
Mutex.o: 	Mutex.cpp Makefile
StderrLog.o: 	StderrLog.cpp Makefile
set_t.o: 	set_t.cpp Makefile

deps: $(DEPS)
	

%.d: %.cpp
	$(CXX) $(CXXFLAGS) -E  $< > /dev/null

-include $(DEPS)
.PHONY: all clean install
