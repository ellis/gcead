TEMPLATE = subdirs
CONFIG += debug_and_release
SUBDIRS = Core \
    Filters \
    Model \
    IdacDriver \
    IdacDriver4 \
    IdacDriver2 \
    Idac \
	Scope
win32:SUBDIRS += IdacDriverES
SUBDIRS += \
	ScopeTest \
    View
#win32:SUBDIRS += IdacEs IdacEsTest
OTHER_FILES += Todo.txt \
    UseCases.txt \
    Notes.md \
    Compiling.txt \
    README.md

IdacDriver.depends = Core
IdacDriver2.depends = IdacDriver
IdacDriver4.depends = IdacDriver
Idac.depends = IdacDriver IdacDriver2 IdacDriver4
Scope.depends = Core Filters Model Idac
ScopeTest.depends = Core IdacDriver IdacDriver2 IdacDriver4 Idac Filters Model Scope
View.depends = Core IdacDriver IdacDriver2 IdacDriver4 Idac Filters Model Scope
