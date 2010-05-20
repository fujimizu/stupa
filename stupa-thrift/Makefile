CXX = g++
VERSION = 0.1.1
CXXFLAGS = -O3 -Wall
LDFLAGS = -L/usr/local/lib -L../stupa
INCLUDES = -I/usr/local/include/thrift -I/usr/local/include/stupa/ -I../stupa
LIBS =  -lthrift -lstupa
LIBS_NB = -lthriftnb -levent
OBJ = Search.o Search_handler.o stupa_constants.o stupa_types.o
TARGET_THREAD = stupa_thread
TARGET_NONBLOCK = stupa_nonblock
PACKAGE = stupa-thrift-$(VERSION)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<

all: $(TARGET_THREAD) $(TARGET_NONBLOCK)

$(TARGET_THREAD) : $(OBJ) Search_thread.o
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) Search_thread.o $(LDFLAGS) $(LIBS)

$(TARGET_NONBLOCK) : $(OBJ) Search_nonblock.o
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) Search_nonblock.o $(LDFLAGS) $(LIBS) $(LIBS_NB)

clean:
	rm -f *.o $(TARGET_THREAD) $(TARGET_NONBLOCK) core *~ *.tar.gz a.out gmon.out leak.log

dist:
	rm -fr $(PACKAGE)
	mkdir $(PACKAGE)
	cp -r *.h *.cpp stupa.thrift Makefile README COPYING perl $(PACKAGE)
	rm -rf $(PACKAGE)/perl/.svn; rm -rf $(PACKAGE)/perl/lib/.svn
	tar cvzf $(PACKAGE).tar.gz $(PACKAGE)
	rm -fr $(PACKAGE)

check:

Search_handler.o : Search_handler.h
Search_thread.o : Search_handler.h
Search_nonblock.o : Search_handler.h
