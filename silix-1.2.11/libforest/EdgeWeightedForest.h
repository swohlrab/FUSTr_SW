/* EdgeWeightedForest.h
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

#ifndef BUILDFAM_EDGEWEIGHTEDFOREST_H
#define BUILDFAM_EDGEWEIGHTEDFOREST_H
#include<ForestI.h>
#include<vector>
#include<iostream>
#include <limits>

namespace buildfam {
  
  class EdgeWeightedForest : public ForestI
  {
  protected: 
    // _info[_m+i] : weight of edge (i,root(i)) with _m=_n/2
    int _m; 
    void setWeight(int i, int w){ 
      _info[size()+i] = w; 
    }
    EdgeWeightedForest();
  public:
    EdgeWeightedForest(int n) 
      : ForestI(2*n) {_m=n;}
    EdgeWeightedForest(const EdgeWeightedForest& f) 
      : ForestI(f) {_m=_n/2;}
    ~EdgeWeightedForest(){}
    EdgeWeightedForest& operator=(const EdgeWeightedForest& f){
      ForestI::operator=(f);
      _m=_n/2;
      return *this;
    }
    int weight(int i) const{
      return  _info[size()+i];
    }
    void addEdge(int i, int j, int w){
      if (w>weight(i)){
	setRoot(i,j); 
	setWeight(i,w);
      }
    }
    void merge(const EdgeWeightedForest& ewf1){
      for(int i=0; i<size(); i++)
	if(ewf1.isRooted(i))
	  addEdge(i, ewf1.root(i), ewf1.weight(i));
    }
    virtual int size() const{
      return _m;
    }
  };
  
}
#endif
