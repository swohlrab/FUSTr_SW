/* BlastAnalyzer.cc
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
#include<BlastAnalyzer.h>
#include<math.h>
#include<stdlib.h>
#include<algorithm>
#include<sstream>

using namespace std;

namespace buildfam {

  void BlastAnalyzer::processHSP(int lgq, int lgs, int nbhsp, const vector<string>& hsplines)
  {  
    _lgq = lgq;
    _lgs = lgs;
    readBlast(nbhsp, hsplines);
    initBla2();
    // remove or shorten non-consistent HSPs
    consistentHSP();
    // evaluate similarities
    summaryHIT();
  }

  void BlastAnalyzer::readBlast(int nbhsp, const vector<string>& hsplines)
  {
    resetBla(nbhsp);    
    for(int ii=0; ii <nbhsp; ii++) {
      istringstream linestream(hsplines[ii]);
      string tmpstr;	
      linestream>>tmpstr; // query
      linestream>>tmpstr; // subject
      double percid;
      linestream>>percid;
      int longaln;
      linestream>>longaln;
      linestream>>tmpstr;//unused value
      linestream>>tmpstr;//idem
      linestream>>_bla._hsp[ii]._bq;
      linestream>>_bla._hsp[ii]._eq;
      linestream>>_bla._hsp[ii]._bs;
      linestream>>_bla._hsp[ii]._es;
      linestream>>_bla._hsp[ii]._expt;
      if (linestream.eof()){
	cerr<<"Format error in BLAST file at line "<<endl<<hsplines[ii]<<endl;
	exit(1);
      }	
      else
	linestream>>_bla._hsp[ii]._sco;
      _bla._hsp[ii]._id = int(floor(percid * longaln / 100.0 + 0.5));
      _bla._hsp[ii]._pos = _bla._hsp[ii]._id;	/* Ici il ya un pb, la valeur des pos n'est pas connue. Je prend la velur de id*/
      _bla._hsp[ii]._sens = 1;
      if (_bla._hsp[ii]._es < _bla._hsp[ii]._bs ) {
	_bla._hsp[ii]._es = - _bla._hsp[ii]._es ;
	_bla._hsp[ii]._bs = - _bla._hsp[ii]._bs ; 
	_bla._hsp[ii]._sens = -1;
      }
            
      int lg1 = _bla._hsp[ii]._es - _bla._hsp[ii]._bs + 1;
      int lg2 = _bla._hsp[ii]._eq - _bla._hsp[ii]._bq + 1;	
      if(lg1 > lg2 ) 
	_bla._hsp[ii]._lgHSP = lg1;
      else 
	_bla._hsp[ii]._lgHSP = lg2;
      _bla._hsp[ii]._stat = 1;
    }
  }
  
  void BlastAnalyzer::resetBla(int nbhsp)
  {
    _bla._nbhsp = nbhsp;
    // if vector is not enough long, allocate necessary space
    for (int i=_bla._hsp.size(); i<nbhsp; i++)
      _bla._hsp.push_back(BLASTHSP());    
  }

  void BlastAnalyzer::initBla2() 
  {
    _bla2._sel = _bla._sel;
    resetBla2(_bla._nbhsp);
  }

  void BlastAnalyzer::resetBla2(int nbhsp)
  {  
    _bla2._nbhsp = 0;
    // if vector is not enough long, allocate maximum space
    for (int i=_bla2._hsp.size(); i<nbhsp; i++)
      _bla2._hsp.push_back(BLASTHSP());
  }

  void BlastAnalyzer::consistentHSP() 
  {
    int nbhsp = _bla._nbhsp;
  
    /* sort HSPs by decreasing score */
    sort(_bla._hsp.begin(), _bla._hsp.begin() + nbhsp, decreasingCompareHSPScore());

    /* keep 1st HSP and initiate the others */
    _bla._hsp[0]._stat = 1;
    for(int ii=1; ii<nbhsp; ii++)
      _bla._hsp[ii]._stat = -1;

    /* check all HSPs for consistency */
    for(int ii=0; ii<nbhsp; ii++) {
      /* if an HSP has already been rejected: continue */
      if(_bla._hsp[ii]._stat == 0) 
	continue;
      for (int jj=ii+1; jj<nbhsp; jj++) 
	confrontHSP(ii, jj);
    }
 
    /* copy HSPs that are OK */
    for(int ii = 0; ii<nbhsp; ii++) {
      if (_bla._hsp[ii]._stat == 0) continue;  
      if (_bla._hsp[ii]._stat != 1) 
	cerr<<"WARNING !"<<endl;  
      blaBla2(ii); 
    }
  }

  void BlastAnalyzer::blaBla2(int numHSPbla)
  { 
    _bla2._nbhsp++;
    int i =  _bla2._nbhsp-1;
    _bla2._hsp[i]._sco   = _bla._hsp[numHSPbla]._sco;
    _bla2._hsp[i]._expt  = _bla._hsp[numHSPbla]._expt;
    _bla2._hsp[i]._id    = _bla._hsp[numHSPbla]._id;
    _bla2._hsp[i]._pos   = _bla._hsp[numHSPbla]._pos;
    _bla2._hsp[i]._lgHSP = _bla._hsp[numHSPbla]._lgHSP;
    _bla2._hsp[i]._bq    = _bla._hsp[numHSPbla]._bq;
    _bla2._hsp[i]._eq    = _bla._hsp[numHSPbla]._eq;
    _bla2._hsp[i]._bs    = _bla._hsp[numHSPbla]._bs;
    _bla2._hsp[i]._es    = _bla._hsp[numHSPbla]._es;
  }

  void BlastAnalyzer::confrontHSP(int ii, int jj)
  {
    int bq1, eq1; /* begin and end of HSP1 (ii) in query seq. */
    int bq2, eq2; /* begin and end of HSP2 (jj) in query seq. */
    int bs1, es1; /* begin and end of HSP1 (ii) in sbjct seq. */
    int bs2, es2; /* begin and end of HSP2 (jj) in sbjct seq. */
    int ovq; /* <= 0 if HSP1 and HSP2 are overlapping in query seq. */
    int ovs; /* <= 0 if HSP1 and HSP2 are overlapping in sbjct seq. */
    int ss1,ss2; /* CDS senses*/

    /* if an HSP has already been rejected: return */
    if(_bla._hsp[jj]._stat == 0) return;

    eq1 = _bla._hsp[ii]._eq;
    eq2 = _bla._hsp[jj]._eq;
    bq1 = _bla._hsp[ii]._bq;
    bq2 = _bla._hsp[jj]._bq;
    es1 = _bla._hsp[ii]._es;
    es2 = _bla._hsp[jj]._es;
    bs1 = _bla._hsp[ii]._bs;
    bs2 = _bla._hsp[jj]._bs;
    ss1 = _bla._hsp[ii]._sens;
    ss2 = _bla._hsp[jj]._sens;    
    
    if (ss1 * ss2 == 0){
      cerr<<"Error in cds orientation : orientation has not been defined"<<endl;
      exit(1);
    }
    if (ss1 * ss2 < 0){
      _bla._hsp[jj]._stat = 0;
      return;
    }
 
    /********************************************************
   reject HSPs that are located within another one.
   only the best HSP (highest score = HSP1) is kept.
    *********************************************************/
    if(bq1 >= bq2 && eq1 <= eq2)  {
      _bla._hsp[jj]._stat = 0;
      return;
    }
    if(bq2 >= bq1 && eq2 <= eq1)  {
      _bla._hsp[jj]._stat = 0;
      return;
    }
    if(bs1 >= bs2 && es1 <= es2)  {
      _bla._hsp[jj]._stat = 0;
      return;
    }
    if(bs2 >= bs1 && es2 <= es1)  {
      _bla._hsp[jj]._stat = 0;
      return;
    }
    /********************************************************
   reject HSPs that are in different orientation:
   Query:    ---- A ---- B -----
   Sbjct:    ---- B ---- A -----

   only the best HSP (highest score = HSP1) is kept.
    *********************************************************/
    if(((eq1 - eq2) * (es1 - es2)) <= 0) {
      _bla._hsp[jj]._stat = 0;
      return;
    }
    if(((bq1 - bq2) * (bs1 - bs2)) <= 0) {
      _bla._hsp[jj]._stat = 0;
      return;
    }

    /********************************************************
   calculate if HSPs are overlapping
    *********************************************************/
    ovq = (bq2 - eq1) * (eq2 - bq1);
    ovs = (bs2 - es1) * (es2 - bs1);

    /********************************************************
   accept non-overlapping HSPs in correct orientation
    *********************************************************/
    if(ovq > 0 && ovs > 0) {
      _bla._hsp[jj]._stat = 1;
      return;
    }

    /********************************************************
   remove overlap between overlapping HSPs : keep the
   best HSP, and remove the part of the other that
   overlaps with it.
    *********************************************************/
    if(ovq < 0) {
      removeOverlapq(ii, jj);
      /* calculate if there is an overlap in sbjct seq */
      es1 = _bla._hsp[ii]._es;
      es2 = _bla._hsp[jj]._es;
      bs1 = _bla._hsp[ii]._bs;
      bs2 = _bla._hsp[jj]._bs;
      ovs = (bs2 - es1) * (es2 - bs1);
    }
    if(ovs < 0) removeOverlaps(ii, jj);

    /* filter out HSPs that are too short */
    if(_bla._hsp[jj]._lgHSP < HSPMIN) {
      _bla._hsp[jj]._stat = 0;
      return;
    }
    /* set status to 1 for consistent HSPs */
    _bla._hsp[jj]._stat = 1;
    return;
  }


  void BlastAnalyzer::removeOverlapq(int ii, int jj)
  {
    int bq1, eq1; /* begin and end of HSP1 (ii) in query seq. */
    int bq2, eq2; /* begin and end of HSP2 (jj) in query seq. */
    int bs2, es2; /* begin and end of HSP2 (jj) in sbjct seq. */

    int bqnew, eqnew; 	/* new begin and end in query seq. */
    int bsnew, esnew; 	/* new begin and end in sbjct seq. */
    int delta;	
    double fract;

    eq1 = _bla._hsp[ii]._eq;
    eq2 = _bla._hsp[jj]._eq;
    bq1 = _bla._hsp[ii]._bq;
    bq2 = _bla._hsp[jj]._bq;
    es2 = _bla._hsp[jj]._es;
    bs2 = _bla._hsp[jj]._bs;

    if(bq2 < bq1) {
      bqnew = bq2;
      eqnew = bq1 - 1;
      delta = eq2 - eqnew;
      bsnew = bs2;
      esnew = es2 - delta;
    }
    else if (eq2 > eq1) {
      bqnew = eq1 + 1;
      eqnew = eq2;
      delta = bqnew - bq2;
      bsnew = bs2 + delta;
      esnew = es2;
    }
    else {
      cerr<<"EXIT: unexpected case! (1)"<<endl;
      exit(1);
    }

    /* set new boundaries */
    _bla._hsp[jj]._bq = bqnew;
    _bla._hsp[jj]._eq = eqnew;
    _bla._hsp[jj]._bs = bsnew;
    _bla._hsp[jj]._es = esnew;

    /* estimate new score, id, pos : */
    /* = initial value * (fraction of length that has been preserved) */
    fract = 1.0 - ((double) delta / (double) _bla._hsp[jj]._lgHSP);

    _bla._hsp[jj]._sco = int(floor ((double) _bla._hsp[jj]._sco * fract));
    _bla._hsp[jj]._id  = int(floor ((double) _bla._hsp[jj]._id  * fract));
    _bla._hsp[jj]._pos = int(floor ((double) _bla._hsp[jj]._pos * fract));


    /* calculate new length */
    _bla._hsp[jj]._lgHSP = _bla._hsp[jj]._lgHSP - delta;

    /* set expect value to -1 : this value should not be used after */
    /* having changed HSPs boundaries */
    _bla._hsp[jj]._expt = -1.0;
  }

  void BlastAnalyzer::removeOverlaps(int ii, int jj)
  {
    int bq2, eq2; /* begin and end of HSP2 (jj) in query seq. */
    int bs1, es1; /* begin and end of HSP1 (ii) in sbjct seq. */
    int bs2, es2; /* begin and end of HSP2 (jj) in sbjct seq. */
    int bsnew, esnew; 	/* new begin and end in query seq. */
    int bqnew, eqnew; 	/* new begin and end in sbjct seq. */
    int delta;	
    double fract;

    es1 = _bla._hsp[ii]._es;
    es2 = _bla._hsp[jj]._es;
    bs1 = _bla._hsp[ii]._bs;
    bs2 = _bla._hsp[jj]._bs;
    eq2 = _bla._hsp[jj]._eq;
    bq2 = _bla._hsp[jj]._bq;

    if(bs2 < bs1) {
      bsnew = bs2;
      esnew = bs1 - 1;
      delta = es2 - esnew;
      bqnew = bq2;
      eqnew = eq2 - delta;
    }
    else if (es2 > es1) {
      bsnew = es1 + 1;
      esnew = es2;
      delta = bsnew - bs2;
      bqnew = bq2 + delta;
      eqnew = eq2;
    }
    else {
      cerr<<"EXIT: unexpected case! (1)"<<endl;
      exit(1);
    }

    /* set new boundaries */
    _bla._hsp[jj]._bs = bsnew;
    _bla._hsp[jj]._es = esnew;
    _bla._hsp[jj]._bq = bqnew;
    _bla._hsp[jj]._eq = eqnew;

    /* estimate new score, id, pos : */
    /* = initial value * (fraction of length that has been preserved) */
    fract = 1.0 - ((double) delta / (double) _bla._hsp[jj]._lgHSP);

    _bla._hsp[jj]._sco = int(floor ((double) _bla._hsp[jj]._sco * fract));
    _bla._hsp[jj]._id  = int(floor ((double) _bla._hsp[jj]._id  * fract));
    _bla._hsp[jj]._pos = int(floor ((double) _bla._hsp[jj]._pos * fract));

    /* calculate new length */
    _bla._hsp[jj]._lgHSP = _bla._hsp[jj]._lgHSP - delta;

    /* set expect value to -1 : this value should not be used after */
    /* having changed HSPs boundaries */
    _bla._hsp[jj]._expt = -1.0;
  }

  void BlastAnalyzer::summaryHIT()
  {
    int delta_lg; /* length difference between aligned seq. */
    int sco_tot, lg_tot, pos_tot, id_tot;
    int ii;
    double frac_HSPshlen; /* cumulative length of HSPs / shortest seq. length */
    double pospercentsh;  /* number of positives / shortest seq. length */
    int nbhsp = _bla2._nbhsp;
    /* sort HSPs by increasing position */
    sort(_bla2._hsp.begin(), _bla2._hsp.begin() + nbhsp, increasingCompareHSPBeginQ());

    /* N-term */
    delta_lg = abs(_bla2._hsp[0]._bq - _bla2._hsp[0]._bs);

    /* C-term */
    ii =  nbhsp - 1;
    delta_lg += abs((_lgq - _bla2._hsp[ii]._eq) - 
		    (_lgs - _bla2._hsp[ii]._es));

    /* internal gaps */
    for(ii = 1; ii < nbhsp; ii++)
      delta_lg += abs((_bla2._hsp[ii]._bq - _bla2._hsp[ii -1 ]._eq) - 
		      (_bla2._hsp[ii]._bs - _bla2._hsp[ii -1 ]._es));

    /* calculate total length, score, Identities and Positives : */
    sco_tot = 0;
    lg_tot = 0;
    pos_tot = 0;
    id_tot = 0;
    for(ii = 0; ii < nbhsp; ii++) {
      sco_tot += _bla2._hsp[ii]._sco;
      lg_tot  += _bla2._hsp[ii]._lgHSP;
      pos_tot += _bla2._hsp[ii]._pos;
      id_tot  += _bla2._hsp[ii]._id;
    }
    _bla2._sco = sco_tot;

    frac_HSPshlen = (double) lg_tot / ((double) (_lgs + _lgq)/ 2.);
    pospercentsh = (double) pos_tot / ((double) (_lgs + _lgq)/2.); 


#ifdef VERBOSE
    _bla2._goverlap = frac_HSPshlen; 
    _bla2._pospercentsh = pospercentsh;
    if ( _lgs > _lgq){
      _bla2._overlap = (double) lg_tot / ((double) (_lgs ));
    }
    else{ 
      _bla2._overlap = (double) lg_tot / ((double) (_lgq ));
    }
#endif
  
    if(pospercentsh > _pospercentshmin &&
       frac_HSPshlen > _frachspshlenmin) {	
      _bla2._sel = GLOBAL;
      return;
    }

    /* sinon, si  similarite pas OK on rejete */
    pospercentsh = (double) pos_tot / lg_tot;
    if(pospercentsh < _pospercentshmin) {
      _bla2._sel = REJECTED;
      return;
    }

    /* si similarite OK et query incluse dans HSP alors INCLUDED */
    frac_HSPshlen = (double) lg_tot / _lgq;
    if(frac_HSPshlen > _frachspshlenmin) {
      _bla2._sel = INCLUDED;
      return;
    }

    /* si similarite OK et sbjct incluse dans HSP alors CONTAIN */
    frac_HSPshlen = (double) lg_tot / _lgs;
    if(frac_HSPshlen > _frachspshlenmin) {
      _bla2._sel = CONTAIN;
      return;
    }

    /* autre cas: on rejete */
    _bla2._sel = REJECTED;
  }

}
