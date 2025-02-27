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
    if (argc < 4) {
        cerr << "\tMissing required arguments." << endl;
        cerr << "\tUsage:" << endl;
        cerr << "\tkbf <reads.fa> <k> <query.fa> [outfile prefix = 'test'] [# queries = 1M] [use all TP = 'false']" << endl;
        exit(1);
    }

    string input_fasta = argv[1];
    int K = stoi(argv[2]);
    unsigned long query_set_size = 1000000;
    string base_prefix = "test";
    string queryFilename = "";
    bool TP = false;
    if (argc > 3) {
        queryFilename = argv[3];
    }
    if (argc > 4) {
        base_prefix = argv[4];
    }
    if (argc > 5) {
        query_set_size = stoi(argv[5]);
    }
    if (argc > 6) {
        string TP_string = argv[6];
        if (TP_string.compare("true") == 0)
            TP = true;
        else
            assert(TP_string.compare("false") == 0);
    }
    unordered_set<kmer_t> read_kmers;

    // parse input reads -- will build a kmer set from that
    // cerr << "Parsing input fasta " << input_fasta << endl;
    // auto start = std::chrono::system_clock::now();
    vector<string> reads = parseFasta(input_fasta);
    // auto end = std::chrono::system_clock::now();
    // std::chrono::duration<double> elapsed_seconds = end - start;
    // cerr << "Parsing: " << elapsed_seconds.count() << " s" << endl;
    // cerr << "Getting kmers... " << endl;
    // start = std::chrono::system_clock::now();
    read_kmers = getKmers(reads, K);
    // end = std::chrono::system_clock::now();
    // elapsed_seconds = end - start;
    // cerr << "Input kmers: " << read_kmers.size() << endl;
    // cerr << "Getting kmers: " << elapsed_seconds.count() << " s" << endl;

    // parse test reads and count the test kmers
    // cerr << "Generating test kmers..." << endl;
    // vector<kmer_t> query_kmers = sample_kmers(read_kmers, query_set_size, K, TP);
    vector<kmer_t> query_kmers = getKmersVect(parseFasta(queryFilename), K);
    // const std::vector<size_t> size_factors = {7, 8, 9, 10};  //1 hash function: []
    const std::vector<size_t> size_factors = {26, 28, 30, 32, 35, 40};
    std::cout << "{" << std::endl;

    for (auto size_factor : size_factors) {
        std::cout << "    \"" << size_factor << "\": {" << std::endl;
        std::string prefix = base_prefix + "_" + std::to_string(size_factor);
        {
            // Test the classic bloom filter
            // cerr << "#### CLASSIC BLOOM FILTER ####" << endl;
            auto start = std::chrono::high_resolution_clock::now();
            BaseBloomFilter b(K, read_kmers.size(), size_factor);
            b.populate(read_kmers);
            auto end = std::chrono::high_resolution_clock::now();

            cout << "        \"classic_index\": " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "," << endl;
            start = std::chrono::high_resolution_clock::now();
            cout << "        \"number_of_elements\": " << read_kmers.size() << "," << std::endl;
            cout << "        \"size\": " << read_kmers.size() * size_factor << "," << std::endl;
            queryKmers(query_kmers, read_kmers, b, prefix + "_classic.txt");
            end = std::chrono::high_resolution_clock::now();
            cout << "        \"classic_query\": " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "," << endl;
        }

        {
            // Test KBF1
            // cerr << "#### KBF1 ####" << endl;
            auto start = std::chrono::high_resolution_clock::now();
            KBF1 kbf1(K, read_kmers, size_factor, 1);
            auto end = std::chrono::high_resolution_clock::now();
            cout << "        \"kbf1_index\": " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "," << endl;
            start = std::chrono::high_resolution_clock::now();
            queryKmers(query_kmers, read_kmers, kbf1, prefix + "_kbf1.txt");
            end = std::chrono::high_resolution_clock::now();
            cout << "        \"kbf1_query\": " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "" << endl;
        }

        std::cout << "    }";
        if (size_factor < 40) {
            cout << ",";
        }

        cout << std::endl;
    }
    cout << "}";
}
