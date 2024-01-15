## clean transdecoder headers 
before running fustr, copy the transdecoder .pep in one file (use cat for example) and copy the .cds in one file, then clean the headers.
 
# first (batch) rename all headers from fastas resulting from transedocer, in this case I aim to remove all after the dot
# in the peptide and cds file 
# go to the direcotry containg the folder with the cds and pep fasta files

# make a new directory for the files
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
 
# Afterwards, the new_* directories can be moved into the input folder that the docker container will use to run FUSTr