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
#include <vector>
#include <string>
#include <BlastAnalyzer.h>

using namespace std;
using namespace buildfam;


class BlastAnalyzerTest : public CppUnit::TestFixture {

public:
  void setUp() {
    _ban = new BlastAnalyzer(0.35, 0.70); 
    cerr<<"...................."<<endl;
  }

  void testProcessHSP() {
    std::vector<std::string> hsplines;
    hsplines.push_back(string("AAAAAAU	AAAAAAT	40.66	332	189	4	79	404	65	394	1e-55	 218"));
    hsplines.push_back(string("AAAAAAU AAAAAAT 35.89	418	244	8	14	411	23	436	1e-53	 212"));
    hsplines.push_back(string("AAAAAAU AAAAAAT 38.94	416	239	5	38	446	30	437	2e-59	 231"));
    _ban->processHSP(404, 394, 3, hsplines);
    CPPUNIT_ASSERT(_ban->select() == GLOBAL);
  }  

  void tearDown() {
    delete _ban;
  }

private :
  BlastAnalyzer * _ban;
};


int
main(int argc, char **argv)
{
  CppUnit::TestSuite *suite=new CppUnit::TestSuite("BlastAnalyzer Test Suite");
  suite->addTest(new CppUnit::TestCaller<BlastAnalyzerTest>("testProcessHSP",
							    &BlastAnalyzerTest::testProcessHSP));
  
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(suite);
  bool res=runner.run();
  int retval=EXIT_SUCCESS;
  if (!res)
    retval=EXIT_FAILURE;  
  return retval;
}
