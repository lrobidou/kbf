# How to reproduce the results of kbf

## install kbf
```
git clone --recursive https://github.com/lrobidou/kbf
cd kbf
chmod +x build.sh
./build.sh
```
## Execute kbf.
```
./build/thirdparty/libbf/bin/kbf /groups/genscale/NGSdatasets/metagenomics/hmp/SRS014107_SRS016349_fasta/SRS014107.denovo_duplicates_marked.trimmed.1.fasta 31 /groups/genscale/NGSdatasets/metagenomics/hmp/SRS014107_SRS016349_fasta/SRS016349.denovo_duplicates_marked.trimmed.1.fasta
```
This execution will produce 9 files. There are 3 loops over different size factors, and for each siez factor, there are three files (for the classic filter, the kbf1 filter and the kbf2 filter).

It is not possible to pass directly the expected FPR for the filter. Instead, you must pass the size factor.
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
After the exectution, you can compute the false positive rate from those file usint the python script:

python3 loopanalyser.py > kbf.json

The file kbf.json contains a json file, looking like:
```
{
    "19": {
        "classic": 5.127479316985625,
        "kbf1": 1.8254322167008583,
        "kbf2": 0.19984716721756995
    },
    "20": {
        "classic": 4.889611319887777,
        "kbf1": 1.6867815785476044,
        "kbf2": 0.17840099782338675
    },
    "21": {
        "classic": 4.6444216466795485,
        "kbf1": 1.5491686582682622,
        "kbf2": 0.15819432477859857
    }
}
```
Each inner object represent a loop with a size factor given as a key. For instance:
```
    "19": {
        "classic": 5.127479316985625,
        "kbf1": 1.8254322167008583,
        "kbf2": 0.19984716721756995
    },
```
means that with a size factor of 19 (i.e. by allocating (19 * number_of_elements) bits for the filter):

    - the classic bloom filter have a FPR of 5.12%
    
    - kbf1 gets a FPR of 1.83%
    
    - bkf2 gets a FPR of 0.2%

