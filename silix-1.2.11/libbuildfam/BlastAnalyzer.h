/* BlastAnalyzer.h
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

#ifndef BUILDFAM_BLASTANALYZER_H
#define BUILDFAM_BLASTANALYZER_H
#include<iostream>
#include<vector>
#include<string>
#include<functional>
#include<config.h>


namespace buildfam {
  
  const int REJECTED=0;
  const int CONTAIN=1;
  const int INCLUDED=2;
  const int GLOBAL=4;
  const int HSPMIN=3;

  class BLASTHSP {
  public:
    int _sco;			/* Score */
    double _expt;		/* Expect */
    int _id;			/* Identities */
    int _pos;			/* Positives */
    int _lgHSP;			/* length of HSP */
    int _bq, _eq;		/* begin and end in query seq. */
    int _bs, _es;		/* begin and end in db seq. */
    int _stat;			/* -1 if status undefined  0 if bad
				   HSP  1 if OK */				   
    int _sens;			/* HSP sens */
			   
    BLASTHSP()
      : _sco(0), _expt(0.), _id(0), _pos(0), _lgHSP(0),
	_bq(0), _eq(0), _bs(0), _es(0), _stat(0),  _sens(0){}
    BLASTHSP(const BLASTHSP& b){
      _sco=b._sco; _expt=b._expt; _id=b._id; _pos=b._pos; _lgHSP=b._lgHSP;
	_bq=b._bq; _eq=b._eq; _bs=b._bs; _es=b._es; _stat=b._stat; _sens=b._sens;
    }
    ~BLASTHSP(){}
  };

  class BLASTHIT {
  public:
    short _sel; // SELECTED if the hit fits criteria of similarity, else REJECTED
    std::vector<BLASTHSP> _hsp; // list of HSPs
    int _nbhsp; // effective number of HSPs (can be < _hsp.size())
    int _sco; // total HSP score 
#ifdef VERBOSE
    double _overlap; //   percentage of total HSP length over the length of the longest sequence in a pairwise alignment
    double _goverlap; //  percentage of total HSP length over the mean length in a pairwise alignment
    double _pospercentsh; /* number of positives==%identity / shortest seq. length */
#endif 
    BLASTHIT()
      : _sel(0), _hsp(), _nbhsp(0), _sco(0)
#ifdef VERBOSE
      ,_overlap(0.), _goverlap(0.), _pospercentsh(0.)
#endif
    {}
    BLASTHIT(const BLASTHIT& b){
      _sel=b._sel;_hsp=b._hsp;_nbhsp=b._nbhsp;_sco=b._sco;
#ifdef VERBOSE
      _overlap=b._overlap;_goverlap=b._goverlap;_pospercentsh=b._pospercentsh;
#endif 
    }
  ~BLASTHIT(){}
  };
  
  struct decreasingCompareHSPScore : public std::binary_function<BLASTHSP, BLASTHSP, bool> {
    bool operator() (const BLASTHSP& b1, const BLASTHSP& b2){
      return b1._sco>b2._sco;
    }
  };
  
  struct increasingCompareHSPBeginQ : public std::binary_function<BLASTHSP, BLASTHSP, bool> {
    bool operator() (const BLASTHSP& b1, const BLASTHSP& b2){
      return  b1._bq<b2._bq;
    }
  };

  
  class BlastAnalyzer
  {
  protected: 
    double _pospercentshmin;
    double _frachspshlenmin;
    // length of query sequence
    int _lgq;
    // length of subject sequence
    int _lgs;
    // initial blast hit
    BLASTHIT _bla;
    // blast hit with selected hsp
    BLASTHIT _bla2;

    void readBlast(int nbhsp, const std::vector<std::string>& hsplines);
    void resetBla(int nbhsp);
    void initBla2(void);
    void resetBla2(int nbhsp);
    void blaBla2(int numHSPbla);
    void consistentHSP();
    void confrontHSP(int ii, int jj);
    void removeOverlapq(int numHSP1, int numHSP2);
    void removeOverlaps(int numHSP1, int numHSP2);
    void summaryHIT();
    BlastAnalyzer(const BlastAnalyzer& ba);
  public:   
    BlastAnalyzer(double pospercentshmin, double frachspshlenmin)
      : _pospercentshmin(pospercentshmin), _frachspshlenmin(frachspshlenmin), _lgq(0), _lgs(0) {}    
    ~BlastAnalyzer(){};
    // process a vector of nbhsp hsp given the sequence length of query and subject lgq and lgs
    void processHSP(int lgq, int lgs, int nbhsp, const std::vector<std::string>& hsplines);
    // select status
    int select() const{
      return _bla2._sel;
    }
    // hsp total score 
    int score() const{
      return _bla2._sco;
    }    
#ifdef VERBOSE
    // overlap 
    double overlap() const{
      return _bla2._overlap;
    }    
    // goverlap 
    double goverlap() const{
      return _bla2._goverlap;
    }
    // pospercentsh
    double pospercentsh() const{
      return _bla2._pospercentsh;
    }
#endif    
    
  };

}
#endif
