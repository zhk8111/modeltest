#############################################################################
# Makefile for building: modeltest-gui
# Generated by qmake (2.01a) (Qt 4.8.6) on: Do. Nov 12 23:43:18 2015
# Project:  src/modeltest.pro
# Template: app
# Command: /usr/bin/qmake-qt4 -spec /usr/share/qt4/mkspecs/linux-g++-64 CONFIG+=debug CONFIG+=declarative_debug -o Makefile src/modeltest.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -m64 -pipe -g -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -m64 -pipe -std=c++11 -g -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++-64 -Isrc -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4 -I/usr/local/lib -I/usr/local/include -I. -I. -Isrc -I.
LINK          = g++
LFLAGS        = -m64
LIBS          = $(SUBLIBS)  -L/usr/lib/x86_64-linux-gnu -L/home/diego/Repositories/modeltest/src/../../../../../usr/local/lib/ -lpll -lpll_optimize -lQtGui -lQtCore -lpthread 
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake-qt4
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = src/main.cpp \
		src/utils.cpp \
		src/model.cpp \
		src/modeltest.cpp \
		src/progressdialog.cpp \
		src/model_optimizer_pll.cpp \
		src/msapll.cpp \
		src/treepll.cpp \
		src/model_selection.cpp \
		src/main_console.cpp \
		src/modeltest_gui.cpp \
		src/partition.cpp \
		src/partitioning_scheme.cpp moc_progressdialog.cpp \
		moc_modeltest_gui.cpp
OBJECTS       = main.o \
		utils.o \
		model.o \
		modeltest.o \
		progressdialog.o \
		model_optimizer_pll.o \
		msapll.o \
		treepll.o \
		model_selection.o \
		main_console.o \
		modeltest_gui.o \
		partition.o \
		partitioning_scheme.o \
		moc_progressdialog.o \
		moc_modeltest_gui.o
DIST          = /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/declarative_debug.prf \
		/usr/share/qt4/mkspecs/features/shared.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		src/modeltest.pro
QMAKE_TARGET  = modeltest-gui
DESTDIR       = 
TARGET        = modeltest-gui

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET): ui_compute_options.h ui_progressdialog.h ui_modeltest_gui.h $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)
	{ test -n "$(DESTDIR)" && DESTDIR="$(DESTDIR)" || DESTDIR=.; } && test $$(gdb --version | sed -e 's,[^0-9][^0-9]*\([0-9]\)\.\([0-9]\).*,\1\2,;q') -gt 72 && gdb --nx --batch --quiet -ex 'set confirm off' -ex "save gdb-index $$DESTDIR" -ex quit '$(TARGET)' && test -f $(TARGET).gdb-index && objcopy --add-section '.gdb_index=$(TARGET).gdb-index' --set-section-flags '.gdb_index=readonly' '$(TARGET)' '$(TARGET)' && rm -f $(TARGET).gdb-index || true

Makefile: src/modeltest.pro  /usr/share/qt4/mkspecs/linux-g++-64/qmake.conf /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/declarative_debug.prf \
		/usr/share/qt4/mkspecs/features/shared.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		/usr/lib/x86_64-linux-gnu/libQtGui.prl \
		/usr/lib/x86_64-linux-gnu/libQtCore.prl
	$(QMAKE) -spec /usr/share/qt4/mkspecs/linux-g++-64 CONFIG+=debug CONFIG+=declarative_debug -o Makefile src/modeltest.pro
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/common/gcc-base.conf:
/usr/share/qt4/mkspecs/common/gcc-base-unix.conf:
/usr/share/qt4/mkspecs/common/g++-base.conf:
/usr/share/qt4/mkspecs/common/g++-unix.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/debug.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/declarative_debug.prf:
/usr/share/qt4/mkspecs/features/shared.prf:
/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
/usr/lib/x86_64-linux-gnu/libQtGui.prl:
/usr/lib/x86_64-linux-gnu/libQtCore.prl:
qmake:  FORCE
	@$(QMAKE) -spec /usr/share/qt4/mkspecs/linux-g++-64 CONFIG+=debug CONFIG+=declarative_debug -o Makefile src/modeltest.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/modeltest-gui1.0.0 || $(MKDIR) .tmp/modeltest-gui1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/modeltest-gui1.0.0/ && $(COPY_FILE) --parents src/model_defs.h src/utils.h src/model.h src/modeltest.h src/progressdialog.h src/model_optimizer.h src/model_optimizer_pll.h src/msa.h src/msapll.h src/plldefs.h src/tree.h src/treepll.h src/model_selection.h src/global_defs.h src/modeltest_gui.h src/partition.h src/partitioning_scheme.h src/thread/threadpool.h .tmp/modeltest-gui1.0.0/ && $(COPY_FILE) --parents src/main.cpp src/utils.cpp src/model.cpp src/modeltest.cpp src/progressdialog.cpp src/model_optimizer_pll.cpp src/msapll.cpp src/treepll.cpp src/model_selection.cpp src/main_console.cpp src/modeltest_gui.cpp src/partition.cpp src/partitioning_scheme.cpp .tmp/modeltest-gui1.0.0/ && $(COPY_FILE) --parents src/compute_options.ui src/progressdialog.ui src/modeltest_gui.ui .tmp/modeltest-gui1.0.0/ && (cd `dirname .tmp/modeltest-gui1.0.0` && $(TAR) modeltest-gui1.0.0.tar modeltest-gui1.0.0 && $(COMPRESS) modeltest-gui1.0.0.tar) && $(MOVE) `dirname .tmp/modeltest-gui1.0.0`/modeltest-gui1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/modeltest-gui1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: moc_progressdialog.cpp moc_modeltest_gui.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_progressdialog.cpp moc_modeltest_gui.cpp
moc_progressdialog.cpp: src/model.h \
		src/model_defs.h \
		src/global_defs.h \
		src/plldefs.h \
		/usr/local/include/pll_optimize.h \
		/usr/local/include/pll.h \
		src/progressdialog.h
	/usr/lib/x86_64-linux-gnu/qt4/bin/moc $(DEFINES) $(INCPATH) src/progressdialog.h -o moc_progressdialog.cpp

moc_modeltest_gui.cpp: src/modeltest.h \
		src/global_defs.h \
		src/msa.h \
		src/tree.h \
		src/model.h \
		src/model_defs.h \
		src/plldefs.h \
		/usr/local/include/pll_optimize.h \
		/usr/local/include/pll.h \
		src/model_selection.h \
		src/model_optimizer_pll.h \
		src/model_optimizer.h \
		src/treepll.h \
		src/msapll.h \
		src/partitioning_scheme.h \
		src/utils.h \
		src/partition.h \
		src/modeltest_gui.h
	/usr/lib/x86_64-linux-gnu/qt4/bin/moc $(DEFINES) $(INCPATH) src/modeltest_gui.h -o moc_modeltest_gui.cpp

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all: ui_compute_options.h ui_progressdialog.h ui_modeltest_gui.h
compiler_uic_clean:
	-$(DEL_FILE) ui_compute_options.h ui_progressdialog.h ui_modeltest_gui.h
ui_compute_options.h: src/compute_options.ui
	/usr/lib/x86_64-linux-gnu/qt4/bin/uic src/compute_options.ui -o ui_compute_options.h

ui_progressdialog.h: src/progressdialog.ui
	/usr/lib/x86_64-linux-gnu/qt4/bin/uic src/progressdialog.ui -o ui_progressdialog.h

ui_modeltest_gui.h: src/modeltest_gui.ui
	/usr/lib/x86_64-linux-gnu/qt4/bin/uic src/modeltest_gui.ui -o ui_modeltest_gui.h

compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean compiler_uic_clean 

####### Compile

main.o: src/main.cpp src/modeltest_gui.h \
		src/modeltest.h \
		src/global_defs.h \
		src/msa.h \
		src/tree.h \
		src/model.h \
		src/model_defs.h \
		src/plldefs.h \
		/usr/local/include/pll_optimize.h \
		/usr/local/include/pll.h \
		src/model_selection.h \
		src/model_optimizer_pll.h \
		src/model_optimizer.h \
		src/treepll.h \
		src/msapll.h \
		src/partitioning_scheme.h \
		src/utils.h \
		src/partition.h \
		src/thread/threadpool.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o src/main.cpp

utils.o: src/utils.cpp src/utils.h \
		src/global_defs.h \
		src/msa.h \
		src/tree.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o utils.o src/utils.cpp

model.o: src/model.cpp src/model.h \
		src/model_defs.h \
		src/global_defs.h \
		src/plldefs.h \
		/usr/local/include/pll_optimize.h \
		/usr/local/include/pll.h \
		src/utils.h \
		src/msa.h \
		src/tree.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o model.o src/model.cpp

modeltest.o: src/modeltest.cpp src/modeltest.h \
		src/global_defs.h \
		src/msa.h \
		src/tree.h \
		src/model.h \
		src/model_defs.h \
		src/plldefs.h \
		/usr/local/include/pll_optimize.h \
		/usr/local/include/pll.h \
		src/model_selection.h \
		src/model_optimizer_pll.h \
		src/model_optimizer.h \
		src/treepll.h \
		src/msapll.h \
		src/partitioning_scheme.h \
		src/utils.h \
		src/partition.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o modeltest.o src/modeltest.cpp

progressdialog.o: src/progressdialog.cpp src/progressdialog.h \
		src/model.h \
		src/model_defs.h \
		src/global_defs.h \
		src/plldefs.h \
		/usr/local/include/pll_optimize.h \
		/usr/local/include/pll.h \
		ui_progressdialog.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o progressdialog.o src/progressdialog.cpp

model_optimizer_pll.o: src/model_optimizer_pll.cpp src/model_optimizer_pll.h \
		src/model_optimizer.h \
		src/model.h \
		src/model_defs.h \
		src/global_defs.h \
		src/plldefs.h \
		/usr/local/include/pll_optimize.h \
		/usr/local/include/pll.h \
		src/treepll.h \
		src/tree.h \
		src/msapll.h \
		src/msa.h \
		src/utils.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o model_optimizer_pll.o src/model_optimizer_pll.cpp

msapll.o: src/msapll.cpp src/msapll.h \
		src/msa.h \
		src/global_defs.h \
		src/plldefs.h \
		/usr/local/include/pll_optimize.h \
		/usr/local/include/pll.h \
		src/utils.h \
		src/tree.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o msapll.o src/msapll.cpp

treepll.o: src/treepll.cpp src/treepll.h \
		src/tree.h \
		src/global_defs.h \
		src/plldefs.h \
		/usr/local/include/pll_optimize.h \
		/usr/local/include/pll.h \
		src/utils.h \
		src/msa.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o treepll.o src/treepll.cpp

model_selection.o: src/model_selection.cpp src/model_selection.h \
		src/model.h \
		src/model_defs.h \
		src/global_defs.h \
		src/plldefs.h \
		/usr/local/include/pll_optimize.h \
		/usr/local/include/pll.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o model_selection.o src/model_selection.cpp

main_console.o: src/main_console.cpp src/modeltest.h \
		src/global_defs.h \
		src/msa.h \
		src/tree.h \
		src/model.h \
		src/model_defs.h \
		src/plldefs.h \
		/usr/local/include/pll_optimize.h \
		/usr/local/include/pll.h \
		src/model_selection.h \
		src/model_optimizer_pll.h \
		src/model_optimizer.h \
		src/treepll.h \
		src/msapll.h \
		src/partitioning_scheme.h \
		src/utils.h \
		src/partition.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main_console.o src/main_console.cpp

modeltest_gui.o: src/modeltest_gui.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o modeltest_gui.o src/modeltest_gui.cpp

partition.o: src/partition.cpp src/partition.h \
		src/global_defs.h \
		src/model.h \
		src/model_defs.h \
		src/plldefs.h \
		/usr/local/include/pll_optimize.h \
		/usr/local/include/pll.h \
		src/msa.h \
		src/tree.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o partition.o src/partition.cpp

partitioning_scheme.o: src/partitioning_scheme.cpp src/partitioning_scheme.h \
		src/utils.h \
		src/global_defs.h \
		src/msa.h \
		src/tree.h \
		src/partition.h \
		src/model.h \
		src/model_defs.h \
		src/plldefs.h \
		/usr/local/include/pll_optimize.h \
		/usr/local/include/pll.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o partitioning_scheme.o src/partitioning_scheme.cpp

moc_progressdialog.o: moc_progressdialog.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_progressdialog.o moc_progressdialog.cpp

moc_modeltest_gui.o: moc_modeltest_gui.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_modeltest_gui.o moc_modeltest_gui.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

