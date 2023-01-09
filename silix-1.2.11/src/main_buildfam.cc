/* main_buildfam.cc
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
#include<fstream>
#include<config.h>
#ifdef WITHMPI
#include<MPIFamBlast.h>
#else
#include<FamBlast.h>
#endif
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
    ("version,v", "Display version information");
  // Declare a group of options that will be 
  // allowed both on command line and in
  // config file
  double ident, overlap, minpov;
  int minplen;
  po::options_description config("Configuration");
  config.add_options()
    ("prefix,f", 
     po::value<string>()->composing(), 
     "Prefix for family ids (none by default)")
    ("partial,p", 
     po::value<string>()->composing(), 
     "File containing partial sequence ids, one per line (none by default)")
    ("ident,i", po::value<double>(&ident)->default_value(0.35,"0.35"), 
     "Min % identity to accept blast hits for building families (in [0,1])")
    ("overlap,r", po::value<double>(&overlap)->default_value(0.80, "0.80"), 
     "Min % overlap to accept blast hits for building families (in [0,1])")
    ("minplen,l", po::value<int>(&minplen)->default_value(100), 
     "Min length to accept partial sequences in families")
    ("minpov,m", po::value<double>(&minpov)->default_value(0.50, "0.50"), 
     "Min % overlap to accept partial sequences in families (in [0,1])")
#ifdef WITHMPI
    ("net,n", "Activate streaming of positively filtered pairs of proteins into files [filenames automatically generated using the prefixes found in MULTIBLASTFILE]");
#else
    ("net,n", "Activate streaming of positively filtered pairs of proteins into a file [filename automatically generated using the prefix of BLASTFILE]");
#endif
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


  string isparallel;   
#ifdef WITHMPI 
  isparallel = "parallel ";
#endif

  if (vm.count("help")) {
    stringstream infostream;
#ifdef WITHMPI
    infostream<<"silix [OPTIONS] <FASTAFILE> <MULTIBLASTFILE>"<<endl<<endl
#else
    infostream<<"silix [OPTIONS] <FASTAFILE> <BLASTFILE>"<<endl<<endl
#endif
              <<"From sequences in fasta format and blast tabulated results, "
	      <<"display sequence families."<<endl
	      <<"'FASTAFILE' respects the fasta format (no blank lines),"<<endl
#ifdef WITHMPI
	      <<"'MULTIBLASTFILE' is a text file containing multiple filenames, each line of these files is a blast hit (option -outfmt 6)."<<endl
#else
	      <<"each line of 'BLASTFILE' is a blast hit (option -outfmt 6)."<<endl
#endif 
	      <<"Return a [prefixed] family id for each sequence id."<<endl; 

    cout<<"silix, "<<isparallel<<"ultra fast SIngle LInkage Clustering of Sequences"<<endl<<endl;
    cout<<"General use:"<<endl<<"  "<<infostream.str()<<endl;
    cout << visible << "\n";
    return 0;
  }
  if (vm.count("version")) {
    cout<<"silix, "<<isparallel<<"ultra fast SIngle LInkage Clustering of Sequences -- ";
    stringstream infostream;
    infostream<<"version "<<VERSION<<endl; 
    cout<<infostream.str()<<endl;
    return 0;
  }

  bool edgesstreamstatus = false;
  if (vm.count("net"))
    edgesstreamstatus = true;
  bool badinput = false;
  if (!vm.count("input-file"))
    badinput = true;
  else if (vm["input-file"].as< vector<string> >().size()!=2)
    badinput = true;
  if (badinput){
    cerr<<"error: uncorrect number of command line arguments"<<endl;
    return 1;
  }
  vector<string> inputfiles = vm["input-file"].as< vector<string> >();
  string partfile;
  if (vm.count("partial"))
    partfile = vm["partial"].as<string>(); 
  string fprefix;
  if (vm.count("prefix"))
    fprefix = vm["prefix"].as<string>();
  // END PROGRAM OPTIONS //
  

  vector<string> blastfiles;
#ifndef WITHMPI
  FamBlast bfamb(inputfiles[0], partfile, fprefix, edgesstreamstatus);
  blastfiles.push_back(inputfiles[1]);
  bfamb.processBlast(blastfiles, ident, overlap, minplen, minpov);
  cout<<bfamb;
  cerr<<"Number of families : "<<bfamb.nbFam()<<endl;
#else
  MPI::Init();
  int rank = MPI::COMM_WORLD.Get_rank();
  int nbprocs = MPI::COMM_WORLD.Get_size();

  // checking the number of blast files
  ifstream blastfstream;
  blastfstream.open(inputfiles[1].c_str());
  if (!blastfstream.good()){
    cerr<<"error: unable to open file "<<inputfiles[1]<<endl;
    exit(1);
  }
  string blastfile;
  int nbfiles = 0;
  while (!blastfstream.eof()){
    blastfstream>>blastfile;
    if (blastfile.length()){
      if (nbfiles%nbprocs==rank){
	blastfiles.push_back(blastfile);
	//cerr<<"rank "<<rank<<" takes "<<blastfile<<endl;
      }
      nbfiles++;
    }
    blastfile.clear();
  }
  blastfstream.close();
  
  // building a group for processing the blast files
  MPI::Intracomm comm = MPI::COMM_WORLD;
  MPI::Group group = comm.Get_group();
  if (nbprocs>nbfiles){
    int triplet[3];
    triplet[0] = 0;
    triplet[1] = nbfiles-1;
    triplet[2] = 1;
    group = group.Range_incl(1, &triplet);
    comm = MPI::COMM_WORLD.Create(group);
  }

  // processing
  if (rank<nbfiles){
    MPIFamBlast bfamb(inputfiles[0], partfile, comm, fprefix, edgesstreamstatus);
    bfamb.processBlast(blastfiles, ident, overlap, minplen, minpov);
    if (rank==0){
      cout<<bfamb;
      cerr<<"Number of families : "<<bfamb.nbFam()<<endl;
    }
  }
  return(MPI_Finalize()); 
#endif
  return 0;
}
