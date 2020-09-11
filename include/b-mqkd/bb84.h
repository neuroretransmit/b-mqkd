#pragma once

#include <bitset>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include <log/log.h>

using std::bind;
using std::default_random_engine;
using std::pair;
using std::random_device;
using std::uniform_int_distribution;

/// BB84 quantum key distribution scheme simulation
namespace bb84
{
    // Initiator side of BB84 protocol
    class server
    {
      public:
        /**
         * Constructor for initiator side of BB84 protocol
         * @param key_size: key size in bits
         */
        server(size_t key_size) : KEY_SIZE(key_size) {}

        /**
         * Generate base photons for initiator side
         * @param photons: vector to store photons in
         * @param n: number of photons to create
         */
        void generate_photons(vector<pair<int, int>>& photons, size_t n)
        {
            auto dice = bind(distribution, generator);
            log::info("Photons generating...");

            for (size_t i = 0; i < n; i++) {
                pair<int, int> photon;
                photon.first = dice();
                photon.second = dice();
                photons.push_back(photon);
            }
        }

        /**
         * Check polarizations received from initiate and create secret key from truthiness
         * @param photons: generated photons
         * @param polarizations: initiate's polarizations
         * @param key: vector of bytes to store key in
         */
        bool check_polarizations(const vector<pair<int, int>>& photons, const vector<int>& polarizations,
                                 vector<u8>& key)
        {
            stringstream ss;
            ss << "Correct polarization checking (1 - correct, 0 - not): ";

            vector<bool> correct;
            for (size_t i = 0; i < photons.size(); i++) {
                if (photons[i].first == polarizations[i])
                    correct.push_back(1);
                else
                    correct.push_back(0);
            }

            for (size_t i = 0; i < correct.size(); i++)
                ss << correct[i] << ", ";

            log::info(ss.str());

            string key_binary = "";
            for (size_t i = 0; i < correct.size(); i++)
                if (correct[i])
                    key_binary += (photons[i].second == 2) ? '0' : '1';

            log::info("Binary key: %s", key_binary.c_str());
            log::info("Key bit length: %lu", key_binary.size());

            if (key_binary.size() < KEY_SIZE)
                return false;

            convert_to_bytes(key_binary, key);
            return true;
        }

        size_t key_size() const { return KEY_SIZE; }

      private:
        const size_t KEY_SIZE;
        uniform_int_distribution<int> distribution = uniform_int_distribution<int>(1, 2);
        random_device r;
        default_random_engine generator = default_random_engine(r());

        /**
         * Convert bitstring to vector of bytes
         * @param key: secret key bitstring
         * @param bytes: vector of bytes to store key in
         */
        void convert_to_bytes(const string& key, vector<u8>& bytes)
        {
            for (auto it = key.begin(); it < key.end(); it += 8) {
                string byte(it, it + 8);
                u8 b = stoi(byte, nullptr, 2);
                bytes.push_back(b);
            }

            log::data(bytes);
        }

        /**
         * Show string representation of a single photon
         * @param f: first element of photon
         * @param s: second element of photon
         */
        void show_photon(int f, int s)
        {
            if (f == 1) {
                cout << "(+, ";

                if (s == 1)
                    cout << "->) ; ";
                else
                    cout << "|) ; ";
            } else {
                cout << "(x, ";

                if (s == 1)
                    cout << "|r) ; ";
                else
                    cout << "|l) ; ";
            }
        }

        /**
         * Show a vector of photons
         * @param photons: generated photons
         */
        void show_photons(const vector<pair<int, int>>& photons)
        {
            for (const pair<int, int>& photon : photons)
                show_photon(photon.first, photon.second);
        }
    };

    // Initiate side of BB84 protocol
    class client
    {
      public:
        /// Constructor for initiate side of BB84 protocol
        client() {}

        /**
         * Create polarizatios of same length as initiator's photons
         * @param n: number of photons
         * @param polarizations: vector to store polarizations
         */
        void choose_polarization(size_t n, vector<int>& polarizations)
        {
            auto dice = bind(distribution, generator);
            for (size_t i = 0; i < n; i++)
                polarizations.push_back(dice());
            show_polarizations(polarizations);
        }

      private:
        /**
         * Show string representation of single polarization
         * @param polarization: polarization value
         * @param ss: stringstream to continue writing output to
         */
        void show_polarization(int polarization, stringstream& ss)
        {
            if (polarization == 1)
                ss << "+, ";
            else
                ss << "x, ";
        }

        /**
         * Show string representation of all polarizations
         * @param polarizations: polarizations
         */
        void show_polarizations(const vector<int>& polarizations)
        {
            stringstream ss;
            ss << "Polarization: ";
            for (int polarization : polarizations)
                show_polarization(polarization, ss);
            log::info(ss.str());
        }

        
        random_device r;
        default_random_engine generator = default_random_engine(r());
        uniform_int_distribution<int> distribution = uniform_int_distribution<int>(1, 2);
    };
} // namespace bb84
