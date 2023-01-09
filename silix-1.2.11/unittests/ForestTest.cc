/* ForestTest.cc
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

#include <SpanningStarForest.h>

using namespace std;
using namespace buildfam;


class ForestTest : public CppUnit::TestFixture {

public:
  void setUp() {
    _f = new Forest(6); 
  }

  void testAdd() {
    _f->addEdge(2,1);
    _f->addEdge(3,2);
    _f->addEdge(4,0);
    _f->addEdge(4,1);
    _f->addEdge(5,4);
    //cerr<<*_f<<endl;
  }
  
  void testBackward() {
    testAdd();
    
    SpanningStarForest ssf(*_f);
    //cerr<<ssf<<endl;
    CPPUNIT_ASSERT(ssf.nbRoots() == 1);
  }

  void tearDown() {
    delete _f;
  }

private :
  Forest * _f;
};


int
main(int argc, char **argv)
{
  CppUnit::TestSuite *suite=new CppUnit::TestSuite("Forest Test Suite");
  suite->addTest(new CppUnit::TestCaller<ForestTest>("testBackward",
						       &ForestTest::testBackward));
  
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(suite);
  bool res=runner.run();
  int retval=EXIT_SUCCESS;
  if (!res)
    retval=EXIT_FAILURE; 
  return retval;  
}
