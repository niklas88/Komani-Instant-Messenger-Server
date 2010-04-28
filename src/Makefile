OS= $(shell echo `uname`)

ifeq ($(OS),Darwin)

	LDFLAGS=-L/sw/lib -lmysqlclient
	INCLUDEPATH=-I/sw/include
	CXXFLAGS+=-O3 $(INCLUDEPATH) -mtune=pentium-m -MD -Wall 
endif
ifeq ($(OS),Linux)

	LDFLAGS=-lmysqlclient -lpthread
	INCLUDEPATH=
	CXXFLAGS+=-O3 $(INCLUDEPATH) -MD -Wall
endif

SRCS=mbuffer.cpp TCPNet.cpp str.cpp main.cpp  user.cpp 
OBJS=$(patsubst %.cpp, %.o, $(SRCS)) 
DEPS= $(patsubst %.o,%.d,$(OBJS))

all: kimd libdb.a 

clean:
	rm *.d
	rm *.o
	rm libmysql++/*.o
	rm libmysql++/*.d
	rm libmysql++/*.a

kimd: $(OBJS) libdb.a Makefile  
	$(CXX) $(LDFLAGS) $(OBJS) ./libmysql++/libdb.a -o $@

TCPNet.o:	TCPNet.cpp Makefile
user.o: 	user.cpp Makefile
main.o: 	main.cpp Makefile
mbuffer.o: 	mbuffer.cpp Makefile
str.o: 		str.cpp Makefile
libdb.a: 
	make -C ./libmysql++/ 

deps: $(DEPS)
	

#%.d: %.cpp
#	$(CXX) $(CXXFLAGS) -E  $< > /dev/null

-include $(DEPS)
.PHONY: all clean install libdb
