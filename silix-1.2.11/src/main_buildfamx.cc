/* main_buildfamx.cc
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
 * This program is distributed in the hope that it will be useful, but1
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public Li1
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include<fstream>
#include<sstream>
#include<config.h>
#include<FamXadj.h>
#include<Converter.h>
#include<boost/program_options.hpp>

using namespace std;
using namespace buildfam;
namespace po = boost::program_options;

int main(int argc, char ** argv )
{
 // BEGIN PROGRAM OPTIONS //
  // Declare a group of options that will be 
  // allowed only on command line
  po::options_description generic("Generic options");
  generic.add_options()
    ("help,h", "Display this information")    
    ("version,v", "Display version information")
    ("expert,x", "Specify if the expert mode is activated");
  // Declare a group of options that will be 
  // allowed both on command line and in
  // config file 
  po::options_description config("Configuration");
  config.add_options()
    ("numeric,n", "Specify that sequences ids are integers in [0;N-1] to improve time performance");
    // Hidden options, will be allowed both on command line and
  // in config file, but will not be shown to the user.
  po::options_description hidden("Hidden options");
  hidden.add_options()
    ("inputs", po::value< vector<string> >(), "inputs");
        
  po::options_description cmdline_options;
  cmdline_options.add(generic).add(config).add(hidden);
  po::options_description visible("");
  visible.add(generic).add(config);        
  po::positional_options_description pd;
  pd.add("inputs", 2);        
  po::variables_map vm;
  store(po::command_line_parser(argc, argv).
	options(cmdline_options).positional(pd).run(), vm);        
  po::notify(vm);

  string isparallel;   
#ifdef WITHMPI 
  isparallel = "parallel ";
#endif

  if (vm.count("help")) {
    stringstream infostream;
    infostream<<"silixx [OPTIONS] <N> <FILE>"<<endl<<endl
	      <<"From [an upper bound of] the number of sequences and from sequence id pairs, "
	      <<"display sequence families."<<endl
	      <<"Each line of 'FILE' is a pair of sequence ids."<<endl
	      <<"Note that, in expert mode, a pair of sequence ids (whatever the order) must be seen only once."<<endl
	      <<"Return a family id for each sequence id."<<endl; 

    cout<<"silixx, "<<isparallel<<"ultra fast SIngle LInkage Clustering of Sequences"<<endl<<endl;
    cout<<"General use:"<<endl<<"  "<<infostream.str()<<endl;
    cout << visible << "\n";
    return 0;
  }
  if (vm.count("version")) {
    cout<<"silixx, "<<isparallel<<"ultra fast SIngle LInkage Clustering of Sequences -- ";
    stringstream infostream;
    infostream<<"version "<<VERSION<<endl; 
    cout<<infostream.str()<<endl;
    return 0;
  }
  if (vm.count("expert")) {
#ifdef EXPERT
    cout<<"silixx, expert mode activated."<<endl;
    return 0;
#else
    cout<<"silixx, expert mode inactivated."<<endl;
    return 0;
#endif
  }


  bool badinput = false;
  if (!vm.count("inputs"))
    badinput = true;
  else if (vm["inputs"].as< vector<string> >().size()!=2)
    badinput = true;
  if (badinput){
    cerr<<"error: uncorrect number of command line arguments"<<endl;
    return 1;
  }
  vector<string> inputs = vm["inputs"].as< vector<string> >();
  // END PROGRAM OPTIONS //


  int n = atoi(inputs[0].c_str()); 
  string fname = inputs[1];

  if (vm.count("number")){
    FamXadj<ConverterI2I> bfam(n, fname);
    cout<<bfam<<endl; 
    cerr<<"Number of families : "<<bfam().nbRoots()<<endl;  
  }
  else{
    FamXadj<ConverterStr2I> bfam(n, fname);
    cout<<bfam<<endl; 
    cerr<<"Number of families : "<<bfam().nbRoots()<<endl;     
  }
  return 0;
}
