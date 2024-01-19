/* Converter.h
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

#ifndef BUILDFAM_CONVERTER_H
#define BUILDFAM_CONVERTER_H
#include<string>
#include<sstream>
#include <stdlib.h>
#ifdef WITHBOOST
#include<boost/unordered_map.hpp>
#else
#include<map>
#endif

namespace buildfam {

#ifdef WITHBOOST
  typedef boost::unordered_map<std::string, int> mapStringToInt;
#else
  typedef std::map<std::string, int> mapStringToInt;
#endif


  class ConverterI2I
  {
  private:
    int _n; // greatest used int
  public:
    ConverterI2I() : _n(0) {}
    ~ConverterI2I(){}
    int operator()(const std::string& str){
      int i = atoi(str.c_str());
      if (i>_n)
	_n = i;
      return i;
    }
    int size() const{
      return _n+1;
    }
    
    class iterator{
    private:
      int _pos;
    public:
      iterator() {_pos=0;}
      iterator(int i) {_pos=i;}
      ~iterator() {}
      bool operator==(const iterator& it) const{
	return it._pos==this->_pos;
      }
      bool operator!=(const iterator& it) const{
	return it._pos!=this->_pos;
      }
      iterator& operator++(int){
	_pos++;
	return *this;
      }
      std::string key() const{
	std::stringstream ss;
	ss<<_pos;
	return ss.str();
      }
      int value() const{
	return _pos;
      } 
    };
    
    ConverterI2I::iterator begin(){
      return iterator();
    }
    ConverterI2I::iterator end(){
      return iterator(_n);
    }
  };

  class ConverterStr2I
  {
  protected:
    int _n; // greatest used int
    // map converter from string name to int index
    mapStringToInt _str2i;
  public:
    ConverterStr2I() : _n(0) {}
    virtual ~ConverterStr2I(){}
    int operator()(const std::string& str){
      mapStringToInt::iterator it = _str2i.find(str);
      if (it==_str2i.end()){
	return integrate(str);
      }
      else
	return it->second;      
    }
    int size() const{
      return _n+1;
    }
    virtual int integrate(const std::string& str){
      _str2i[str] = _n++;
      return _n-1;
    }
    class iterator{
    private:
      mapStringToInt::iterator _itm;
    public:
      iterator(mapStringToInt::iterator& itm) : _itm(itm) {}
      ~iterator() {}
      bool operator==(const iterator& it) const{
	return it._itm == this->_itm;
      }
      bool operator!=(const iterator& it) const{
	return it._itm != this->_itm;
      }
      iterator& operator++(int){
	_itm++;
	return *this;
      }
      std::string key() const{
	return _itm->first;
      }
      int value() const{
	return _itm->second;
      } 
    };

    ConverterStr2I::iterator begin(){
      mapStringToInt::iterator itm =_str2i.begin();
      return iterator(itm);
    }
    ConverterStr2I::iterator end(){
      mapStringToInt::iterator itm = _str2i.end();
      return iterator(itm);
    }
  };

  class ConverterStr2I2Str : public ConverterStr2I
  {
  protected: 
    std::vector<std::string> _i2str;
  public:
    virtual int integrate(const std::string& str){
      _i2str.push_back(str);
      return ConverterStr2I::integrate(str);
    }
    std::string operator[](int i) const{
      return _i2str[i];
    }    
  };
  
}
#endif
