/* DBSequence.cc
 *
 * Copyright (C) 2009-2011 CNRS
 *
 * This file is part of SiLiX.
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
#include<DBSequence.h>
#include <sstream>
#include <fstream>
#include <stdlib.h>

using namespace std;

namespace buildfam {

  DBSequence::DBSequence(const std::string& seqfile, const std::string partialfile, bool edgesstreamstatus)
  {
    _nbs = loadSeq(seqfile, edgesstreamstatus);
    if (partialfile.size())
      _nbp = loadPartial(partialfile);
    else
      _nbp = 0;
  }

  int DBSequence::loadSeq(const std::string& seqfile, bool edgesstreamstatus)
  {
    ifstream seqfstream;
    seqfstream.open(seqfile.c_str());
    if (!seqfstream.good()){
      cerr<<"Error in DBSequence : unable to open file "<<seqfile<<endl;
      exit(1);
    }
  
    // fasta processing
    int nbseq = 0;
    string line;
    int slg = 0;
    string sname;
    while(seqfstream.good()){
        getline(seqfstream,line);
        if (line[0]=='>'){
            if (slg>0){
                _dbseq.back()._lg = slg;
                slg = 0;
            }
            istringstream linestream(line);
            linestream>>sname;
            sname = sname.substr(1);
            _dbseq.push_back(Sequence());
            _str2i[sname] = nbseq++;
            _i2str.push_back(sname);
        }
        else{
            slg += line.length();
        }
        line.clear();
    }
    if (slg>0){
        _dbseq.back()._lg = slg;
    }
    seqfstream.close();
    return nbseq;
  }

  int DBSequence::loadPartial(const std::string& partialfile)
  {
    // partial sequences
    ifstream partialfstream;
    partialfstream.open(partialfile.c_str());
    if (!partialfstream.good()){
      cerr<<"Error in DBSequence : unable to open file "<<partialfile<<endl;
      exit(1);
    }
  
    mapStringToInt::iterator itend = _str2i.end();
    int currindexp = 0;
    string tmpstr;
    partialfstream>>tmpstr;
    while(tmpstr.length()){
      mapStringToInt::iterator it = _str2i.find(tmpstr);
      if (it!=itend){
	_dbseq[it->second]._indexp = currindexp++;
      }
      tmpstr.clear();
      partialfstream>>tmpstr;
    }	      
    partialfstream.close();
    return currindexp;
  }

}
