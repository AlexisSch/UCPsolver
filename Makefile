




#-----------------------------------------------------------------------------
# paths
#-----------------------------------------------------------------------------

SCIPDIR         =       /home/schlegel/scipoptsuite-7.0.2/scip




#-----------------------------------------------------------------------------
# Main Program
#-----------------------------------------------------------------------------

MAINNAME	=	ucp.exe


# Add all the objects here
MAINOBJ	=	main.o \
			\
			DataClasses/InstanceUCP.o \
			DataClasses/ProductionPlan.o \
			\
			OtherResolution/FormulationCompact.o \
			OtherResolution/FormulationLinearRelaxation.o \
			\
			Decomposition/FormulationMaster.o \
			Decomposition/FormulationPricer.o \
			Decomposition/VariableMaster.o \
			\
			UnitDecomposition/FormulationMasterUnitDecomposition.o \
			UnitDecomposition/FormulationPricerUnitDecomposition.o \
			UnitDecomposition/ObjPricerUCPUnitDecomposition.o \
			\
			UnitDecomposition2/FormulationMasterUnitDecomposition2.o \
			UnitDecomposition2/FormulationPricerUnitDecomposition2.o \
			UnitDecomposition2/ObjPricerUCPUnitDecomposition2.o \
			\
			UnitDecomposition3/FormulationMasterUnitDecomposition3.o \
			UnitDecomposition3/FormulationPricerUnitDecomposition3.o \
			UnitDecomposition3/ObjPricerUCPUnitDecomposition3.o

									
MAINSRC	=	$(addprefix $(SRCDIR)/,$(MAINOBJ:.o=.cpp))

MAIN		=	$(MAINNAME).$(BASE).$(LPS)$(EXEEXTENSION)
MAINFILE	=	$(BINDIR)/$(MAIN)
MAINSHORTLINK	=	$(BINDIR)/$(MAINNAME)
MAINOBJFILES	=	$(addprefix $(OBJDIR)/,$(MAINOBJ))


#-----------------------------------------------------------------------------
# include default project Makefile from SCIP (need to do this twice, once to
# find the correct binary, then, after getting the correct flags from the
# binary (which is necessary since the ZIMPL flags differ from the default
# if compiled with the SCIP Optsuite instead of SCIP), we need to set the
# compile flags, e.g., for the ZIMPL library, which is again done in make.project
#-----------------------------------------------------------------------------
include $(SCIPDIR)/make/make.project
SCIPVERSION				:=$(shell $(SCIPDIR)/bin/scip.$(BASE).$(LPS).$(TPI)$(EXEEXTENSION) -v | sed -e 's/$$/@/')
override ARCH			:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* ARCH=\([^@]*\).*/\1/')
override EXPRINT		:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* EXPRINT=\([^@]*\).*/\1/')
override GAMS			:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* GAMS=\([^@]*\).*/\1/')
override GMP			:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* GMP=\([^@]*\).*/\1/')
override SYM			:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* SYM=\([^@]*\).*/\1/')
override IPOPT			:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* IPOPT=\([^@]*\).*/\1/')
override IPOPTOPT		:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* IPOPTOPT=\([^@]*\).*/\1/')
override LPSCHECK		:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* LPSCHECK=\([^@]*\).*/\1/')
override LPSOPT 		:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* LPSOPT=\([^@]*\).*/\1/')
override NOBLKBUFMEM	:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* NOBLKBUFMEM=\([^@]*\).*/\1/')
override NOBLKMEM		:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* NOBLKMEM=\([^@]*\).*/\1/')
override NOBUFMEM		:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* NOBUFMEM=\([^@]*\).*/\1/')
override PARASCIP		:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* PARASCIP=\([^@]*\).*/\1/')
override READLINE		:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* READLINE=\([^@]*\).*/\1/')
override SANITIZE		:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* SANITIZE=\([^@]*\).*/\1/')
override ZIMPL			:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* ZIMPL=\([^@]*\).*/\1/')
override ZIMPLOPT		:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* ZIMPLOPT=\([^@]*\).*/\1/')
override ZLIB			:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* ZLIB=\([^@]*\).*/\1/')
include $(SCIPDIR)/make/make.project




#-----------------------------------------------------------------------------
# Rules
#-----------------------------------------------------------------------------

ifeq ($(VERBOSE),false)
.SILENT:	$(MAINFILE) $(MAINOBJFILES) $(MAINSHORTLINK)
endif

.PHONY: all
all:            $(SCIPDIR) $(MAINFILE) $(MAINSHORTLINK)


$(MAINSHORTLINK):	$(MAINFILE)
		@rm -f $@
		cd $(dir $@) && ln -s -g $(notdir $(MAINFILE)) $(notdir $@)

$(OBJDIR):
		@-mkdir -p $(OBJDIR)

$(BINDIR):
		@-mkdir -p $(BINDIR)



$(MAINFILE):	$(BINDIR) $(OBJDIR) $(SCIPLIBFILE) $(LPILIBFILE) $(NLPILIBFILE) $(MAINOBJFILES)
		@echo "-> linking $@"
		$(LINKCXX) $(MAINOBJFILES) $(LINKCXXSCIPALL) $(LDFLAGS) -g $(LINKCXX_o)$@


$(OBJDIR)/%.o:	$(SRCDIR)/%.cpp
		@echo "-> compiling $@"
		$(CXX) $(FLAGS) $(OFLAGS) $(BINOFLAGS) $(CXXFLAGS) $(DFLAGS) -g -c $< $(CXX_o)$@



#---- EOF --------------------------------------------------------------------











