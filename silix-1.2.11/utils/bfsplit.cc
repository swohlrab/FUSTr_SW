/* bfsplit.cc
 *
 * Copyright (C) 2009-2011 CNRS
 *
 * This file is part of SiLiX.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public Li1
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include<cstdlib>
#include<fstream>
#include<iostream>
#include<map>
#include<string>
#include <cstring>
#include<vector>
#include<sstream>
#include <limits>
#include<boost/program_options.hpp>
using namespace std;
namespace po = boost::program_options;

const int LMAX= 256;
const int BSIZE = 100;
const int NBFAMWARNING = 50000;

class Buffer
{
private:
  string _odir;
  string _prefix;
  string _extension;
  char* _data;
  vector<char*> _begptr;
  vector<char*> _currptr;
  map<string, int>& _famid2iid;
  void terminate(){
    for (map<string, int>::iterator itfamid = _famid2iid.begin();
        itfamid != _famid2iid.end(); itfamid++){
        flush(itfamid->first, itfamid->second);
    }
  }
  void flush(string famid, int iid){
    ofstream vout;
    vout.open(string(_odir+"/"+_prefix+"_"+famid+_extension).c_str(), ios_base::app);
    char* line = _begptr[iid];
    while(line != _currptr[iid]){
        vout<<line<<endl;
        line += LMAX;
    }
    _currptr[iid] = _begptr[iid];
    vout.close();
  }
public:
  Buffer(map<string, int>& famid2iid, string odir, string prefix, string extension)
  : _odir(odir), _prefix(prefix), _extension(extension), _famid2iid(famid2iid){
    int nbfam = _famid2iid.size();
    _data = new char[nbfam*BSIZE*LMAX];
    for (int i=0; i<nbfam; i++){
        _begptr.push_back(_data+ i*BSIZE*LMAX);
    }
    _currptr = _begptr;
    _begptr.push_back(_data + nbfam*BSIZE*LMAX);
  }
  ~Buffer(){
    terminate();
    if (_data)
      delete[] _data;
  }
  void bufferize(string line, string famid){
    // if line.size()<LMAX
    int iid = _famid2iid[famid];
    strcpy(_currptr[iid], line.c_str());
    _currptr[iid] += LMAX;
    if (_currptr[iid] == _begptr[iid+1])
      flush(famid, iid);
  }
};

int main(int argc, char** argv)
{
// BEGIN PROGRAM OPTIONS //

  // Declare a group of options that will be
  // allowed only on command line
  po::options_description generic("Generic options");
  generic.add_options()
    ("help,h", "Display this information");
  // Declare a group of options that will be
  // allowed both on command line and in
  // config file
  int min, max;
  string odir, oprefix;
  po::options_description config("Configuration");
  config.add_options()
  ("odir,o",
	po::value<string>(&odir)->default_value("."),
	"DIRECTORY for output files (current directory by default)")
  ("min,n", po::value<int>(&min)->default_value(5),
   "Minimum family size allowed (>=5 by default)")
   ("max,N", po::value<int>(&max)->default_value(std::numeric_limits<int>::max()),
   "Maximum family size allowed (<+inf by default)")
   ("prefix,p",
         po::value<string>(&oprefix)->default_value(""),
         "Prefix for output files (chosen automatically by default)");
  // Hidden options, will be allowed both on command line and
  // in config file, but will not be shown to the user.
  po::options_description hidden("Hidden options");
  hidden.add_options()
    ("input-file", po::value< vector<string> >(), "input file");

  po::options_description cmdline_options;
  cmdline_options.add(generic).add(config).add(hidden);
  po::options_description visible("");
  visible.add(generic).add(config);
  po::positional_options_description pd;
  pd.add("input-file", 2);
  po::variables_map vm;
  store(po::command_line_parser(argc, argv).
	options(cmdline_options).positional(pd).run(), vm);
  po::notify(vm);


  if (vm.count("help")) {
    stringstream infostream;
    infostream<<argv[0]<<" [OPTION...] FILE.NET/FASTA FILE.FNODES ";
    infostream<<" - Family spliter"<<endl<<endl
	      <<"From FILE.fnodes containing sequence ids prefixed by a family id (clustering from SiLiX),"<<endl
	      <<"and from a FILE.net or FILE.fasta file,"
	      <<"split into multiple .net or .fasta files associated to each family."<<endl;

    cout<<"General use:"<<endl<<"  "<<infostream.str()<<endl;
    cout << visible << "\n";
    return 0;
  }
  bool badinput = false;
  if (!vm.count("input-file"))
    badinput = true;
  else if (vm["input-file"].as< vector<string> >().size()!=2)
    badinput = true;
  if (badinput){
    cerr<<"Error : uncorrect number of command line arguments"<<endl;
    return 1;
  }
  vector<string> inputfiles = vm["input-file"].as< vector<string> >();
  // END PROGRAM OPTIONS //

  // extension choice
  size_t dotpos = inputfiles[0].rfind('.');
  string extension = inputfiles[0].substr(dotpos);
  size_t slashpos = inputfiles[0].rfind('/');
  string prefix;
  string dir;
  if (slashpos == string::npos){
    dir = ".";
    prefix = inputfiles[0].substr(0, dotpos);
  }
  else{
    dir = inputfiles[0].substr(0, slashpos);
    prefix = inputfiles[0].substr(slashpos+1, dotpos-slashpos-1);
  }
  if (oprefix.length()){ // if given by user
    prefix = oprefix;
  }


  // families number and size and conversion tools
  cerr<<"Treating "<<inputfiles[1]<<endl;
  ifstream f1;
  f1.open(inputfiles[1].c_str());
  if (!f1.is_open()){
	cerr<<"Impossible to open "<<inputfiles[1]<<endl;
	exit(1);
  }
  string line;
  map<string, int> dico;
  map<string, string> seqid2famid;
  getline(f1, line);
  while (line.length()){
    istringstream linestream(line);
    string famid;
    linestream>>famid;
    string seqid;
    linestream>>seqid;
    ++dico[famid];
    seqid2famid[seqid] = famid;
    line.clear();
    getline(f1, line);
  }
  f1.close();

   // selecting families
  map<string, int> famid2iid;
  int nbfam = 0;
  for (map<string, int>::iterator it=dico.begin(); it!=dico.end(); it++){
    if (it->second<min || it->second>max){
      // dico.erase(it++);
    }
    else{
      string famid = it->first;
      famid2iid[famid] = nbfam;
      ofstream vout;
      vout.open(string(odir+"/"+prefix+"_"+famid+extension).c_str(), ios_base::out);
      vout.close();
      nbfam++;
    }
  }
  dico.clear();
  if (nbfam>NBFAMWARNING){
    cerr<<"Warning: large number of families ("<<nbfam<<") found may generate too many files for the system or allocate too much memory. Please consider using -n option."<<endl;
  }
  

  // processing
  Buffer buff(famid2iid, odir, prefix, extension);
  cerr<<"Treating "<<inputfiles[0]<<endl;
  ifstream f0;
  f0.open(inputfiles[0].c_str());
  if (!f0.is_open()){
    cerr<<"Impossible to open "<<inputfiles[0]<<endl;
    exit(1);
  }
  if ((extension==".fasta")||(extension==".fa")){
      // Splitting fasta file
      string famid;
      bool discarded = true;
      while (f0.good()){
          getline(f0, line);
          if (line.length()){
              if (line[0]=='>'){
                  istringstream linestream(line);
                  string seqid;
                  linestream>>seqid;
                  seqid = seqid.substr(1);
                  map<string, string>::iterator it = seqid2famid.find(seqid);
                  if (it!=seqid2famid.end()){
                      map<string, int>::iterator infamid = famid2iid.find(it->second);
                      if (infamid != famid2iid.end()){
                          famid= infamid->first;
			  discarded = false;
                      }
		      else
			discarded = true;
                  }
                  else{
                      cerr<<"Warning: unknown sequence id "<<seqid<<" in "<<inputfiles[0]<<endl;
		      discarded = true;
                  }
              }
	      if (!discarded)
		buff.bufferize(line, famid);
          }
          line.clear();
      }
  }
  else{
      if (extension==".net"){
          // Splitting network file
          while (f0.good()){
              getline(f0, line);
              if (line.length()){
                  istringstream linestream(line);
                  string seqid1;
                  linestream>>seqid1;
                  map<string, string>::iterator it1 = seqid2famid.find(seqid1);
                  string seqid2;
                  linestream>>seqid2;
                  map<string, string>::iterator it2 = seqid2famid.find(seqid2);
                  if ( (it1!=seqid2famid.end()) && (it2!=seqid2famid.end()) ){
                      if (it1->second==it2->second){
                          // strict:  if an edge connects two nodes of different families, the edge was skipped
                          map<string, int>::iterator infamid = famid2iid.find(it1->second);
                          if (infamid != famid2iid.end()){
                              string famid = infamid->first;
                              buff.bufferize(line, famid);
                          }
                      }
                  }
                  else{
                      cerr<<"Warning: unknown sequence id "<<seqid1<<" or "<<seqid2<<" in "<<inputfiles[0]<<endl;
                  }
              }
              line.clear();
          }
      }
    else{
      cerr<<"Error: authorized file extensions are .fasta/.fa or .net only."<<endl;
      exit(1);
    }
  }
}
