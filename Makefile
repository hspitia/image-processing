#############################################################################
# Makefile for building: image-processing
# Generated by qmake (2.01a) (Qt 4.4.3) on: S�b 4. Abr 23:25:22 2009
# Project:  image-processing.pro
# Template: app
# Command: e:\temporal\Qt\QtCreator\qt\bin\qmake.exe -win32 -o Makefile image-processing.pro
#############################################################################

first: debug
install: debug-install
uninstall: debug-uninstall
MAKEFILE      = Makefile
QMAKE         = e:/temporal/Qt/QtCreator/qt/bin/qmake.exe
DEL_FILE      = rm
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir
COPY          = cp
COPY_FILE     = $(COPY)
COPY_DIR      = xcopy /s /q /y /i
INSTALL_FILE  = $(COPY_FILE)
INSTALL_PROGRAM = $(COPY_FILE)
INSTALL_DIR   = $(COPY_DIR)
DEL_FILE      = rm
SYMLINK       = 
DEL_DIR       = rmdir
MOVE          = mv
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir
SUBTARGETS    =  \
		debug \
		release

debug: $(MAKEFILE).Debug FORCE
	$(MAKE) -f $(MAKEFILE).Debug
debug-make_default: $(MAKEFILE).Debug FORCE
	$(MAKE) -f $(MAKEFILE).Debug 
debug-make_first: $(MAKEFILE).Debug FORCE
	$(MAKE) -f $(MAKEFILE).Debug first
debug-all: $(MAKEFILE).Debug FORCE
	$(MAKE) -f $(MAKEFILE).Debug all
debug-clean: $(MAKEFILE).Debug FORCE
	$(MAKE) -f $(MAKEFILE).Debug clean
debug-distclean: $(MAKEFILE).Debug FORCE
	$(MAKE) -f $(MAKEFILE).Debug distclean
debug-install: $(MAKEFILE).Debug FORCE
	$(MAKE) -f $(MAKEFILE).Debug install
debug-uninstall: $(MAKEFILE).Debug FORCE
	$(MAKE) -f $(MAKEFILE).Debug uninstall
release: $(MAKEFILE).Release FORCE
	$(MAKE) -f $(MAKEFILE).Release
release-make_default: $(MAKEFILE).Release FORCE
	$(MAKE) -f $(MAKEFILE).Release 
release-make_first: $(MAKEFILE).Release FORCE
	$(MAKE) -f $(MAKEFILE).Release first
release-all: $(MAKEFILE).Release FORCE
	$(MAKE) -f $(MAKEFILE).Release all
release-clean: $(MAKEFILE).Release FORCE
	$(MAKE) -f $(MAKEFILE).Release clean
release-distclean: $(MAKEFILE).Release FORCE
	$(MAKE) -f $(MAKEFILE).Release distclean
release-install: $(MAKEFILE).Release FORCE
	$(MAKE) -f $(MAKEFILE).Release install
release-uninstall: $(MAKEFILE).Release FORCE
	$(MAKE) -f $(MAKEFILE).Release uninstall

Makefile: image-processing.pro  e:/temporal/Qt/QtCreator/qt/mkspecs/default/qmake.conf e:/temporal/Qt/QtCreator/qt/mkspecs/qconfig.pri \
		e:/temporal/Qt/QtCreator/qt/mkspecs/features/qt_functions.prf \
		e:/temporal/Qt/QtCreator/qt/mkspecs/features/qt_config.prf \
		e:/temporal/Qt/QtCreator/qt/mkspecs/features/exclusive_builds.prf \
		e:/temporal/Qt/QtCreator/qt/mkspecs/features/default_pre.prf \
		e:/temporal/Qt/QtCreator/qt/mkspecs/features/win32/default_pre.prf \
		e:/temporal/Qt/QtCreator/qt/mkspecs/features/debug.prf \
		e:/temporal/Qt/QtCreator/qt/mkspecs/features/debug_and_release.prf \
		e:/temporal/Qt/QtCreator/qt/mkspecs/features/default_post.prf \
		e:/temporal/Qt/QtCreator/qt/mkspecs/features/win32/console.prf \
		e:/temporal/Qt/QtCreator/qt/mkspecs/features/win32/rtti.prf \
		e:/temporal/Qt/QtCreator/qt/mkspecs/features/win32/exceptions.prf \
		e:/temporal/Qt/QtCreator/qt/mkspecs/features/win32/stl.prf \
		e:/temporal/Qt/QtCreator/qt/mkspecs/features/shared.prf \
		e:/temporal/Qt/QtCreator/qt/mkspecs/features/warn_on.prf \
		e:/temporal/Qt/QtCreator/qt/mkspecs/features/qt.prf \
		e:/temporal/Qt/QtCreator/qt/mkspecs/features/win32/thread.prf \
		e:/temporal/Qt/QtCreator/qt/mkspecs/features/moc.prf \
		e:/temporal/Qt/QtCreator/qt/mkspecs/features/resources.prf \
		e:/temporal/Qt/QtCreator/qt/mkspecs/features/uic.prf \
		e:/temporal/Qt/QtCreator/qt/mkspecs/features/yacc.prf \
		e:/temporal/Qt/QtCreator/qt/mkspecs/features/lex.prf
	$(QMAKE) -win32 -o Makefile image-processing.pro
e:/temporal/Qt/QtCreator/qt/mkspecs/qconfig.pri:
e:/temporal/Qt/QtCreator/qt/mkspecs/features/qt_functions.prf:
e:/temporal/Qt/QtCreator/qt/mkspecs/features/qt_config.prf:
e:/temporal/Qt/QtCreator/qt/mkspecs/features/exclusive_builds.prf:
e:/temporal/Qt/QtCreator/qt/mkspecs/features/default_pre.prf:
e:/temporal/Qt/QtCreator/qt/mkspecs/features/win32/default_pre.prf:
e:/temporal/Qt/QtCreator/qt/mkspecs/features/debug.prf:
e:/temporal/Qt/QtCreator/qt/mkspecs/features/debug_and_release.prf:
e:/temporal/Qt/QtCreator/qt/mkspecs/features/default_post.prf:
e:/temporal/Qt/QtCreator/qt/mkspecs/features/win32/console.prf:
e:/temporal/Qt/QtCreator/qt/mkspecs/features/win32/rtti.prf:
e:/temporal/Qt/QtCreator/qt/mkspecs/features/win32/exceptions.prf:
e:/temporal/Qt/QtCreator/qt/mkspecs/features/win32/stl.prf:
e:/temporal/Qt/QtCreator/qt/mkspecs/features/shared.prf:
e:/temporal/Qt/QtCreator/qt/mkspecs/features/warn_on.prf:
e:/temporal/Qt/QtCreator/qt/mkspecs/features/qt.prf:
e:/temporal/Qt/QtCreator/qt/mkspecs/features/win32/thread.prf:
e:/temporal/Qt/QtCreator/qt/mkspecs/features/moc.prf:
e:/temporal/Qt/QtCreator/qt/mkspecs/features/resources.prf:
e:/temporal/Qt/QtCreator/qt/mkspecs/features/uic.prf:
e:/temporal/Qt/QtCreator/qt/mkspecs/features/yacc.prf:
e:/temporal/Qt/QtCreator/qt/mkspecs/features/lex.prf:
qmake: qmake_all FORCE
	@$(QMAKE) -win32 -o Makefile image-processing.pro

qmake_all: FORCE

make_default: debug-make_default release-make_default FORCE
make_first: debug-make_first release-make_first FORCE
all: debug-all release-all FORCE
clean: debug-clean release-clean FORCE
distclean: debug-distclean release-distclean FORCE
	-$(DEL_FILE) Makefile

debug-mocclean: $(MAKEFILE).Debug
	$(MAKE) -f $(MAKEFILE).Debug mocclean
release-mocclean: $(MAKEFILE).Release
	$(MAKE) -f $(MAKEFILE).Release mocclean
mocclean: debug-mocclean release-mocclean

debug-mocables: $(MAKEFILE).Debug
	$(MAKE) -f $(MAKEFILE).Debug mocables
release-mocables: $(MAKEFILE).Release
	$(MAKE) -f $(MAKEFILE).Release mocables
mocables: debug-mocables release-mocables
FORCE:

$(MAKEFILE).Debug: Makefile
$(MAKEFILE).Release: Makefile
