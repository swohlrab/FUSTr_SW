/* bfstat.cc
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
#include<bitset>
#include<string>
#include<sstream>
#include<boost/program_options.hpp>
using namespace std;
namespace po = boost::program_options;

int main(int argc, char** argv)
{ 

  // BEGIN PROGRAM OPTIONS
   
  // Declare a group of options that will be 
  // allowed only on command line
  po::options_description generic("Generic options");
  generic.add_options()
    ("help,h", "Display this information");
  // Hidden options, will be allowed both on command line and
  // in config file, but will not be shown to the user.
  po::options_description hidden("Hidden options");
  hidden.add_options()
    ("input-file", po::value< vector<string> >(), "input file");
  generic.add_options()
    ("sym,s", "Symmetrize the matrix");     
  po::options_description cmdline_options;
  cmdline_options.add(generic).add(hidden);
  po::options_description visible("");
  visible.add(generic);        
  po::positional_options_description pd;
  pd.add("input-file", 2);        
  po::variables_map vm;
  store(po::command_line_parser(argc, argv).
	options(cmdline_options).positional(pd).run(), vm);        
  po::notify(vm);
   

  if (vm.count("help")) {
    stringstream infostream;
    infostream<<argv[0]<<" FILE.FNODES FILE.[W]NET "; 
    infostream<<" - Affinity matrix builder"<<endl<<endl
	      <<"From nodes prefixed by a family id and a [weighted] network,"<<endl
	      <<"display a matrix with the percentages of nodes of two families being in contact."<<endl;
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
  
  
  string line;
  int currid = 0;
  map<string, int> seq2num;
  map<string, int> fam2num;
  vector<int> famsize;
  vector<string> num2fam;

  // selecting sequences of each fam id 
  cerr<<"Treating "<<inputfiles[0]<<endl;
  ifstream f1;
  f1.open(inputfiles[0].c_str());
  getline(f1, line);
  while (line.length()){
    istringstream linestream(line);
    string famid;
    linestream>>famid;
    int numfam;
    map<string, int>::iterator itf = fam2num.find(famid);
    if (itf==fam2num.end()){
      fam2num[famid] = currid;
      num2fam.push_back(famid);
      numfam=currid;
      famsize.push_back(1);
      currid++;
    }
    else{
      numfam = itf->second;
      famsize[numfam]++;
    }
    string seq;
    linestream>>seq;
    seq2num[seq] = numfam;
    line.clear();
    getline(f1, line);
  }
 
  // treating edges in network
  int nbfam = currid;
  vector< vector<int> > nbedges;
  nbedges.assign(nbfam, vector<int>(nbfam, 0));
  map<string, vector<int> > seq2bit;
  for (map<string, int>::iterator it=seq2num.begin(); it!=seq2num.end(); it++){
    seq2bit[it->first].assign(nbfam, 0);
  }
  cerr<<"Treating "<<inputfiles[1]<<endl;
  ifstream f2;
  f2.open(inputfiles[1].c_str());
  getline(f2, line);
  while (line.length()){
    istringstream linestream(line);
    string seq1;
    linestream>>seq1;
    string seq2;
    linestream>>seq2;
    map<string, int>::iterator it1 = seq2num.find(seq1);
    map<string, int>::iterator it2 = seq2num.find(seq2);
    
    nbedges[it1->second][it2->second]++;
    if (it1->second!=it2->second)
      nbedges[it2->second][it1->second]++;

    seq2bit[seq1][it2->second]++;
    seq2bit[seq2][it1->second]++;

    line.clear();
    getline(f2, line);
  }

  // fam to fam edges
  // for (int i=0; i<nbfam; i++){
  //   for (int j=0; j<nbfam; j++){
  //     cerr<<num2fam[i]<<"\t"<<num2fam[j]<<"\t"<<nbedges[i][j]<<endl;
  //   }
  // }

  // summary
  vector<int> pi(nbfam*nbfam, 0);
  for (map<string, vector<int> >::iterator it=seq2bit.begin(); it!=seq2bit.end(); it++){
    vector<int>& vref = it->second;
    for (int i=0; i<nbfam; i++){
      if (vref[i]>0)
	pi[seq2num[it->first]*nbfam+i]++;   
    }
  }

  // print
  if (nbfam>=10){
    cerr<<"Warning: if class ids are numeric, for 10 classes or more,\nthe matrix columns are displayed in alphabetical order, not in numeric order."<<endl;
  }
  cout<<"# Q"<<endl<<nbfam<<endl;
  //for (map<string, int>::iterator iti=fam2num.begin(); iti!=fam2num.end(); iti++){
  //int i = iti->second;
  //cout<<num2fam[i]<<"\t";
  //}
  //cout<<endl;
  int nbseq = seq2num.size();
  cout<<"# Alpha ";
  for (map<string, int>::iterator iti=fam2num.begin(); iti!=fam2num.end(); iti++){
    cout<<iti->first<<" ";
  }
  cout<<endl;
  for (map<string, int>::iterator iti=fam2num.begin(); iti!=fam2num.end(); iti++){
    int i = iti->second;
    cout<<double(famsize[i])/double(nbseq)<<" ";
  }
  cout<<endl;
  //for (map<string, int>::iterator iti=fam2num.begin(); iti!=fam2num.end(); iti++){
  //int i = iti->second;
  //for (int i=0; i<nbfam; i++){
  //cout<<num2fam[i]<<"\t";
  //}
  //cout<<endl; 
  cout<<"# Pi-Contact"<<endl;
  cout.precision(3);
  for (map<string, int>::iterator iti=fam2num.begin(); iti!=fam2num.end(); iti++){
    int i = iti->second;
    //cout<<num2fam[i]<<"\t";
    for (map<string, int>::iterator itj=fam2num.begin(); itj!=fam2num.end(); itj++){
      int j = itj->second;
      //for (int j=0; j<nbfam; j++){
      if (vm.count("sym"))
	cout<<double(pi[i*nbfam+j]+pi[j*nbfam+i])/double(famsize[i]+famsize[j])<<"\t";
      else
	cout<<double(pi[i*nbfam+j])/double(famsize[i])<<"\t";
      //cout<<pi[i*nbfam+j]<<"/"<<famsize[i]<<"\t";
    }
    cout<<endl;
  }
}
