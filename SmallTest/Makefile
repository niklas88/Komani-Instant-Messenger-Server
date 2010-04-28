OS= $(shell echo `uname`)

ifeq ($(OS),Darwin)

	LDFLAGS+=-arch ppc  
	CXXFLAGS+= -Os  -MD -Wall -arch ppc 
endif
ifeq ($(OS),Linux)

	LDFLAGS+= -lpthread
	CXXFLAGS+=-Os -MD -Wall -g
endif

SRCS=main.cpp TCPNet.cpp mbuffer.cpp askpass.cpp Mutex.cpp
OBJS=$(patsubst %.cpp, %.o, $(SRCS))
DEPS= $(patsubst %.o,%.d,$(OBJS))

all: smalltest

smalltest: $(OBJS) Makefile 
	$(CXX) $(LDFLAGS) $(OBJS) -o $@

main.o: 	main.cpp Makefile
TCPNet.o:       TCPNet.cpp Makefile
mbuffer.o: 	mbuffer.cpp Makefile
askpass.o:      askpass.cpp Makefile
clean:
	rm *.o
	rm *.d


deps: $(DEPS)	

%.d: %.cpp
	$(CXX) $(CXXFLAGS) -E  $< > /dev/null

-include $(DEPS)
.PHONY: all clean install
