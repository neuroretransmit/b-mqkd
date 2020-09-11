#include <gtest/gtest.h>

#include "b-mqkd/bb84.h"

TEST(BB84, GenerateRawKey)
{
    bb84::server s(128);
    vector<pair<int, int>> photons;
    s.generate_photons(photons, 256);
    bb84::client c;
    vector<int> polarizations;
    c.choose_polarization(photons.size(), polarizations);
    vector<u8> key;
    int ret = s.check_polarizations(photons, polarizations, key);
    ASSERT_TRUE(ret == -1 || ret > 0);
}

void renegotiate(bb84::server& s, bb84::client& c, vector<pair<int, int>>& photons, vector<int>& polarizations, vector<u8>& key, int adjustment)
{
    photons.clear();
    polarizations.clear();
    key.clear();
    s.generate_photons(photons, (s.key_size()) + adjustment);
    c.choose_polarization(photons.size(), polarizations);
    
}

TEST(BB84, RenegotiateOfKeySize)
{
    bb84::server s(128);
    vector<pair<int, int>> photons;
    s.generate_photons(photons, s.key_size() * 2);
    bb84::client c;
    vector<int> polarizations;
    c.choose_polarization(photons.size(), polarizations);
    vector<u8> key;
    
    int adjustment = 1;
    while (s.check_polarizations(photons, polarizations, key) != (int) s.key_size()) {
        if (key.size() < s.key_size()) {
            adjustment++;
        } else if (key.size() > s.key_size()) {
            adjustment--;
        }
        
        renegotiate(s, c, photons, polarizations, key, adjustment);
    }
    
    ASSERT_EQ(128, key.size() * 8);
}
