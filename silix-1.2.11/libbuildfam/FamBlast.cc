/* FamBlast.cc
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

#include<FamBlast.h>
#include<fstream>
#include<sstream>
#include<map>

using namespace std;

namespace buildfam {  

  int FamBlast::processBlast(const std::vector<string>& blastfiles, double pospercentshmin, double frachspshlenmin, int minplen, double minpov)
  {
    int currline = 0;
    if (blastfiles.size()){
      BlastAnalyzer blastan(pospercentshmin, frachspshlenmin);
      for (int i=0; i<int(blastfiles.size()); i++){
	string blastfile = blastfiles[i];
	ifstream blaststream;
	blaststream.open(blastfile.c_str());
	if (!blaststream.good()){
	  cerr<<"Error in Fam : unable to open file "<<blastfile<<endl;
	  exit(1);
	}    
	if (_edgesstreamstatus){
	    size_t dotpos = blastfile.rfind('.');
	    size_t slashpos = blastfile.rfind('/');
	    string prefix;
	    string dir;
	    if (slashpos == string::npos){
	      dir = ".";
	      prefix = blastfile.substr(0, dotpos);
	    }
	    else{
	      dir = blastfile.substr(0, slashpos);
	      prefix = blastfile.substr(slashpos+1, dotpos-slashpos-1);
	    }
	    string ofilename = dir+"/"+prefix+".net";
	    _edgesstream.open(ofilename.c_str());
	    if (!_edgesstream.good()){
	        ofilename = "./"+prefix+".net";
	        _edgesstream.open(ofilename.c_str());
	        if (!_edgesstream.good()){
	            cerr<<"Unable to write in "+dir+" or in curent directory"<<endl;
	            exit(1);
	        }
	    }
	    _edgesstream.precision(3);
	    cerr<<"Generating "<<ofilename<<endl;
	}
	string line;
	vector<string> hsplines;
	int nbhsp;
	string mn1, mn2, firstmn1, firstmn2;
	// first line
	getline(blaststream,line);
	istringstream linestream(line);
	linestream>>firstmn1;
	linestream>>firstmn2;
	currline = 1, nbhsp = 0;
	int numq = _dbseq.index(firstmn1);
	int nums = _dbseq.index(firstmn2);
	// next
	while(line.length()){
	  istringstream linestream(line);
	  linestream>>mn1;
	  linestream>>mn2;
	  if ((mn1==firstmn1)&&(mn2==firstmn2)){
	    nbhsp++;
	    if (int(hsplines.size())<nbhsp)
	      hsplines.push_back(line);
	    else
	      hsplines[nbhsp-1] = line;
	  }
	  else{
#ifndef EXPERT
	    if ((numq==-1)||(nums==-1)){
	      cerr<<"Warning: length of "<<firstmn1<<" or "<<firstmn2<<" is unknown: blast hit skipped."<<endl;
	    }
	    else
#endif
	      if (numq!=nums){
		blastan.processHSP(_dbseq.length(numq), _dbseq.length(nums), nbhsp, hsplines);
		bool accepted = updateFamComp(numq, nums, blastan.select());
#ifdef VERBOSE
	        if (_edgesstreamstatus)
	          if (accepted)
	            _edgesstream<<"\t"<<blastan.pospercentsh()<<"\t"<<blastan.goverlap()<<endl;
#else	      
	        if (_edgesstreamstatus)
                  if (accepted)
                    _edgesstream<<endl;
#endif
		updateFamPart(numq, nums, blastan.select(), minplen, minpov, blastan.score());
	      }
	    nbhsp = 1;
	    if (mn1!=firstmn1) // time redux : optimization for same query name
	      numq = _dbseq.index(mn1); 
	    nums = _dbseq.index(mn2);
	    firstmn1 = mn1;
	    firstmn2 = mn2;
	    hsplines[0] = line;
	  }
	  line.clear();	
	  getline(blaststream,line);
	  currline++;
	}
	// last line
	blastan.processHSP(_dbseq.length(numq), _dbseq.length(nums), nbhsp, hsplines);
	bool accepted = updateFamComp(numq, nums, blastan.select());
#ifdef VERBOSE
	if (_edgesstreamstatus){
	    if (accepted)
	      _edgesstream<<"\t"<<blastan.pospercentsh()<<"\t"<<blastan.goverlap()<<endl;
	    _edgesstream.close();
	}
#else
	if (_edgesstreamstatus){
            if (accepted)
              _edgesstream<<endl;
	    _edgesstream.close();
	}
#endif      
	updateFamPart(numq, nums, blastan.select(), minplen, minpov, blastan.score());
	cerr<<"Number of processed lines in "<<blastfile<<": "<<(currline-1)<<endl;
	blaststream.close();
      }
      // terminal steps
      computeFamComp();
      computeFamPart();
      computeFamSingleton();
    }
    return currline; // blast number
  }
  
  bool FamBlast::updateFamComp(int numq, int nums, int sel)
  {
    if(sel != GLOBAL) return false;
    if(_dbseq.indexPartial(nums) >= 0) return false;
    if(_dbseq.indexPartial(numq) >= 0) return false;
#ifndef EXPERT 
    if (numq<nums)
      _fwork.addEdge(nums, numq); // consistent for the algorithm, in case of adding (numq,nums) and (numq,nums)
    else
#endif
      _fwork.addEdge(numq, nums);
    if (_edgesstreamstatus){
        _edgesstream<<_dbseq.name(numq)<<"\t"<<_dbseq.name(nums);
    }
    return true;
  }

  bool FamBlast::updateFamPart(int numq, int nums, int sel, int minplen, double minpov, int score)
  {
    /*
      2eme tour de classification, en se basant sur la 1ere, mais permettant
      cette fois des differences de longueur => traitement des sequences
      partielles : si une sequence partielle est incluse dans une complete,
      on la classe dans sa famille. 
      Les sequences partielles sont classees
      dans une seule famille, celle avec laquelle est a le meilleur score
      total - apres suppression des overlaps et blocs  non consistants.
    */
    float pover;
    /* si les criteres de similarite ne sont pas remplis :  return */
    if(sel == REJECTED) return false;
  
    /* si les 2 sequences sont completes (deja traite) :  return */
    if(_dbseq.indexPartial(nums) == -1 && _dbseq.indexPartial(numq) == -1) return false;
  
    /* si les 2 sequences sont partielles (non-traite) :  return */
    if(_dbseq.indexPartial(nums) >= 0 && _dbseq.indexPartial(numq) >= 0) return false;
  
    /* si la sequence query est partielle  :   */
    if(_dbseq.indexPartial(numq) >= 0) {
      pover = (float) _dbseq.length(numq) / _dbseq.length(nums);
      /* si seq. partielle trop courte, on ne la classe pas */
      if(_dbseq.length(numq) < minplen || pover < minpov) return false;
      if(sel == GLOBAL || sel == INCLUDED){
	_ewfwork.addEdge(_dbseq.indexPartial(numq), nums, score);
      }
    }
  
    /* si la sequence subject est partielle  :   */
    if(_dbseq.indexPartial(nums) >= 0) {
      pover = (float) _dbseq.length(nums) / _dbseq.length(numq);
      /* si seq. partielle trop courte, on ne la classe pas */
      if(_dbseq.length(nums) < minplen || pover < minpov) return false;
      if(sel == GLOBAL || sel == CONTAIN){
	_ewfwork.addEdge(_dbseq.indexPartial(nums), numq, score);
      }
    }
    return true;
  }

  void FamBlast::computeFamComp()
  {      
    SpanningStarForest ssf(_fwork);

    _currfamnum = 0;
    map<int, int> rootnum;
    for (int i=0; i<ssf.size(); i++){    
      if (!ssf.isUnknown(i)){
	int r;
	if (ssf.isRooted(i))
	  r = ssf.root(i);
	else
	  r = i;
	map<int, int>::iterator rootnum_it = rootnum.find(r);
	if (rootnum_it!=rootnum.end()){
	  _dbseq.family(i) = rootnum_it->second;  
	}
	else{ 
	  rootnum[r] = ++_currfamnum;	   
	  _dbseq.family(i) = _currfamnum;
	}
      }  
    }
  }
  
  void FamBlast::computeFamPart()
  {
    for (int i=0; i<_dbseq.size(); i++){
      int ip = _dbseq.indexPartial(i);
      if(ip >= 0){
	if (_ewfwork.isRooted(ip)){
	  int rootp =  _ewfwork.root(ip);
	  if (_dbseq.family(rootp) == -1){
	    _dbseq.family(i) = ++_currfamnum;
	    _dbseq.family(rootp) = _currfamnum;
	  }
	  else
	    _dbseq.family(i) = _dbseq.family(rootp);
	}
      }
    }
  }
  
  void FamBlast::computeFamSingleton()
  {
    for (int i=0; i<_dbseq.size(); i++){
      if (_dbseq.family(i) == -1){
	_dbseq.family(i) = ++_currfamnum;
      }
    }
  }

  ostream& operator<<(std::ostream& out, FamBlast& f)
  {    	
    mapStringToInt::iterator itseq = f._dbseq.begin();
    mapStringToInt::iterator itseqe = f._dbseq.end();
    if (f._prefix.size())
      for(; itseq!=itseqe; itseq++){
	int i = itseq->second;
	out<<f._prefix;	
	out.fill('0');
	streamsize w = out.width(6);
	out<<f._dbseq.family(i); 
	out.width(w);
	out<<"\t"<<itseq->first; 
	int ip = f._dbseq.indexPartial(i);
	if(ip >= 0){
	    if (f._ewfwork.isRooted(ip))
	        out<<"\t#partial\t"<<f._dbseq.name(f._ewfwork.root(ip));
	    else
	        out<<"\t#partial\t-";
	}
	out<<endl;
    }
    else
      for(; itseq!=itseqe; itseq++){
	int i = itseq->second;
	out<<f._dbseq.family(i); 
	out<<"\t"<<itseq->first; 
	int ip = f._dbseq.indexPartial(i);
        if(ip >= 0){
            if (f._ewfwork.isRooted(ip))
                out<<"\t#partial\t"<<f._dbseq.name(f._ewfwork.root(ip));
            else
                out<<"\t#partial\t-";
        }
	out<<endl;
      }  
    return out;  
  }
  
}
