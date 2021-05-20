/*************************************************************
* Copyright (c) David Pellow, Darya Filippova, Carl Kingsford
*************************************************************/
#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>
//
#include "BaseBloomFilter.hpp"
#include "KBF1.hpp"
#include "KBF2.hpp"
#include "KBFSparse.hpp"
#include "KBFSparseRelaxed.hpp"
#include "KBFUtil.hpp"
//
#include "JellyfishUtil.h"
using namespace std;

////////////////////////////////////////////////////////////////////////////////
// query a set of test kmers and write out results
////////////////////////////////////////////////////////////////////////////////
void queryKmers(vector<kmer_t>& test_kmers, unordered_set<kmer_t>& true_kmers, BaseBloomFilter& sbf, const string& out_fname) {
    vector<bool> states;
    // time this part
    auto start = std::chrono::system_clock::now();
    for (auto qk : test_kmers) {
        bool state = sbf.contains(qk);
        states.push_back(state);
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    // cerr << "query time: " << elapsed_seconds.count() << " s" << endl;
    // end the timing here

    // write states and true answers to file
    ofstream f_out(out_fname);
    f_out << "kmer\tBF_state\ttrue_state" << endl;
    for (int i = 0; i < states.size(); i++)
        f_out << test_kmers[i] << "\t" << states[i] << "\t" << (true_kmers.find(test_kmers[i]) != true_kmers.end()) << endl;
    f_out.close();
}

////////////////////////////////////////////////////////////////////////////////
// Sample a subset of the kmers
////////////////////////////////////////////////////////////////////////////////
vector<kmer_t> sample_kmers(unordered_set<kmer_t>& kmer_set, int const set_size, const int K, bool TP = false) {
    vector<kmer_t> query_kmers;

    // store the kmer set in a vector so that can sample from it
    vector<kmer_t> kmer_vec;
    for (auto km : kmer_set) kmer_vec.push_back(km);

    // set up a random number gen
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> sample_dis(0, kmer_set.size() - 1);
    std::uniform_int_distribution<> shift_dis(0, K - 1);
    std::uniform_int_distribution<> base_dis(0, 3);
    const char base_table[4] = {'A', 'C', 'G', 'T'};

    //sample the input kmers
    int i = query_kmers.size();
    while (i < set_size) {
        auto r = sample_dis(gen);
        assert(r < kmer_set.size());
        kmer_t sample_kmer = kmer_vec[r];
        // mutate the sampled kmer
        if (!TP) {
            string string_kmer = mer_binary_to_string(sample_kmer, K);
            auto base = base_table[base_dis(gen)];
            auto ind = shift_dis(gen);
            while (string_kmer[ind] == base) {
                base = base_table[base_dis(gen)];
            }
            string_kmer[ind] = base;
            sample_kmer = mer_string_to_binary(string_kmer.c_str(), K);
        }
        query_kmers.push_back(sample_kmer);
        i++;
    }
    return query_kmers;
}

/////////////////////////////////////////////////////////
// main
/////////////////////////////////////////////////////////
// Usage:
//./kbf <input fasta> <query fasta> <k> [outfile prefix = 'test'] [# queries = 1000000] [use all TP = false]
int main(int argc, char* argv[]) {
    if (argc < 5) {
        cerr << "\tMissing required arguments." << endl;
        cerr << "\tUsage:" << endl;
        cerr << "\tkbf <reads.fa> <k> <query.fa> <size_factor> [outfile prefix = 'test'] [# queries = 1M] [use all TP = 'false']" << endl;
        exit(1);
    }

    string input_fasta = argv[1];
    int K = stoi(argv[2]);
    unsigned long query_set_size = 1000000;
    string base_prefix = "test";
    string queryFilename = "";
    size_t size_factor = 0;
    bool TP = false;
    if (argc > 3) {
        queryFilename = argv[3];
    }
    if (argc > 4) {
        size_factor = stoi(argv[4]);
    }
    if (argc > 5) {
        base_prefix = argv[5];
    }
    if (argc > 6) {
        query_set_size = stoi(argv[6]);
    }
    if (argc > 7) {
        string TP_string = argv[7];
        if (TP_string.compare("true") == 0)
            TP = true;
        else
            assert(TP_string.compare("false") == 0);
    }
    unordered_set<kmer_t> read_kmers;
    vector<string> reads = parseFasta(input_fasta);

    vector<kmer_t> query_kmers = getKmersVect(parseFasta(queryFilename), K);

    ofstream f_out("results/exe2kbf_" + std::to_string(size_factor) + ".json");
    f_out << "{" << std::endl;

    f_out << "    \"" << size_factor << "\": {" << std::endl;
    std::string prefix = base_prefix + "_" + std::to_string(size_factor);

    {
        unordered_set<kmer_t> edge_kmers;
        read_kmers.clear();
        auto start = std::chrono::high_resolution_clock::now();
        getKmersAndEdgeKmers(reads, K, 1, read_kmers, edge_kmers);
        f_out << "        \"number_of_elements\": " << read_kmers.size() << "," << std::endl;

        KBF2 kbf2(K, read_kmers, edge_kmers, 1, size_factor);
        auto end = std::chrono::high_resolution_clock::now();
        f_out << "        \"kbf2_index\": " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "," << endl;
        // cerr << "Potential edge kmers: " << edge_kmers.size() << endl;Z
        start = std::chrono::high_resolution_clock::now();
        queryKmers(query_kmers, read_kmers, kbf2, prefix + "_kbf2.txt");
        end = std::chrono::high_resolution_clock::now();
        f_out << "        \"kbf2_query\": " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << endl;
    }
    f_out << "    }";
    f_out << std::endl;

    f_out << "}";
    f_out.close();
}
