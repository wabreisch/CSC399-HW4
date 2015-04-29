CC=gcc
CFLAGS=-Wall -g -c
LFLAGS=-Wall

EXECA=modDate
OBJSA=$(EXECA).o

EXECB=splitFork
OBJSB=$(EXECB).o

EXECC=forkSharing
OBJSC=$(EXECC).o

EXECD=forkPipe
OBJSD=$(EXECD).o

EXECE=dontInterruptMe
OBJSE=${EXECE}.o

EXECF=dupExample
OBJSF=${EXECF}.o

EXECG=dupBiggerExample
OBJSG=${EXECG}.o

EXECH=forkPipeTwo
OBJSH=$(EXECH).o

EXECS=$(EXECA) $(EXECB) $(EXECC) $(EXECD) $(EXECE) $(EXECF) $(EXECG) $(EXECH)
OBJS=$(OBJSA) $(OBJSB) $(OBJSC) $(OBJSD) $(OBJSE) $(OBJSF) $(OBJSG) $(OBJSH)

SRC=${OBJS:.o=.c}
HEADERS=${OBJS:.o=.h}


all: $(EXECS)

$(EXECA): $(OBJSA)
	$(CC) $(LFLAGS) -o $(EXECA) $(OBJSA)

$(EXECB): $(OBJSB)
	$(CC) $(LFLAGS) -o $(EXECB) $(OBJSB)

$(EXECC): $(OBJSC)
	$(CC) $(LFLAGS) -o $(EXECC) $(OBJSC)

$(EXECD): $(OBJSD)
	$(CC) $(LFLAGS) -o $(EXECD) $(OBJSD)

$(EXECE): $(OBJSE)
	$(CC) $(LFLAGS) -o $(EXECE) $(OBJSE)

$(EXECF): $(OBJSF)
	$(CC) $(LFLAGS) -o $(EXECF) $(OBJSF)

$(EXECG): $(OBJSG)
	$(CC) $(LFLAGS) -o $(EXECG) $(OBJSG)

$(EXECH): $(OBJSH)
	$(CC) $(LFLAGS) -o $(EXECH) $(OBJSH)

include $(OBJS:.o=.d)   # Include All Object Dependencies

%.o: %.c
	$(CC) $(CFLAGS) $*.c

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
