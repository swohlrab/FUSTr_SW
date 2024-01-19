/* MPIFamBlast.h
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
#ifndef BUILDFAM_MPIFAMBLAST_H
#define BUILDFAM_MPIFAMBLAST_H
#include<math.h>
#include<FamBlast.h>
#include<mpi.h>

namespace buildfam {

  class MPIFamBlast : public FamBlast
  {
  protected:
    int _rank;    
    int _nbprocs;
    MPI::Intracomm& _comm;
    template<typename TForest> void transfer(TForest& tfwork);
    template<typename TForest> void transferDyadic(TForest& tfwork);
    template<typename TForest> void binaryCommunication(TForest& tfwork, TForest& tfrecv, int source, int dest);
    inline virtual void computeFamComp();
    inline virtual void computeFamPart();
    inline virtual void computeFamSingleton();
  public:
    // from files containing the sequences and partial sequences name
    MPIFamBlast(const std::string& seqfile, const std::string& partialfile, MPI::Intracomm& comm, const std::string prefix="", bool edgesstreamstatus = false)
      : FamBlast(seqfile, partialfile, prefix, edgesstreamstatus), _comm(comm) {
      _rank = comm.Get_rank();
      _nbprocs = comm.Get_size();
    }
    virtual ~MPIFamBlast(){}
  };

  template<typename TForest> void MPIFamBlast::transfer(TForest& tfwork)
  {
    int n = tfwork.size();
    if (_rank>0){ 
      // synchronous & blocking
      //std::cerr<<clock()<<" -Ssend from "<<_rank<<" to "<<0<<" with label "<<_rank<<std::endl;
      _comm.Ssend(&(tfwork.info()[0]), tfwork.info().size(), MPI::INT, 0, _rank);
      //std::cerr<<clock()<<" +Ssend from "<<_rank<<" to "<<0<<" with label "<<_rank<<std::endl;
    }
    else{
      TForest frecv(n);
      for (int i=0; i<_nbprocs-1; i++){
	MPI::Status status;
	//std::cerr<<clock()<<" -Recv from ? in "<<_rank<<" with label "<<"?"<<std::endl;
	_comm.Probe(MPI::ANY_SOURCE, MPI::ANY_TAG, status);
	_comm.Recv(&(frecv.info()[0]), frecv.info().size(), MPI::INT, status.Get_source(), status.Get_tag());
	//std::cerr<<clock()<<" +Recv from "<<status.Get_source()<<" in "<<_rank<<" with label "<<status.Get_tag()<<std::endl;
	tfwork.merge(frecv);
      }
    } 
    _comm.Barrier(); // necessary for working transfer by transfer 
  }
  
  template<typename TForest> void MPIFamBlast::transferDyadic(TForest& tfwork)
  { 
    if (_nbprocs>1){
      int k1 = _nbprocs;
      int k2 = int(ceil(k1/2));
      int k3 = int(ceil((k1+1)/2));
      
      // create empty or useful receiver TForest
      int n = 0;
      if (_rank<k2)
	n =  tfwork.size();            
      TForest tfrecv(n);
      
      // dyadic iterative communications
      while(k1>1){
	// low ranked procs receive data
	// from high ranked procs
	for (int i=0; i<k2; i++)	
	  binaryCommunication(tfwork, tfrecv, i+k3, i); 
	_comm.Barrier(); // necessary for iteratively communicate
	// following steps
	k1 = k3;
	k2 = int(ceil(k1/2));
	k3 = int(ceil((k1+1)/2));	
      }
      _comm.Barrier(); // necessary for working transfer by transfer 
    }
  }
  
  template<typename TForest> void MPIFamBlast::binaryCommunication(TForest& tfwork, TForest& tfrecv, 
								   int source, int dest)
  {
    if (_rank==source){ 
      // synchronous & blocking
      //std::cerr<<clock()<<" -Ssend from "<<_rank<<" to "<<dest<<" with label "<<source<<std::endl;
      _comm.Ssend(&(tfwork.info()[0]), tfwork.info().size(), MPI::INT, dest, source);
      //std::cerr<<clock()<<" +Ssend from "<<_rank<<" to "<<dest<<" with label "<<source<<std::endl;
    }
    if (_rank==dest){ 
      //std::cerr<<clock()<<" -Recv from "<<source<<" in "<<_rank<<" with label "<<source<<std::endl;
      _comm.Recv(&(tfrecv.info()[0]), tfrecv.info().size(), MPI::INT, source, source);
      //std::cerr<<clock()<<" +Recv from "<<source<<" in "<<_rank<<" with label "<<source<<std::endl;
      tfwork.merge(tfrecv);
    }
  }

  void MPIFamBlast::computeFamComp()
  {
    //transferDyadic<Forest>(_fwork);
    transfer<Forest>(_fwork);
    if (_rank==0)
      FamBlast::computeFamComp();
  }
  
  void MPIFamBlast::computeFamPart()
  {
    //transferDyadic<EdgeWeightedForest>(_ewfwork);
    transfer<EdgeWeightedForest>(_ewfwork);
    if (_rank==0)      
      FamBlast::computeFamPart();
  } 
  
  void MPIFamBlast::computeFamSingleton()
  {
    if (_rank==0)
      FamBlast::computeFamSingleton();
  }

}
#endif
