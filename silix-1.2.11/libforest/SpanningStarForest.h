/* SpanningStarForest.h
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

#ifndef BUILDFAM_SPANNINGSTARFOREST_H
#define BUILDFAM_SPANNINGSTARFOREST_H
#include<Forest.h>

namespace buildfam {

  class SpanningStarForest : public Forest
  {
  private:
    int _nbroots;
    void forward();
    SpanningStarForest();
  public:
    SpanningStarForest(int n)
      : Forest(n), _nbroots(0) {}
    SpanningStarForest(const Forest& f);
    virtual ~SpanningStarForest(){}
    SpanningStarForest& operator=(const SpanningStarForest& f){
      Forest::operator=(f);
      _nbroots = f._nbroots;
      return *this;
    } 
    int nbRoots() const{ return _nbroots; }
  };

}
#endif
