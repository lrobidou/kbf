# don't mind me, I'm a quick and dirty python script
# I extract false positive rate from the output files of kbf


def lire(filemane):
    TP, TN, FP, FN = 0, 0, 0, 0
    with open(filemane, "r") as fichier:
        for ligne in fichier:
            cols = ligne.split()
            if cols[1] == "0":
                if cols[2] == "0":
                    TN += 1
                else:
                    FN += 1
            else:
                if cols[2] == "0":
                    FP += 1
                else:
                    TP += 1
    return TP, TN, FP, FN


def analyser(filename):
    TP, TN, FP, FN = lire(filename)
    return filename, (FP / (FP + TN)) * 100


def main():
    print(analyser("test_classic.txt"))
    print(analyser("test_kbf1.txt"))
    print(analyser("test_kbf2.txt"))


if __name__ == "__main__":
    main()