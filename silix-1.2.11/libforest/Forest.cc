/* Forest.cc
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

#include <Forest.h>

using namespace std;

namespace buildfam {
    
  void Forest::addEdge(int i, int j)
  {
    unionHeuristic(i, j);
  }
  
  void Forest::merge(const Forest& f1)
  {
    for (int i=0; i<size(); i++){
      int ri1 = f1.find(i);
      if (i!=ri1)
	addEdge(ri1, i); // by construction, ri1>i       
    }    
  }
  
}
