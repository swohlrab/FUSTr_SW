/* DBSequence.h
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

#ifndef BUILDFAM_DBSEQUENCE_H
#define BUILDFAM_DBSEQUENCE_H
#include<iostream>
#include <string>
#include<vector>
#include<config.h>

#ifdef WITHBOOST
#include <boost/unordered_map.hpp>
#else
#include<map>
#endif

namespace buildfam {

#ifdef WITHBOOST
  typedef boost::unordered_map<std::string, int> mapStringToInt;
#else
  typedef std::map<std::string, int> mapStringToInt;
#endif

  class Sequence
  {
  public:
    int _lg; // length, default=0
    int _fam; // family, default=-1
    int _indexp; // sequence index among partials sequences,
		 // default=-1 if not partial
    Sequence()
      : _lg(0), _fam(-1), _indexp(-1) {}
    Sequence(const Sequence& s){
      _lg=s._lg;_fam=s._fam;_indexp=s._indexp;
    }
    Sequence& operator=(const Sequence& s){
      _lg=s._lg;_fam=s._fam;_indexp=s._indexp;
      return *this;
    }
    ~Sequence(){}
  };
  
  class DBSequence
  {
  protected:
    // number of sequences
    int _nbs;
    // number of partial sequences
    int _nbp;
    // vector of sequences
    std::vector<Sequence> _dbseq;
    // map converter from sequence string name to int index
    mapStringToInt _str2i;
    // vector converter from  int index to sequence string name
    std::vector<std::string> _i2str;
    int loadSeq(const std::string& seqfile, bool _edgesstreamstatus);
    int loadPartial(const std::string& partialfile);
  public:
    // from files containing the sequences and partial sequences name
    DBSequence(const std::string& seqfile, const std::string partialfile = std::string(), bool edgesstreamstatus = false);
    ~DBSequence(){}
    // access to sequence i
    inline Sequence& operator[] (int i);
    // access to sequence with string name sname
    inline Sequence& operator()(const std::string& sname);
    // length of sequence i
    int& length(int i){
      return _dbseq[i]._lg;
    }
    // family of sequence i
    int& family(int i){ 
      return _dbseq[i]._fam;
    }
    // index of sequence i in the partial sequences
    int& indexPartial(int i){ 
      return _dbseq[i]._indexp;
    }
    // index of sequence with string name sname
    int index(const std::string& sname){
#ifdef EXPERT
      return _str2i[sname];
#else
      mapStringToInt::iterator it = _str2i.find(sname);
      if (it!=_str2i.end())
	return it->second;
      else
	return -1;      
#endif
    }
    // number of sequences
    int size() const{ 
      return _nbs; 
    }
    // number of partial sequences
    int nbPartial() const{
      return _nbp;
    }
    // iterator to the map converter begin
    mapStringToInt::iterator begin(){
      return _str2i.begin();
    }
    // iterator to the map converter end
    mapStringToInt::iterator end(){
      return _str2i.end();
    }
    // convert index of sequence to string name
    std::string name(int index) const{
      return _i2str[index];
    }
  };
  
  inline Sequence& DBSequence::operator[] (int i){
    return _dbseq[i];
  }

  inline Sequence& DBSequence::operator() (const std::string& sname){
    return _dbseq[_str2i[sname]];
  }

}
#endif
