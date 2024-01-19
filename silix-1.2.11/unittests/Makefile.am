if CPPUNIT
TESTS = ForestTest FamXadjTest DBSequenceTest BlastAnalyzerTest FamBlastTest
else
TESTS = 
endif

noinst_PROGRAMS = $(TESTS)

AM_CPPFLAGS = -I../libforest -I../libbuildfam

ForestTest_SOURCES = ForestTest.cc
ForestTest_LDADD = ../libforest/libforest.la 
ForestTest_CXXFLAGS =  `@CPPUNITCONFIG@ --cflags`

FamXadjTest_SOURCES = FamXadjTest.cc
FamXadjTest_LDADD = ../libforest/libforest.la 
FamXadjTest_CXXFLAGS =  `@CPPUNITCONFIG@ --cflags`

DBSequenceTest_SOURCES = DBSequenceTest.cc
DBSequenceTest_LDADD = ../libbuildfam/libbuildfam.la 
DBSequenceTest_CXXFLAGS =  `@CPPUNITCONFIG@ --cflags`

BlastAnalyzerTest_SOURCES = BlastAnalyzerTest.cc
BlastAnalyzerTest_LDADD = ../libbuildfam/libbuildfam.la 
BlastAnalyzerTest_CXXFLAGS =  `@CPPUNITCONFIG@ --cflags`

FamBlastTest_SOURCES = FamBlastTest.cc
FamBlastTest_LDADD = ../libbuildfam/libbuildfam.la ../libforest/libforest.la 
FamBlastTest_CXXFLAGS =  `@CPPUNITCONFIG@ --cflags`

AM_LDFLAGS = `@CPPUNITCONFIG@ --libs`

if CPPUNIT
$(TESTS) : ../libforest/libforest.la ../libbuildfam/libbuildfam.la
endif

