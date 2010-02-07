TEMPLATE = subdirs
CONFIG += debug_and_release
SUBDIRS = Core \
    Model \
    IdacDriver \
    IdacDriver4 \
    IdacDriver2 \
    Idac \
    Scope \
    ScopeTest \
    View
OTHER_FILES += Todo.txt \
    UseCases.txt \
    Notes.txt \
    Compiling.txt
