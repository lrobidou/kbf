# What is this repository ?
This repository contains a slightly modified version of https://github.com/Kingsford-Group/kbf.
Indeed, the dependecies of kbf have changed a bit and thus it not not so easy to use anymore.
Moreover, it it seemed to lack a way to index a fasta/fastq file and compare it to another file, making it hard to compare other tools to it. This is now allowed on this version of kbf.

Youc can check the changes made since the fork here: https://github.com/lrobidou/kbf/compare/start...master

## Execute this version:

### for 2kbf:
```
rm results/spaceOf2Kbf.txt  # if any from the previous execution
rm results/exe2kbf_*  # same
./build.sh
python3 kbf2launcher.py
```
### for the others filters:

```
# be sure to run this AFTER 2kbf
./build/thirdparty/libbf/bin/kbf ../../qtf/data/ecoli1.fasta 31 ../../qtf/data/ecoli2.fasta "results/test" > results/exeKbf.json
python3 bfAndKbfAnalyser.py
```

================================================


#k-mer Bloom filters
------------------------------

Sequence k-mer content is often used to compare sequences, enabling significant 
performance improvements in metagenomic species identification, estimation of transcript abundances, 
and alignment-free comparison of sequencing data. k-mer sets often reach hundreds 
of millions of elements making traditional data structures impractical for k-mer set storage.
Probabilistic Bloom filters and their variants are often used instead. 
Bloom filters reduce the memory footprint and allow for fast set containment queries.
Since k-mers are derived from sequencing reads, the information about k-mer overlap 
can be used to reduce the false positive rate up to two orders of magnitude 
with little or no additional memory and with set containment queries that are 
1.3 - 1.6 times slower. 
Alternatively, we can leverage k-mer overlap information to store k-mer sets in about half 
the space while maintaining the original false positive rate. 

More details are available at:

``` 
Pellow, Filippova, and Kingsford. "Improving Bloom filter performance on sequence data using k-mer Bloom filters" To appear in RECOMb 2016.
```

--------

#### Dependencies

Boost: http://www.boost.org/doc/libs/1_59_0/more/getting_started/unix-variants.html

#### Compilation

```
git clone --recursive https://github.com/lrobidou/kbf
cd kbf
chmod +x build.sh
./build.sh

./build/thirdparty/libbf/bin/kbf /groups/genscale/NGSdatasets/metagenomics/hmp/SRS014107_SRS016349_fasta/SRS014107.denovo_duplicates_marked.trimmed.1.fasta 31 /groups/genscale/NGSdatasets/metagenomics/hmp/SRS014107_SRS016349_fasta/SRS016349.denovo_duplicates_marked.trimmed.1.fasta
```

#### *k*BF Variants

The C++ source files are in the directory `cpp-src`

* KBF1.hpp -- one-sided Bloom filter that improves false postive rate three fold without using any additional storage

* KBF2.hpp -- two-sided Bloom filter that improve FPR by an order of magnitude while using very little additional memory

* KBFSparse.hpp -- sparse Bloom filter with a strict `contains` function that uses 1/2 of space to store the same set of kmers and guarantees the same FPR as a classic Bloom filter

* KBFSparseRelaxed.hpp -- same as above, but `contains` is relaxed
