# How to reproduce the results of kbf

## install kbf
```
git clone --recursive https://github.com/lrobidou/kbf
cd kbf
chmod +x build.sh
./build.sh
```
## Execute kbf for kbf2.
```
rm results/*  # if data left from the previous execution
./build.sh
python3 kbf2launcher.py
```
### for the others filters:

```
# be sure to run this AFTER kbf2
./build/thirdparty/libbf/bin/kbf ../../qtf/data/ecoli1.fasta 31 ../../qtf/data/ecoli2.fasta "results/test" > results/exeKbf.json
python3 bfAndKbfAnalyser.py
```

Caution: this will produce a huge amount of data on your disk if the fast you use itself is huge. If you do noy have enough space, kbf won't be able to write ist file to your disk, leading to a crash of the scrpt ```bfAndKbfAnalyser.py``` (see below)

It is not possible to pass directly the expected FPR for the filter. One you must use a size factor.
By noting:

    - m the number ofbits used for the filter 
    
    - n the number of element added in the filter
    
The size factor would be m/n.

Since the size factor is an integer, it is not possible to select some specific FPR for the filter. For instance, if:

    a size factor of 19 gives you a FPR of 5.12%
    
    and
    
    a size factor of 20 gives you a FPR of 4.8%
    
there is no way to have a FPR of 5% (since the size factor would have to be between 19 and 20, but htis is not possible as it is an integer).

## Compute the result
After the exectution, you can compute the false positive rate from the output of kbf using the python script:
```
python3 bfAndKbfAnalyser.py
```

## Get a better time estimation

Because kbf writes a lot on the disk, this can drastically change the time taken to perform a query. On commit d2296e08d11db8c3c7efb3a4012413569a408efb, kbf do not write on disk anymore, but since we used those files to get the FPR, the FPR is not available anymore at this commit. If using this commit, do:
```
python3 "bfAndKbfAnalyser_time only.py"
```
rather than:
```
python3 bfAndKbfAnalyser.py
```
Because bfAndKbfAnalyser will search files that do not exist anymore.
