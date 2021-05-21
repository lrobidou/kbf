# import subprocess

import analyse
import json


def print_size_fpr_and_time():

    print("size_factor", "classic", "kbf1", "time")
    with open("results/exeKbf.json", "r") as fichier:
        dico = json.load(fichier)

    print("For BF")
    print("size fpr time_index time_query")
    for size_factor in dico:
        fpr_filename = "results/test_" + size_factor + "_" + "classic.txt"

        print(
            dico[size_factor]["size"],
            analyse.analyser(fpr_filename)[1],
            dico[size_factor]["classic_index"],
            dico[size_factor]["classic_query"],
        )

    print("For kBF1")
    print("size fpr time_index time_query")
    for size_factor in dico:
        fpr_filename = "results/test_" + size_factor + "_" + "classic.txt"

        print(
            dico[size_factor]["size"],
            analyse.analyser(fpr_filename)[1],
            dico[size_factor]["kbf1_index"],
            dico[size_factor]["kbf1_query"],
        )

    print("For kBF2")
    print("size fpr time_index time_query")
    # read sizes from memused.sh
    sizes = []
    with open("results/spaceOf2Kbf.txt", "r") as fichier:
        for ligne in fichier:
            if ligne:
                sizes.append(int(ligne) * 8 * 1000)
    # print(sizes)

    index_size = 0
    for size_factor in dico:
        fpr_filename = "results/test_" + size_factor + "_" + "kbf2.txt"
        filename_result = "results/exe2kbf_" + size_factor + ".json"

        with open(filename_result, "r") as fichier:
            result_for_this_size_factor = json.load(fichier)

        print(
            sizes[index_size],
            analyse.analyser(fpr_filename)[1],
            result_for_this_size_factor[size_factor]["kbf2_query"],
            result_for_this_size_factor[size_factor]["kbf2_index"],
        )
        index_size += 1

    # for size_factor in dico:
    #     print(
    #         size_factor,
    #         dico[size_factor]["classic_index"],
    #         dico[size_factor]["classic_query"],
    #         dico[size_factor]["kbf1_index"],
    #         dico[size_factor]["kbf1_query"],
    #     )
    # for size_factor in size_factors:
    #     start_name = "results/test_" + str(size_factor) + "_"
    #     classic = start_name + "classic.txt"
    #     kbf1 = start_name + "kbf1.txt"
    #     kbf2 = start_name + "kbf2.txt"
    #     print(
    #         size_factor,
    #         analyse.analyser(classic)[1],
    #         analyse.analyser(kbf1)[1],
    #         analyse.analyser(kbf2)[1],
    #     )


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
    # size_factors = [3, 5, 7, 9, 15, 18, 21, 24]
    print_size_fpr_and_time()
    # print("\n\n\n\n\n")
    # print_time()


if __name__ == "__main__":
    main()
