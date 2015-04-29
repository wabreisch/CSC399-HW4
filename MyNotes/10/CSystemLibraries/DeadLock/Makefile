CC=g++
CFLAGS=-Wall -g -c
LFLAGS=-Wall

EXECA=diningPhilosophers
OBJSA=$(EXECA).o

EXECS=$(EXECA)
OBJS=$(OBJSA)

SRC=${OBJS:.o=.c}
HEADERS=${OBJS:.o=.h}


all: $(EXECS)

$(EXECA): $(OBJSA)
	$(CC) $(LFLAGS) -o $(EXECA) $(OBJSA)

include $(OBJS:.o=.d)   # Include All Object Dependencies

%.o: %.c
	$(CC) $(CFLAGS) $*.c

%.o: %.C
	$(CC) $(CFLAGS) $*.C

clean:
	@echo Cleaning out directory
	-rm *.o *.d $(EXECS) *~

#=============================================================
#            Automatically create dependencies!!!
#=============================================================
# Notes on Notation:
#    $* == the target stem  - so $*.d replaces say foo.o with foo.d
#    $< == the frist prereq (dependency)
#    %  == used for pattern matching.  So
#          %.c matches say foo.c and creates the target foo.d
#          This one is written to generate dependencies for 3 endings
#    -MM = gcc option to make dependencies automatically
#          Does not include system headers.
#          -M will include all headers
#    -MT = gcc option to specify the target term to make
#          Here is makes TWO targets "foo.d foo.o"
#    -MF = gcc option to specify the file to save the generated target
#=============================================================

%.d: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -MM -MT $*.d -MT $*.o -MF $*.d $<

%.d: %.C
	$(CC) $(CFLAGS) $(INCLUDES) -MM -MT $*.d -MT $*.o -MF $*.d $<

%.d: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -MM -MT $*.d -MT $*.o -MF $*.d $<
