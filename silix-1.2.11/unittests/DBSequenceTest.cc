/* DBSequenceTest.cc
 *
 * Copyright (C) 2009 CNRS
 *
 * This file is part of BuildFam.
 *
 * Fam is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cstdlib>

#include <DBSequence.h>

using namespace std;
using namespace buildfam;


class DBSequenceTest : public CppUnit::TestFixture {

public:
  void setUp() {
    _dbs = new DBSequence("data/dbseqtest_seq.dat", "data/dbseqtest_part.dat"); 
    cerr<<"...................."<<endl;
  }

  void testOperators() {
    CPPUNIT_ASSERT(_dbs->size() == 99);
    CPPUNIT_ASSERT(_dbs->nbPartial() == 29);
    CPPUNIT_ASSERT((*_dbs)("AABPUUK")._lg == 294);
    CPPUNIT_ASSERT((*_dbs)[4]._lg == 294);
  }  

  void tearDown() {
    delete _dbs;
  }

private :
  DBSequence * _dbs;
};


int
main(int argc, char **argv)
{
  CppUnit::TestSuite *suite=new CppUnit::TestSuite("DBSequence Test Suite");
  suite->addTest(new CppUnit::TestCaller<DBSequenceTest>("testOperators",
							 &DBSequenceTest::testOperators));
  
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(suite);
  bool res=runner.run();
  int retval=EXIT_SUCCESS;
  if (!res)
    retval=EXIT_FAILURE;  
  return retval;
}
