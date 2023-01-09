/* ForestI.h
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

#ifndef BUILDFAM_FORESTI_H
#define BUILDFAM_FORESTI_H
#include<vector>
#include<iostream>
#include <algorithm>
#include <iterator>

namespace buildfam {

  class ForestI
  {
  protected:
    int _n;   
    // _info[i] = -1 : i is unknwon
    // _info[i] >= 0  : i is rooted 
    std::vector<int> _info; 
    void setRoot(int i, int r){ 
      _info[i] = r; 
    }
  public:
    ForestI(int n)
      : _n(n) { 
      clean(); 
    }
    ForestI(const ForestI& f)
      : _n(f._n), _info(f._info) {}
    ~ForestI(){}
    ForestI& operator=(const ForestI& f){ 
      _n=f._n;
      _info=f._info; 
      return *this;
    }
    virtual int size() const = 0;
    void clean(){
      _info.assign(_n, -1);
    }
    int root(int i) const{
      return _info[i];
    }
    bool isRoot(int i) const{
	return (_info[i]<-1); 
    }
    bool isUnknown(int i) const{ 
      return (_info[i]==-1); 
    }
    bool isRooted(int i) const{ 
      return (_info[i]>=0); 
    }
    friend std::ostream& operator<<(std::ostream& out, ForestI& f){
      out<<"Forest ";
      std::copy(f._info.begin(),f._info.end(),std::ostream_iterator<int>(out, "\t"));
      return out;
    }
#ifdef WITHMPI
    std::vector<int>& info(){
      return _info;
    }
#endif
  };

}
#endif
