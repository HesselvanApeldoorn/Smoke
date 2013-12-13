## Output exectable name
EXECFILE    = smoke

## Files of interest.
# Use the wildcard to grab all .cpp files. Once built, select the associated 
# .o object files. Similarly, once build, select the associated .d files.
# .d files are temporary files created to establish file dependency
OBJECTS 		= $(SOURCES:.cpp=.o)
DEPENDS 		= $(SOURCES:.cpp=.d)
SOURCES 		= $(wildcard *.cpp)
INCLUDEDIRS = -I./fftw-2.1.5/include/
LIBDIRS     = -L./fftw-2.1.5/lib/

## Linking flags, includes libraries used
LDFLAGS     = -lrfftw -lfftw -lglui 

#Possible flags for release (ffast-math uses less precision for floating-point numbers, check that your application can handle this)
#CFLAGS      = -O3 -march=x86-64 -mtune=generic -DNDEBUG -mfpmath=sse -ffast-math -Wall -pipe
#Debug flags
CFLAGS      = -Wall -g -pedantic
LINKFLAGS   =

## Compiler to be used
CXX					= g++

## Operating System name
UNAME := $(shell uname)

# Linux (default)
ifeq "$(UNAME)" "Linux"
	LDFLAGS += -lGL -lGLU -lglut 
endif

# OS X
ifeq "$(UNAME)" "Darwin"
	LDFLAGS += -framework GLUT -framework OpenGL
	CXX = clang++
endif

all: $(EXECFILE)


$(EXECFILE): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(CFLAGS) $(LDFLAGS) $(LIBDIRS) 

%.o: %.cpp
	$(CXX) -o $@ -c $(CFLAGS) $(INCLUDEDIRS) $<

## Determine dependencies for each .cpp files.
# -M 		Creates a dependency directed acyclic graph, used by Makefiles
%.d: %.cpp
	$(CXX) -M $(CFLAGS) $< > $@

clean:
		-rm -rf $(OBJECTS) $(EXECFILE) $(DEPENDS)

depend: $(DEPENDS)

include $(DEPENDS)


# OBJECTS     = fluids.o
# CFILES      = $(OBJECTS:.o=.cpp)
# EXECFILE    = smoke
# INCLUDEDIRS = -I./fftw-2.1.5/include/
# LIBDIRS     = -L./fftw-2.1.5/lib/
# LIBS        = -lGL 
# #Possible flags for release (ffast-math uses less precision for floating-point numbers, check that your application can handle this)
# #CFLAGS      = -O3 -march=x86-64 -mtune=generic -DNDEBUG -mfpmath=sse -ffast-math -Wall -pipe
# #Debug flags
# CFLAGS      = -ggdb -Wall -pipe -lm -lglut -lGLU  -lfftw -lrfftw
# LINKFLAGS   = 


# .SILENT:

# all: $(EXECFILE)

# $(EXECFILE): $(OBJECTS)
# 		g++ $(LINKFLAGS) $(OBJECTS) -o $(EXECFILE) $(LIBDIRS) $(LIBS)

# .cpp.o: $$@.cpp $$@.h
# 		g++ -c $(CFLAGS) $(INCLUDEDIRS) -cpp $<

# clean:
# 		-rm -rf $(OBJECTS) $(EXECFILE)

# depend:
# 		g++ -c -MM $(CFILES) > make.dep

# -include make.dep
