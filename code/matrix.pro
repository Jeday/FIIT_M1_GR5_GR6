# Created by and for Qt Creator. This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

CONFIG+=console optimize_full

QMAKE_CXXFLAGS+=-fopenmp

QMAKE_LIBS+=-static -lgomp -lpthread

SOURCES = \
   $$PWD/MatrixMultiplication/MatrixMultiplication.cpp

INCLUDEPATH =

