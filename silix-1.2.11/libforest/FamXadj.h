/* FamXadj.h
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

#ifndef BUILDFAM_FAMXADJ_H
#define BUILDFAM_FAMXADJ_H
#include<string>
#include<vector>
#include<iostream>
#include<sstream>
#include<SpanningStarForest.h>

namespace buildfam {

  template<typename TConverter>
  class FamXadj
  {
  private:
    SpanningStarForest* _ssf;
    TConverter _convt;
    void addEdgesFromFile(Forest& f, const std::string& fname);
  public:
    // constructor for n nodes and edges given in file fname
    FamXadj(int n, const std::string& fname);
    // constructor for n nodes and edges given in multiple files
    FamXadj(int n, const std::vector<std::string>& vfname);
    ~FamXadj(){
      delete _ssf;
    }
    SpanningStarForest& operator() () const{
      return *_ssf;
    }
    template <typename TConverter1>
    friend std::ostream& operator<< (std::ostream& out, FamXadj<TConverter1>& f);
  };

  template<typename TConverter>
  FamXadj<TConverter>::FamXadj(int n, const std::string& fname)
  {
    Forest f(n);
    addEdgesFromFile(f, fname);
    _ssf = new SpanningStarForest(f);
  }
  
  template<typename TConverter>
  FamXadj<TConverter>::FamXadj(int n, const std::vector<std::string>& vfname)
  {   
    Forest fbase(n);
    addEdgesFromFile(fbase, vfname[0]); 
    //cerr<<fbase<<endl;
    if (int(vfname.size())>1){
      Forest fnew(n);
      for (int k=1; k<int(vfname.size()); k++){
	addEdgesFromFile(fnew, vfname[k]);
	//cerr<<fnew<<endl;	
	fbase.merge(fnew); 
	//cerr<<fbase<<endl;
	fnew.clean();
      }
    }
    _ssf = new SpanningStarForest(fbase);
  }
 
  template<typename TConverter>
  void FamXadj<TConverter>::addEdgesFromFile(Forest& f, const std::string& fname)
  {
    std::ifstream xadjstream;
    std::string line;
    xadjstream.open(fname.c_str());
    if (!xadjstream.good()){
      std::cerr<<"Error in FamXadj : unable to open file "<<fname<<std::endl;
      exit(1);
    }
    
    int currline = 0;
    while (!xadjstream.eof()) {
      getline(xadjstream,line);
      if (line.length()){
	currline++;
	int i,j;
	std::istringstream linestream(line);
	std::string fromi;
	linestream >> fromi;
	if (fromi.length())
	  i = _convt(fromi);
	else{
	  std::cerr<<"Error in FamXadj : uncorrect format in line "<<currline<<std::endl;
	  exit(1);
	}	
	std::string toj;
	linestream >> toj;
	if (toj.length())
	  j = _convt(toj);
	else{
	  std::cerr<<"Error in FamXadj : uncorrect format in line "<<currline<<std::endl;
	  exit(1);
	}	
#ifdef EXPERT
	f.addEdge(i,j);	
#else
	if (i!=j){
	  if (i>j)
	    f.addEdge(i,j);
	  else
	    f.addEdge(j,i);
	}
#endif
      }
    }
    xadjstream.close();
  }

  template<typename TConverter1>
  std::ostream& operator<<(std::ostream& out, FamXadj<TConverter1>& f)
  {
    SpanningStarForest& ssf = *(f._ssf);
    int n = ssf.size();
    int currfamnum = 0;
    std::vector<int> fams(n, -1);
    for (int i=0; i<n; i++){
      if (ssf.isRooted(i)){
	int r = ssf.root(i);
	if (fams[r]==-1)
	  fams[r] = currfamnum++;
	fams[i] = fams[r];
      }
      else{
	if (fams[i]==-1)
	  fams[i] = currfamnum++;
      }
    }
    for (typename TConverter1::iterator it=f._convt.begin(); it!=f._convt.end(); it++){
        out<<fams[it.value()]<<"\t"<<it.key()<<std::endl;
    }
    return out;
  }

}
#endif
