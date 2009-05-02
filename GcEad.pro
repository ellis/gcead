TEMPLATE = subdirs
CONFIG += debug_and_release
SUBDIRS = Core \
    Model \
    IdacDriver \
    IdacDriver4 \
    IdacDriver2 \
    Idac \
    Scope \
    View
OTHER_FILES += Todo.txt \
    UseCases.txt \
    Notes.txt
