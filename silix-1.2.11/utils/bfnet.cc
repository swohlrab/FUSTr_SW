/* bfnet.cc
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
#include<set>
#include<string>
#include<map>
#include<sstream>
#include<boost/program_options.hpp>
using namespace std;
namespace po = boost::program_options;

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
  po::options_description config("Configuration");
  config.add_options() 
    ("families,f", 
     po::value<string>()->composing(), 
     "FILE.FAMS containing families of interest (empty by default)")
    ("strict,s", "If an edge connects two nodes of different families, the edge is skipped (otherwise duplicated but prefixed by each family id, by default)");
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
    infostream<<argv[0]<<" [OPTION...] FILE.FNODES FILE.[W]NET "; 
    infostream<<" - Family-networks builder"<<endl<<endl
	      <<"From nodes prefixed by a family id and a [weighted] network,"<<endl
	      <<"and optionnaly a list of selected family ids,"<<endl
	      <<"display a [weighted] network of edges prefixed by family id."<<endl;
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
  string famfile;
  if (vm.count("families"))
    famfile = vm["families"].as<string>();
  // END PROGRAM OPTIONS //


  string line;
  map<string, string> seq2fam;

  if (famfile.size()){
    // reading selected fam ids
    cerr<<"Treating "<<famfile<<endl;
    set<string> famids;
    ifstream f3;
    f3.open(famfile.c_str());
    if (!f3.is_open()){
      cerr<<"Impossible to open "<<famfile<<endl;
      exit(1);
    }
    getline(f3, line);
    while (line.length()){
      istringstream linestream(line);
      string famid;
      linestream>>famid;
      famids.insert(famid);
      line.clear();
      getline(f3, line);
    }
    f3.close();
    
    // selecting sequences of each fam id 
    cerr<<"Treating "<<inputfiles[0]<<endl;
    ifstream f1;
    f1.open(inputfiles[0].c_str());
    if (!f1.is_open()){
      cerr<<"Impossible to open "<<inputfiles[0]<<endl;
      exit(1);
    }
    getline(f1, line);
    while (line.length()){
      istringstream linestream(line);
      string famid;
      linestream>>famid;
      if (famids.find(famid)!=famids.end()){
        string seq;
        linestream>>seq;
        seq2fam[seq] = famid;
      }
      line.clear();
      getline(f1, line);
    }
  }
  else{    
    // selecting sequences of each fam id 
    cerr<<"Treating "<<inputfiles[0]<<endl;
    ifstream f1;
    f1.open(inputfiles[0].c_str());
    if (!f1.is_open()){
      cerr<<"Impossible to open "<<inputfiles[0]<<endl;
      exit(1);
    }
    getline(f1, line);
    while (line.length()){
      istringstream linestream(line);
      string famid;
      linestream>>famid;
      string seq;
      linestream>>seq;
      seq2fam[seq] = famid;
      line.clear();
      getline(f1, line);
    }
  }
  
  // selecting edges in network between 2 selected sequences
  cerr<<"Treating "<<inputfiles[1]<<endl;
  ifstream f2;
  f2.open(inputfiles[1].c_str());
  if (!f2.is_open()){
    cerr<<"Impossible to open "<<inputfiles[1]<<endl;
    exit(1);
  }
  getline(f2, line);
  while (line.length()){
    istringstream linestream(line);
    string seq1;
    linestream>>seq1;
    string seq2;
    linestream>>seq2;
    map<string, string>::iterator it1 = seq2fam.find(seq1);
    map<string, string>::iterator it2 = seq2fam.find(seq2);

    if (vm.count("strict")==0){
      if (it1!=seq2fam.end())
	cout<<it1->second<<"\t"<<line<<endl;
      if (it2!=seq2fam.end())
	if (it1->second!=it2->second)// duplication
	  cout<<it2->second<<"\t"<<line<<endl;	      
    }
    else{
      if ((it1!=seq2fam.end())&&(it2!=seq2fam.end()))
	if (it1->second==it2->second)
	  cout<<it1->second<<"\t"<<line<<endl;
    }
    line.clear();
    getline(f2, line);
  }
}
