/* SpanningStarForest.cc
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
#include <typeinfo>
#include <SpanningStarForest.h>

using namespace std;

namespace buildfam {

  SpanningStarForest::SpanningStarForest(const Forest& f)
    : Forest(f), _nbroots(0)
  {
    try {
      SpanningStarForest& ssf = dynamic_cast<SpanningStarForest&>(const_cast<Forest&>(f));
      _nbroots = ssf._nbroots;
    }
    catch (const bad_cast& e){	
      forward();
    }
  }  

  bool minus1(int x){ return x<=-1; }
  
  void SpanningStarForest::forward()
  {
    for (int i=0; i<size(); i++){
      pathCompression(i, find(i), false);
    }  	
    //std::cerr<<*this<<std::endl;
    _nbroots = count_if(_info.begin(), _info.end(), minus1);
  }

}
