CC_FILESIN      := $(wildcard src/*.cc)
ROOTCLASSES     = SampleDescriptor CompoundHistoUnfolding HistoUnfolding HistoUnfoldingTH1 HistoUnfoldingTH2 palettes
ROOTCLASSDICTS  = $(addprefix src/, $(ROOTCLASSES:=Dict.cc))
ROOTMAPS        = $(addprefix lib/, $(ROOTCLASSES:=.rootmap))
CC_FILES        = $(filter-out $(ROOTCLASSDICTS), $(CC_FILESIN))
DEP_FILES       = $(CC_FILES:src/%.cc=dep/%.d)
CC_FILES       += $(ROOTCLASSDICTS)
OBJ_FILES       = $(addprefix obj/, $(notdir $(CC_FILES:.cc=.o)))
CC_FLAGS       := -g
LIBRARY        := libCompoundHisto.so
#.1.0.0 
LIBRARYSL      := libCompoundHisto.so
#-L$(CFAT)/lib -lcfat -Wl,-rpath,$(CFAT)/lib
all:  lib/$(LIBRARYSL) $(ROOTMAPS)

lib/$(LIBRARYSL): lib/$(LIBRARY).1.0.0 
	if [[ -f $@ ]]; then rm $@; fi;
	ln -s $(LIBRARY).1.0.0 $@ 

lib/$(LIBRARY).1.0.0:  $(OBJ_FILES)
	gcc -shared -Wl,-soname,$(LIBRARY)  -g -fPIC -std=c++14 -o $@ $^ 

# obj/CompoundHistoUnfolding.o: interface/SampleDescriptor.hh
# 	@echo "hey"


$(OBJ_FILES): obj/%.o: src/%.cc dep/%.d 
	gcc `root-config --libs --cflags` $(CC_FLAGS) -std=c++14 -c -fPIC -o $@ $< -I interface -I $(ROOUNFOLD)/src -I${CFAT}/interface -L $(ROOUNFOLD) -lRooUnfold -L${CFAT}/lib -lcfat  -Wl,-rpath,$(CFAT)/lib



$(ROOTMAPS): lib/%.rootmap: interface/%.hh linkdef/%LinkDef.h
	rootcint -f src/$(@:lib/%.rootmap=%Dict.cc) -rmf $@ -c $< linkdef/$(@:lib/%.rootmap=%)LinkDef.h
	sed -i --  "s%#include "\""interface/"$(@:lib/%.rootmap=%)".hh%#include "\""/afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib/interface/"$(@:lib/%.rootmap=%)".hh%g" src/$(@:lib/%.rootmap=%Dict.cc)
	cp src/$(@:lib/%.rootmap=%Dict)_rdict.pcm lib	


$(ROOTCLASSDICTS): src/%Dict.cc: interface/%.hh linkdef/%LinkDef.h
	rootcint -f $@ -rmf lib/$(@:src/%Dict.cc=%).rootmap -c $< linkdef/$(@:src/%Dict.cc=%)LinkDef.h
	sed -i --  "s%#include "\""interface/"$(@:src/%Dict.cc=%)".hh%#include "\""/afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib/interface/"$(@:src/%Dict.cc=%)".hh%g" $@
	cp $(@:.cc=)_rdict.pcm lib	



$(DEP_FILES): dep/%.d: src/%.cc
	gcc `root-config --cflags` -I interface -I $(ROOUNFOLD)/src -I${CFAT}/interface -MM $^ -MT $(@:dep/%.d=obj/%.o) -o $@ 


include $(DEP_FILES)


.PHONY: clean

clean:
	echo "cleaning"
	for f in $(wildcard src/*Dict.cc); do rm $$f; done

	for f in $(wildcard src/*Dict_rdict.pcm); do rm $$f; done

	for f in $(wildcard obj/*); do rm $$f; done

	for f in $(wildcard lib/*); do rm $$f; done

	for f in $(wildcard interface/*~); do rm $$f; done

	for f in $(wildcard src/*~); do rm $$f; done

	for f in $(wildcard *.rootmap); do rm $$f; done

	for f in $(wildcard dep/*); do rm $$f; done





