# import subprocess

import analyse


def main():
    results = {}
    size_factors = [10, 20, 30]
    for size_factor in size_factors:
        start_name = "test_" + str(size_factor) + "_"
        classic = start_name + "classic.txt"
        kbf1 = start_name + "kbf1.txt"
        kbf2 = start_name + "kbf2.txt"
        results[size_factor] = {
            "classic": analyse.analyser(classic)[1],
            "kbf1": analyse.analyser(kbf1)[1],
            "kbf2": analyse.analyser(kbf2)[1],
        }
    print(results)


if __name__ == "__main__":
    main()
