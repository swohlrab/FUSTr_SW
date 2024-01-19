/* FamBlast.h
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

#ifndef BUILDFAM_FAMBLAST_H
#define BUILDFAM_FAMBLAST_H
#include<SpanningStarForest.h>
#include<EdgeWeightedForest.h>
#include<DBSequence.h>
#include<BlastAnalyzer.h>
#include<string>
#include<vector>
#include<fstream> 
#include<sstream> 

namespace buildfam {

  class FamBlast
  {
  protected:
    std::string _prefix;
    int _currfamnum;
    DBSequence _dbseq;
    Forest _fwork;
    EdgeWeightedForest _ewfwork;
    bool updateFamComp(int numq, int nums, int sel);
    bool updateFamPart(int numq, int nums, int sel, int minplen, double minpov, int score);
    virtual void computeFamComp();
    virtual void computeFamPart();
    virtual void computeFamSingleton();
    std::ofstream _edgesstream;
    bool _edgesstreamstatus;
  public:
    // from files containing the sequences and partial sequences name
    FamBlast(const std::string& seqfile, const std::string& partialfile, const std::string prefix = "", bool edgesstreamstatus = false)
      : _prefix(prefix), _currfamnum(0), _dbseq(seqfile, partialfile, edgesstreamstatus),
        _fwork(_dbseq.size()), _ewfwork(_dbseq.size()), _edgesstreamstatus(edgesstreamstatus) {}
    virtual ~FamBlast(){}
    // process blast file with tuning parameters
    int processBlast(const std::vector<std::string>& blastfiles, double pospercentshmin, double frachspshlenmin, int minplen, double minpov);
    // number of families
    int nbFam() const{
      return _currfamnum;
    }
    friend std::ostream& operator<<(std::ostream& out, FamBlast& f);
  };
  
}
#endif
