/* FamXadjTest.cc
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

#include <Converter.h>
#include <FamXadj.h>

using namespace std;
using namespace buildfam;


class FamXadjTest : public CppUnit::TestFixture 
{

public:
  void setUp() {
  }
  
  void testBackward1() {
    _bfam = new FamXadj<ConverterI2I>(100, "data/bfamtest_100-1.dat");
    //cerr<<(*_bfam)()<<endl; 
    CPPUNIT_ASSERT((*_bfam)().nbRoots() == 51);
    delete _bfam;
  }

  void testBackward2() {
    _bfam2 = new FamXadj<ConverterStr2I>(100, "data/bfamtest_100-2.dat");
    //cerr<<(*_bfam2)<<endl; 
    CPPUNIT_ASSERT((*_bfam2)().nbRoots() == 53);
    delete _bfam;
  }

  void testBackward2bis() {
    _bfam = new FamXadj<ConverterI2I>(100, "data/bfamtest_100-2.dat");
    //cerr<<(*_bfam)<<endl; 
    CPPUNIT_ASSERT((*_bfam)().nbRoots() == 53);
    delete _bfam;
  }


  void testBackward3() {
    _bfam = new FamXadj<ConverterI2I>(100, "data/bfamtest_100-1-2.dat");
    //cerr<<(*_bfam)()<<endl;
    CPPUNIT_ASSERT((*_bfam)().nbRoots() == 18);
    delete _bfam;
  }

  void testMerge() {
    vector<std::string> vfname;
    vfname.push_back("data/bfamtest_100-1.dat");
    vfname.push_back("data/bfamtest_100-2.dat");
    _bfam = new FamXadj<ConverterI2I>(100, vfname);
    //cerr<<(*_bfam)()<<endl; 
    CPPUNIT_ASSERT((*_bfam)().nbRoots() == 18);
    delete _bfam;

    vfname.clear();
    vfname.push_back("data/bfamtest2-1.dat");
    vfname.push_back("data/bfamtest2-2.dat"); 
    _bfam = new FamXadj<ConverterI2I>(50, vfname);
    //cerr<<(*_bfam)()<<endl; 
    CPPUNIT_ASSERT((*_bfam)().nbRoots() == 25);
    delete _bfam;

    vfname.clear();
    vfname.push_back("data/bfamtest3-1.dat");
    vfname.push_back("data/bfamtest3-2.dat"); 
    _bfam = new FamXadj<ConverterI2I>(7, vfname);
    //cerr<<(*_bfam)()<<endl; 
    CPPUNIT_ASSERT((*_bfam)().nbRoots() == 1);
    delete _bfam;
  }

  void tearDown() {
  }

private :
  FamXadj<ConverterI2I> * _bfam;
  FamXadj<ConverterStr2I> * _bfam2;
};


int
main(int argc, char **argv)
{
  CppUnit::TestSuite *suite=new CppUnit::TestSuite("FamXadj Test Suite");
  suite->addTest(new CppUnit::TestCaller<FamXadjTest>("test1", &FamXadjTest::testBackward1));
  suite->addTest(new CppUnit::TestCaller<FamXadjTest>("test2", &FamXadjTest::testBackward2));
  suite->addTest(new CppUnit::TestCaller<FamXadjTest>("test1", &FamXadjTest::testBackward2bis));
  suite->addTest(new CppUnit::TestCaller<FamXadjTest>("test1", &FamXadjTest::testBackward3));
  suite->addTest(new CppUnit::TestCaller<FamXadjTest>("test2", &FamXadjTest::testMerge));
  
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(suite);
  bool res=runner.run();
  int retval=EXIT_SUCCESS;
  if (!res)
    retval=EXIT_FAILURE; 
  return retval;  
}
