TEMPLATE = subdirs
CONFIG += debug_and_release
SUBDIRS = Core \
    Filters \
    Model \
    IdacDriver \
    IdacDriver4 \
    IdacDriver2 \
    IdacDriverES \
    Idac \
    Scope \
    ScopeTest \
    View
win32:SUBDIRS += IdacEs IdacEsTest
OTHER_FILES += Todo.txt \
    UseCases.txt \
    Notes.txt \
    Compiling.txt
