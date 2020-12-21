NAME = STLover
TYPE = APP
APP_MIME_SIG = application/x-vnd.stlover
SRCS = STLApp.cpp STLWindow.cpp STLToolBar.cpp STLStatView.cpp STLInputWindow.cpp STLRepairWindow.cpp STLLogoView.cpp STLView.cpp main.cpp
RDEFS = Resources.rdef
LIBS = be tracker localestub GL GLU admesh $(STDCPPLIBS)
LOCALES = ca de en fr ro ru tr uk
OPTIMIZE := FULL
SYMBOLS := FLASE
DEBUGGER := FALSE
DEVEL_DIRECTORY := \
	$(shell findpaths -r "makefile_engine" B_FIND_PATH_DEVELOP_DIRECTORY)
include $(DEVEL_DIRECTORY)/etc/makefile-engine
