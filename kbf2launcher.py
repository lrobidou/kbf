import os
import json

size_factors = [3, 5, 7, 9, 15, 18, 21, 24]


def exe_2kbf():
    for size_factor in size_factors:
        cmd = (
            "./memused.sh ./build/thirdparty/libbf/bin/kbf2only ../../qtf/data/ecoli1.fasta 31 ../../qtf/data/ecoli2.fasta "
            + str(size_factor)
            + ' "results/test" >> results/spaceOf2Kbf.txt'
        )
        print(cmd)
        os.system(cmd)


def print_2kbf_results():

    print("time (ms)")
    print("index", "query")
    for size_factor in size_factors:

        with open("results/exe2kbf_" + str(size_factor) + ".json") as fichier:
            result = json.load(fichier)
        print(
            result[str(size_factor)]["kbf2_index"],
            result[str(size_factor)]["kbf2_query"],
        )

    print("\n\n")

    print("space (mb)")
    with open("results/spaceOf2Kbf.txt", "r") as fichier:
        for size_factor, line in zip(size_factors, fichier):
            print(size_factor, int(line))


def main():
    exe_2kbf()
    print_2kbf_results()


if __name__ == "__main__":
    main()
