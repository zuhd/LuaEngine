#**********************************************************
#
#	user design
#**********************************************************

# DEBUG can be set to YES to include debug info, or NO otherwise
DEBUG := YES
OUTPUT	:= /home/shaman/public/liblg_luaengine.so
DEFS	:= 
INCS	:= -I../../Include/BaseModule
LIBS	:= -L../../SrcLibs/BaseModule -llg_baselibs -llua -ldl -lpthread

#**********************************************************
CXX		:= g++
LD		:= g++
AR		:= ar rc
RANLIB	:= ranlib

DEBUG_CXXFLAGS		:=  -Wall -g -DDEBUG
RELEASE_CXXFLAGS	:= -Wall -O3 -DNDEBUG

DEBUG_LDFLAGS		:= -g
RELEASE_LDFLAGS		:=

ifeq (YES, $(DEBUG))
	CXXFLAGS		:= $(DEBUG_CXXFLAGS) $(DEFS)
	LDFLLAGS		:= $(DEBUG_LDFLAGS) $(DEFS)
else
	CXXFLAGS		:= $(RELEASE_CXXFLAGS) $(DEFS)
	LDFLLAGS		:= $(RELEASE_CSSFLAGS) $(DEFS)
endif

all:$(OUTPUT)

SRCS	:= $(wildcard *.cpp)

OBJS	:= $(addsuffix .o, $(basename $(SRCS)))

#**********************************************************
#	OUTPUT
#**********************************************************
include $(subst .cpp,.d,$(SRCS))

%.d : %.cpp
	$(CXX) -M $(CPPFLAGS) $(INCS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$;
	
$(OUTPUT):$(OBJS)
	$(LD) -o $@ -shared $(CXXFLAGS) $(OBJS) $(LIBS)
	
#**********************************************************
#	common rule
#**********************************************************

%.o : %.cpp
	$(CXX) -c -fPIC $(CXXFLAGS) $(INCS) $<

clean:
	rm -f $(OBJS) $(OUTPUT)