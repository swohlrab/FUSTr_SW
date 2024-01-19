## INSTALATION

Download FUSTrDev with the following command
clone it to your $HOME directory
```bash
git clone https://github.com/swohlrab/FUSTr_SW.git
```

## Installing FUSTrDev with Docker
# from within the directory, install (fustr-test is the name of the image, can be changed)

docker build -t fustr-test:latest .

# find image
docker image ls

# start image
docker run -it --rm fustr-test /bin/bash

# or on the VM
screen -S frust docker run -it -v `pwd`:/home/usr/data fustr /bin/bash

#in the docker container, your data have to be in /home/usr/data (check this first). Otherwise copy them in there to specify the correct path for the VM

# start the run: 
navigate to the right folder

cd /home/usr/FUSTrDev_SW/bin

and start the snkaefile
there is now also an alternative configure.json file, named configure_silix08.json, which is less stringent than the previous one.

snakemake -s FUSTrSnakefile -d /home/usr/data --use-conda --configfile /home/usr/configure.json

snakemake -s SnakemakeOrginal -d /home/usr/data --use-conda --configfile /home/usr/configure.json

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
