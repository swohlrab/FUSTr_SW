## INSTALATION

Download FUSTrDev with the following command
clone it to your $HOME directory
```bash
git clone https://github.com/swohlrab/FUSTrDev_SW.git
```

## Installing FUSTrDev with Docker
# from within the directory, install (fustr-test is the name of the image, can be changed)

docker build -t fustr-test:latest .

# find image
docker image ls

# start image
run -it fustr-test /bin/bash

# or on the VM
screen -S frust docker run -it -v `pwd`:/input fustr-test /bin/bash

#in the docker container, your data have to be in /home/usr/data (check this first). Otherwise copy them in there to specify the correct path for the VM

# start the run: 
navigate to the right folder

cd /home/usr/FUSTrDev_SW/bin

and start the snkaefile
there is now also an alternative configure.json file, named configure_silix08.json, which is less stringent than the previous one.

snakemake -s FUSTrSnakefile -d /home/usr/data --use-conda --configfile /home/usr/FUSTrDev_SW/configure.json

due to :
SAMPLES, = glob_wildcards("new_cds/{sample}.fasta")
print(SAMPLES)
        pep=expand("new_pep/{sample}.fasta",sample=SAMPLES),
        cds=expand("new_cds/{sample}.fasta",sample=SAMPLES)
in the snakefile, the fasta files have to have the same name!

# get your results out of the docker vm
after the run is finished, you have to copy your data out of the vm by opening a new terminal window (you can't do it from the docker vm terminal) and type first:

docker container ls

this will list your current running docker container, from which you need the CONTAINER ID
with this id, you can copy the stuff to wherever you want to have it ... here, i just copy all data and results back

docker cp <CONTAINER ID>:/home/usr/data <TARGET_DIR>


you will also get some intermidate files etc., so you have to sort out what you will need on the local machine