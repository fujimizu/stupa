CXX = g++
VERSION = 0.0.1
CXXFLAGS = -O3 -Wall
LDFLAGS = -L/usr/local/lib -L../stupa
INCLUDES = -I/usr/local/include -I/usr/local/include/stupa/ -I../stupa
LIBS =  -levent -lstupa
OBJ = handler.o stupa_evhttpd.o
TARGET = stupa_evhttpd
PACKAGE = stupa-evhttp-$(VERSION)

.cc.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<

all: $(TARGET)

$(TARGET) : $(OBJ) 
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(LDFLAGS) $(LIBS)

clean:
	rm -f *.o $(TARGET) core *~ *.tar.gz a.out gmon.out leak.log

dist:
	rm -fr $(PACKAGE)
	mkdir $(PACKAGE)
	cp -r *.h *.cc Makefile README COPYING perl $(PACKAGE)
	rm -rf $(PACKAGE)/perl/.svn; rm -rf $(PACKAGE)/perl/lib/.svn
	tar cvzf $(PACKAGE).tar.gz $(PACKAGE)
	rm -fr $(PACKAGE)

check:

handler.o : handler.h
