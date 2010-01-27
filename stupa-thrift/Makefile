CXX = g++
VERSION = 0.0.1
CXXFLAGS = -O3 -Wall
LDFLAGS = -L/usr/local/lib -L/home/fujisawa/work/stupa
INCLUDES = -I/usr/local/include -I/usr/local/include/thrift -I/home/fujisawa/work/stupa
LIBS =  -lthrift -lstupa
LIBS_NB = -lthriftnb -levent
OBJ = Stupa.o stupa_constants.o stupa_types.o
TARGET_THREAD = stupa_thread
TARGET_NONBLOCK = stupa_nonblock
PACKAGE = stupa-thrift-$(VERSION)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<

all: $(TARGET_THREAD) $(TARGET_NONBLOCK)

$(TARGET_THREAD) : $(OBJ) Stupa_server_thread.o
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) Stupa_server_thread.o $(LDFLAGS) $(LIBS)

$(TARGET_NONBLOCK) : $(OBJ) Stupa_server_nonblock.o
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) Stupa_server_nonblock.o $(LDFLAGS) $(LIBS) $(LIBS_NB)

clean:
	rm -f *.o $(TARGET_THREAD) $(TARGET_NONBLOCK) core *~ *.tar.gz a.out gmon.out leak.log

dist:
	rm -fr $(PACKAGE)
	mkdir $(PACKAGE)
	cp -r *.h *.cpp stupa.thrift Makefile README COPYING perl $(PACKAGE)
	tar cvzf $(PACKAGE).tar.gz $(PACKAGE)
	rm -fr $(PACKAGE)

check:

Stupa_server_thread.o : Stupa_handler.h
Stupa_server_nonblock.o : Stupa_handler.h
