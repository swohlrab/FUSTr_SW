/* Forest.h
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

#ifndef BUILDFAM_FOREST_H
#define BUILDFAM_FOREST_H
#include<ForestI.h>

namespace buildfam {

  class Forest : public ForestI
  {
  protected:
    // _info[i] < -1 : i is root of rank -_info[i]
    // for convenience, the rank is initialized at -1 instead of 0
    inline int find(int i) const;
    inline void pathCompression(int i, int r, bool removeroot);
    inline void unionHeuristic(int i, int j);
    int rank(int ri) const{ return -_info[ri]; }
    void incrementRank(int ri){ _info[ri]--; }
    Forest();
  public:
    Forest(int n) : ForestI(n) {};
    Forest(const Forest& f) : ForestI(f) {};
    virtual ~Forest(){}
    Forest& operator=(const Forest& f){
     ForestI::operator=(f); 
     return *this;
    }
    void addEdge(int i, int j);
    void merge(const Forest& f1);
    virtual int size() const{
      return _n;
    }
  };
  
  inline int Forest::find(int i) const{    
    int newi = i;
    int ri = root(i);
    if (ri<0) // i is unknown or already root
      return i;
    else
      while (ri>=0){ // i is rooted
	newi = ri;
	ri = root(ri);
      }
    return newi;
  }

  inline void Forest::pathCompression(int i, int r, bool removeroot){    
    int newi = i;
    int ri = root(newi);
    while (ri>=0){
      setRoot(newi, r);
      newi = ri;
      ri = root(ri);
    }
    if (removeroot) 
      setRoot(newi, r);      
  }

  inline void Forest::unionHeuristic(int i, int j){
    // we suppose i,j>0
    int ri = find(i);
    int rj = find(j);  
    if (rank(ri)>=rank(rj)){
      pathCompression(i, ri, false);
      if (ri!=rj){
	if (rank(ri)==rank(rj))
	  incrementRank(ri); // before the removeroot
	pathCompression(j, ri, true);
      }
      else
	pathCompression(j, ri, false);	
    }
    else{ // if (rank(ri)<rank(rj))
      pathCompression(i, rj, true);
      pathCompression(j, rj, false);	
    }      
  }

}
#endif
