/* FamBlastTest.cc
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

#include <FamBlast.h>

using namespace std;
using namespace buildfam;


class FamBlastTest : public CppUnit::TestFixture {

public:
  void setUp() {
    _bfamb = new FamBlast("data/dbseqtest_seq.dat", "data/dbseqtest_part.dat");
    _bfambDNA = new FamBlast("data/sequences10.dat", "");
  }

  void testProcess() {
    vector<string> blastfiles;
    blastfiles.push_back("data/famblastest_blast.dat");
    _bfamb->processBlast(blastfiles, 0.35, 0.70, 100, 0.35);
    //cerr<<(*_bfamb);
    CPPUNIT_ASSERT(_bfamb->nbFam() == 7);

    blastfiles.clear();
    blastfiles.push_back("data/sequences10_blastout.dat");
    _bfambDNA->processBlast(blastfiles, 0.35, 0.80, 100, 0.35);
    //cerr<<(*_bfamb);
    CPPUNIT_ASSERT(_bfambDNA->nbFam() == 3);
  }
  
  void tearDown() {
    delete _bfamb;
  }

private :
  FamBlast * _bfamb;
  FamBlast * _bfambDNA;
};


int
main(int argc, char **argv)
{
  CppUnit::TestSuite *suite=new CppUnit::TestSuite("FamBlast Test Suite");
  suite->addTest(new CppUnit::TestCaller<FamBlastTest>("testProcess",
						       &FamBlastTest::testProcess));
  
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(suite);
  bool res=runner.run();
  int retval=EXIT_SUCCESS;
  if (!res)
    retval=EXIT_FAILURE; 
  return retval;  
}
