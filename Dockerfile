# Use the official Ubuntu 18.04 image as the base image
FROM ubuntu:18.04

# Install software needed to run the pipeline
RUN apt-get -qq update
RUN apt-get install -y wget git build-essential cmake unzip curl


RUN apt-get update && apt-get install -y \
        software-properties-common
    RUN add-apt-repository ppa:deadsnakes/ppa
    RUN apt-get update && apt-get install -y \
        python3.7 \
        python3-pip
    RUN python3.7 -m pip install pip
    RUN apt-get update && apt-get install -y \
        python3-distutils \
        python3-setuptools \
        python3-docutils \
        python3-flask
RUN apt-get install -qqy python3-pip

#ARG package
WORKDIR /home/usr

RUN mkdir data
RUN  apt-get -y install libboost-dev
RUN apt-get -y install libboost-program-options-dev

# Clone the FUSTrDev repository and build silix
WORKDIR /home/usr
COPY silix-1.2.11 /home/usr/FUSTrDev/silix-1.2.11
WORKDIR /home/usr/FUSTrDev/silix-1.2.11
RUN apt-get install automake -y
RUN ./configure && make && make check && make install

WORKDIR /home/usr


RUN echo 'export PATH=/opt/conda/bin:$PATH' > /etc/profile.d/conda.sh && \
    wget --quiet https://repo.continuum.io/miniconda/Miniconda3-4.3.14-Linux-x86_64.sh -O ~/miniconda.sh && \
    /bin/bash ~/miniconda.sh -b -p /opt/conda && \
    rm ~/miniconda.sh

RUN python3 -m pip install --upgrade pip
RUN pip install numpy biopython scipy

ENV PATH /opt/conda/bin:$PATH
RUN conda install snakemake -c bioconda
RUN conda install openssl ca-certificates certifi
RUN conda update openssl ca-certificates certifi
ENV PATH /home/usr/FUSTrDev/bin:$PATH


RUN ln -sf /bin/bash /bin/sh
WORKDIR /home/usr/
#COPY $package /home/usr/data
# Copy specific files to the image
#COPY file1 /destination/path/file1
#COPY /path/to/source/folder /home/usr/data
