# import subprocess

import analyse
import json


def print_fpr(size_factors):
    print("printing FPRs (%)")
    print("size_factor", "classic", "kbf1")
    for size_factor in size_factors:
        start_name = "results/test_" + str(size_factor) + "_"
        classic = start_name + "classic.txt"
        kbf1 = start_name + "kbf1.txt"
        kbf2 = start_name + "kbf2.txt"
        print(
            size_factor,
            analyse.analyser(classic)[1],
            analyse.analyser(kbf1)[1],
            analyse.analyser(kbf2)[1],
        )


def print_time():
    print("printing time(ms)")
    print(
        "size_factor",
        "classic_index",
        "classic_query",
        "kbf1_index",
        "kbf1_query",
    )
    with open("results/exeKbf.json", "r") as fichier:
        dico = json.load(fichier)
    for size_factor in dico:
        print(
            size_factor,
            dico[size_factor]["classic_index"],
            dico[size_factor]["classic_query"],
            dico[size_factor]["kbf1_index"],
            dico[size_factor]["kbf1_query"],
        )


def main():
    size_factors = [3, 5, 7, 9, 15, 18, 21, 24]
    # print_fpr(size_factors)
    print("\n\n\n\n\n")
    print_time()


if __name__ == "__main__":
    main()
