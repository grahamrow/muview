CXX           = g++
CXXFLAGS      = -m64 -pipe -O2 -Wall -W -D_REENTRANT -fPIE $(DEFINES)

OFILES = OMFHeader.o OMFImport.o

all: libomf.a

# Create the static library containing 
libomf.a: $(OFILES) 
	ar -cq libomf.a $(OFILES) 

clean:
	rm -f *.o *.a

####### Implicit rules

# .SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) -o "$@" "$<"
