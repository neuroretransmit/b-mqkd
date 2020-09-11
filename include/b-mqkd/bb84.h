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
    class server
    {
      public:
        server() {}

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

        void check_polarizations(const vector<pair<int, int>>& photons, const vector<int>& polarizations,
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
            convert_to_bytes(key_binary, key);
        }

      private:
        uniform_int_distribution<int> distribution = uniform_int_distribution<int>(1, 2);
        random_device r;
        default_random_engine generator = default_random_engine(r());

        void convert_to_bytes(const string& key, vector<u8>& bytes)
        {
            for (auto it = key.begin(); it < key.end(); it += 8) {
                string byte(it, it + 8);
                u8 b = stoi(byte, nullptr, 2);
                bytes.push_back(b);
            }

            log::data(bytes);
        }

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

        void show_photons(const vector<pair<int, int>>& photons)
        {
            for (const pair<int, int>& photon : photons)
                show_photon(photon.first, photon.second);
        }
    };

    class client
    {
      public:
        client() {}

        void choose_polarization(const vector<pair<int, int>>& photons, vector<int>& polarizations)
        {
            auto dice = bind(distribution, generator);
            for (size_t i = 0; i < photons.size(); i++)
                polarizations.push_back(dice());
            show_polarizations(polarizations);
        }

      private:
        void show_polarization(int polarization, stringstream& ss)
        {
            if (polarization == 1)
                ss << "+, ";
            else
                ss << "x, ";
        }

        void show_polarizations(const vector<int>& polarizations)
        {
            stringstream ss;
            ss << "Polarization: ";
            for (int polarization : polarizations)
                show_polarization(polarization, ss);
            log::info(ss.str());
        }

        uniform_int_distribution<int> distribution = uniform_int_distribution<int>(1, 2);
        default_random_engine generator;
    };
} // namespace bb84
