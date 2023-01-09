# FUSTr / FUSTrDev
Families Under Selection in Transcriptomes

[![doi](https://img.shields.io/badge/doi-10.7717/peerj.4234-green.svg?style=flat)](https://doi.org/10.7717/peerj.4234)

# Introduction
FUSTr is a pipeline that clusters coding sequences from transcriptomes into protein families, and then analyzes those families for positive selection.
This is a modified version of the original pipeline, as I was not sure was transdecoder does in the original version. For this pipeline to work, you have to run transdecoder before. You might also run a self-blast before, but this can be done within the script.
If you run your self-blast on ollie, you can create a .sl script with the following content:
To run the self-blast on ollie is currently no option anymore, as I have modified the whole workflow again. Now it is more smooth, and I can implement the blast line again once I need it... Potentially, I just have to remove the rule blastall in the snakefile and have to put the blast-all output generated externally in the right folder of the docker container (via docker cp)
But now, the container works without the need to add some dummy data :-)! It prints in the end all the names of the families, but this can be changed later...


# start script
######################
#!/bin/bash

#SBATCH --job-name=selfblast
#SBATCH --time=12:00:00
#SBATCH --partition=smp
#SBATCH --cpus-per-task=17
#SBATCH --mem=30G
#SBATCH --mail-type=ALL
#SBATCH --mail-user=<your.name>@awi.de



module load bio/blast/2.10.1

# tasks to be performed
#===================================================================
srun makeblastdb -in <fastafile>.fa -dbtype prot -out <dbname>
srun blastp -query <fastafile>.fa -db <dbname> -evalue 1e-10 -outfmt 6 > out all.pep.combined.blastall.out

###############
# end script
and execute it with srun sbatch <scriptname>.sl

# Getting started

The only software needed to run FUSTr is [Docker](https://www.docker.com/). FUSTrDev takes as input a directory containing transcriptome assemblies

**Important:** transcriptome assemblies need to be fasta files ending in .fasta, all in one directory. But these are only the dummy data to start it... I could not make it better. The dummy data will come with the git repo


Download FUSTrDev with the following command
clone it to your $HOME directory
```bash
git clone https://github.com/swohlrab/FUSTrDev_SW.git
```


# Installing FUSTrDev with Docker
With Docker installed correctly on your system issue the following command to initialize the Docker container. You have to run it in the right location of so that the path in the bash command is right.

```
bash FUSTrDev/setup_dockerV2.sh <directory_containing_your_files>
```

Once the docker container has been initialized with docker up, you can enter it using the following command. Alternatively, you can start the docker container with the docker desktop application, and also enter it from there (easier).
Check if the input data get loaded each time when you start the container from the image, but with new data in there..!?

```
docker run -it fustr /bin/bash
```

Now that you are in the docker container, your data is in /home/usr/data (check this first), to run FUSTr simply issue the following command


Old information:there are two snakmake files, depending on your entry point:
Now, there is only one snakemake file that works: 
#FUSTrSnakefileV2
for this one to work without problems, your data folder specified as input in the .sh command <directory_containing_your_files> must contain two subdirectories which contain the cleaned files returned from transdecoder. A "new_cds" directory, and a "new_pep" directory. Both files have to have the same name and end with .fasta
You can create the imput into these directories i.e. clean the transdecoder output with the following commands:
This snakefile contains at the moment in Line 122 the option "if len(famDict[i])>3" and the "3" should actually be a 14, I have just changes this for test reasons (because if there are no families with more then 14 sequences, the script fails...). But this has to be changed back again...
 
#clean transdecoder headers 
of course, what you subset in the files depends on your taste, and you can or even have to modify the following script according to your needs!
 
# first (batch) rename all headers from fastas resulting from transedocer, in this case I aim to remove all after the dot
# in the peptide and cds file

#go to the direcotry containg the folder with the cds and pep fasta files

#make a new directory for the files
mkdir new_cds
mkdir new_pep

for FILE in cds/*.transdecoder.cds;
do
 sed '/^>/s/[.].*//' $FILE > new_${FILE}
done

for FILE in pep/*.transdecoder.pep;
do
 sed '/^>/s/[.].*//' $FILE > new_${FILE}
done

# okay, that works well, and is like the clean header approach

# remove parts of the files names
for filename in new_cds/*.transdecoder.cds; do 
    [ -f "$filename" ] || continue
    mv "$filename" "${filename//.transdecoder.cds/}"

done

for filename in new_pep/*.transdecoder.pep; do 
    [ -f "$filename" ] || continue
    mv "$filename" "${filename//.transdecoder.pep/}"

done

#remove the folder not needed any more
rm -r cds
rm -r pep
 
Afterwards, the new_* directories can be moved into the input folder that the docker container will use to run FUSTr

##OUTDATED FILES
# FUSTrSnakemake: 
here you need as input the output of transdecoder, and FUSTrDev will also run the self-blast. This will however also create dummy-ids to your sequence, which you have to sort back in the end. For this, you need the following datafiles with have to be named excatly like this: 
1) master_peptides.fasta --> which are the orf peptide sequences you got out of transdecoder
2) master_cds.fasta --> which are the matching nucleotide cds you got out of transdecoder

both files have to be in /home/usr/data_files in your docker environment

# AlternativeSnakefile:
here you need as input the output of transdecoder, and of the self-blast run on the transdecoder orfs. For this script, the following data with the excat naming have to be in your /home/usr/data_files folder 
1) all.pep.combined --> which are the orf peptide sequences you got out of transdecoder. Make sure they do not have a hidden .fa or .fasta extension.
2) all.pep.combined.blastall.out --> output of the self blast from the transdecoder peptides.
3) all.cds.combined --> which are the matching nucleotide cds you got out of transdecoder. Make sure they do not have a hidden .fa or .fasta extension.
This script will throw an error that the end, as it can not produce a "final_results" folder, but that doesn't matter, your results are still there...

##THIS IS STILL RIGHT!
# start the run: 
navigate to the right folder

cd /home/usr/FUSTrDev/bin

and start the snkaefile: (example is for the new snakefile, but if you like to use the other, just change the name)
there is now also an alternative configure.json file, named configure_silix08.json, which is less stringent than the previous one.

snakemake -s FUSTrSnakefileV2 -d /home/usr/data --use-conda --configfile /home/usr/FUSTrDev/configure.json


# get your results out of the docker vm
after the run is finished, you have to copy your data out of the vm by opening a new terminal window (you can't do it from the docker vm terminal) and type first:

docker container ls

this will list your current running docker container, from which you need the CONTAINER ID
with this id, you can copy the stuff to wherever you want to have it ... here, i just copy all data and results back

docker cp <CONTAINER ID>:/home/usr/data <TARGET_DIR>


you will also get some intermidate files etc., so you have to sort out what you will need on the local machine


# Some informative stuff from the original FUSTr README:

# Some notes about docker

In order to setup Docker on a new machine you will need root privileges for running commands or to create a group of users. This is not a problem if Docker is already properly installed on the system.

Also, the default container size for Docker is 10 GB, which was plenty to run the analysis for the manuscript (273,221 transcripts and 48,000 simulated transcripts). For larger datasets, this may not be enough space.

For the reasons above, in the event that users do not have root permissions to setup Docker on a new computer, or have a bewilderingly large dataset that would cause the 10GB Docker container to run out of space, below we have included instructions for installing FUSTr on the user's system.


# What goes on under the hood

The file ```setup_docker.sh``` takes as input a directory that contains the transcriptome assemblies the user wishes to analyze. There may be any number of transcriptomes in this directory. The only reqirements are that they are
1. Uncompressed text files
2. Proper [FASTA format](https://en.wikipedia.org/wiki/FASTA_format)
3. End in .fasta
4. All contained in one directory

This directory is then added to a docker container (*located under /home/usr/data*) that installs all necessary third party dependencies, removing the need for users to install any of them on their actual system.

Once in the docker container (*automatically initiated at /home/usr*), ```FUSTr``` is installed to the system path.

Once ```FUSTr``` is executed using ```FUSTr -d ./data -t <number_of_threads>``` [Snakefile](http://snakemake.readthedocs.io/en/stable/) is executed to run 10 subroutines.

1. ```cleanFasta``` takes each  input fasta file in the ./data and cleans the text file for any spurious characters that commonly occur when transferring text files between different system architectures (such as ```^M```) that will break downstram analysis. The output for this file is found in **intermediate_files/{sample}.clean**

2. ```newHeaders``` takes as input the output from ```cleanFasta```. It further cleans the headers only keeping the first fields of text (some times headers have whole paragraphs of unnecessary information describing the sequence, so these are removed). Then these cleaned headers are analyzed to infer any patterns that may exist. The detected header patterns are placed in **headerPatterns.txt**, the output of the fasta files with new headers are placed in **intermediate_files/{sample}.new_headers**

3. ```orf``` takes as input the outputfrom ```newHeaders```. The program [Transdecoder](https://github.com/TransDecoder/TransDecoder/wiki) finds coding sequences from these transcripts, the output is placed in both **{sample}.new_headers.transdecoder.pep** and **{sample}.new_headers.transdecoder.cds**. Additional output from Transdecoder can be found in **{sample}.new_headers.transdecoder_dir**

4. ```longIsoform``` takes as input the files from ```orf``` and **headerPatterns.txt**
 as input to filter isoforms. It looks for genes that have multiple possible isoforms and only passes along the longest isoform for further analysis. The output can be found in **intermediate_files/{sample}.longestIsoform.pep** and **intermediate_files/{sample}.longestIsoform.cds**
5. ```blast``` takes the combined pep output from ```longIsoform``` with lighter unique identifiers as input using [DIAMOND](https://github.com/bbuchfink/diamond)
to run an all against all BLASTP search. The output can be found in **intermediate_files/all.pep.combined.blastall.out**

6. ```silix``` takes as input the output from ```blast``` and assigns proteins to putative gene families in the file **intermediate_files/all.pep.combined_r90_SLX.fnodes**

7. ```mafft``` generates multiple protein squence alignments for the families generated in ```silix``` into files **Families/family_{fam}.aln**
8. ```fasttree``` reconstructs phylogenies for the alignments generated by ```mafft``` into files **Families/family_{fam}.tree**

9. ```trimAln``` trims spurious columns from output of ```mafft``` in files **Families/family_{fam}.trimmed.aln**

10. ```hyphy``` takes trimmed alignments reverse translated to become codon alignments and classifies the selective regime of each site in each family. Output is placed in **Families/family_{fam}\_dir/family_{fam}.aln.codon.FUBAR.json**

# Investigating families of interest

Use the following command to parse only the json files listed in the **famsUnderSelection.txt** file and place them in a nice csv file per family. The columns will have the following information per codon position of the family alignment:

1. alpha Mean posterior synonymous substitution rate at a site
2. beta Mean posterior non-synonymous substitution rate at a site
3. beta-alpha Mean posterior beta-alpha
Prob[alpha>beta] Posterior probability of negative selection at a site
4. Prob[alpha<beta] Posterior probability of positive selection at a site
5. BayesFactor[alpha<beta] Empiricial Bayes Factor for positive selection at a site
6. PSRF Potential scale reduction factor - an MCMC mixing measure
7. Neff Estimated effective sample site for Prob [alpha<beta]

```bash
python FUSTr/utils/fubar_json.FUSTr.py -d <directory_with_fastas>
```

# Following up with codeml

You may wish to rerun the analyses from FUBAR with CODEML, this will take a sigificant amount of time, but I have added an option for FUSTr to do this, before or after running FUSTr just use the ```-doCodeml``` flag at the end and it will run CODEML on all of the families identified by FUSTr.

**IMPORTANT:** make sure you have biopython installed, and CODEML installed

```
FUSTr -d <directory> -t <threads> -doCodeml
```

The output will be in ```final_results/codemlStatsfile.txt```
It will consist of three columns with the family model comparison (M3 vs M0, M2 vs M1, M8 vs M7,M8 vs M8a), and the assosiated p-values for the liklihood ratios of the model comparisons. Complete CODEML output can be found for each model in directory ```Families/family_{num}_dir/```


# How this one works now:
starting the script with: snakemake -s FUSTrSnakefile -d /home/usr/data --use-conda --configfile /home/usr/FUSTrDev/configure.json from within the bin container.
