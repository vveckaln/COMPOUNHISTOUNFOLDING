CC_FILES       := $(wildcard src/*.cc)
ROOTCLASSES     = SampleDescriptor CompoundHisto CompoundHistoUnfolding HistoUnfolding
ROOTCLASSDICTS  = $(addprefix src/, $(ROOTCLASSES:=Dict.cc))
CC_FILES       += $(ROOTCLASSDICTS)
OBJ_FILES      := $(addprefix obj/, $(notdir $(CC_FILES:.cc=.o)))
CC_FLAGS       := -g
LIBRARY        := libCompoundHisto.so
#.1.0.0 
LIBRARYSL      := libCompoundHisto.so
all:  lib/$(LIBRARYSL)

lib/$(LIBRARYSL): lib/$(LIBRARY).1.0.0 
	if [[ -f $@ ]]; then rm $@; fi;
	ln -s $(LIBRARY).1.0.0 $@ 

lib/$(LIBRARY).1.0.0:  $(OBJ_FILES)
	gcc -shared -Wl,-soname,$(LIBRARY) -g -fPIC -o $@ $^ 

obj/%.o: src/%.cc 
	gcc `root-config --libs --cflags` $(CC_FLAGS) -c -fPIC -o $@ $^ -I interface


$(CC_FILES): $(ROOTCLASSES)
	

$(foreach ROOTCLASS, $(ROOTCLASSES), $(ROOTCLASS)):
	rootcint -f src/$@Dict.cc -c interface/$@.hh $@LinkDef.h
	sed -i --  "s%#include "\""interface/$@.hh%#include "\""/afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib/interface/$@.hh%g" src/$@Dict.cc
	cp src/$@Dict_rdict.pcm lib	



.PHONY: clean

clean:

	for f in $(wildcard src/*Dict.cc); do rm $$f; done

	for f in $(wildcard src/*Dict_rdict.pcm); do rm $$f; done

	for f in $(wildcard obj/*); do rm $$f; done

	for f in $(wildcard lib/*); do rm $$f; done

	for f in $(wildcard interface/*~); do rm $$f; done

	for f in $(wildcard src/*~); do rm $$f; done
