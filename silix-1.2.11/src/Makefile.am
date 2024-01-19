bin_PROGRAMS = silix silixx

silix_SOURCES = main_buildfam.cc 
silix_LDADD = ../libbuildfam/libbuildfam.la ../libforest/libforest.la

silixx_SOURCES = main_buildfamx.cc 
silixx_LDADD = ../libforest/libforest.la

AM_CPPFLAGS = -I. -I../libforest -I../libbuildfam
CLEANFILES =*~
