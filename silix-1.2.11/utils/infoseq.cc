/* infoseq.cc
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
#include<iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include<boost/program_options.hpp>
using namespace std;
namespace po = boost::program_options;

int main(int argc, char ** argv)
{  
  // BEGIN PROGRAM OPTIONS //
   
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
        
  po::options_description cmdline_options;
  cmdline_options.add(generic).add(hidden);
  po::options_description visible("");
  visible.add(generic);        
  po::positional_options_description pd;
  pd.add("input-file", 1);        
  po::variables_map vm;
  store(po::command_line_parser(argc, argv).
	options(cmdline_options).positional(pd).run(), vm);        
  po::notify(vm);
   

  if (vm.count("help")) {
    stringstream infostream;
    infostream<<argv[0]<<" FILE.FASTA"; 
    infostream<<" - Sequence length displayer"<<endl<<endl
	      <<"From a fasta file,"<<endl
	      <<"display sequence length prefixed by sequence id."<<endl;
    cout<<"General use:"<<endl<<"  "<<infostream.str()<<endl;
    cout << visible << "\n";
    return 0;
  }
  bool badinput = false;
  if (!vm.count("input-file"))
    badinput = true;
  else if (vm["input-file"].as< vector<string> >().size()!=1)
    badinput = true;
  if (badinput){
    cerr<<"Error : uncorrect number of command line arguments"<<endl;
    return 1;
  }

  string line;
  ifstream filein(vm["input-file"].as< vector<string> >()[0].c_str(), ifstream::in);
  getline(filein, line);
  while(filein.good())
  {
	  if (line[0]=='>'){
		  stringstream info(line.substr(1,line.length()-1));
		  string id;
		  info>>id;
		  string desc;
		  info>>desc;
		  cout<<id<<"\t";
		  line.clear();
		  getline(filein, line);
	  }
	  else{
		  int l=0;
		  while((line[0]!='>')&&(filein.good())){
			  stringstream seql(line);
			  string sequence;
			  seql>>sequence;
			  l += sequence.length();
			  line.clear();
			  getline(filein, line);
		  }
		  cout<<l<<endl;
	  }
  }
  filein.close();
}

