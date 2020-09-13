#include <gtest/gtest.h>

#include "b-mqkd/bb84.h"

void renegotiate(bb84::server& s, bb84::client& c, vector<pair<int, int>>& photons,
                 vector<int>& polarizations, vector<u8>& key, int adjustment)
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
    bb84::client c;
    vector<pair<int, int>> photons;
    vector<int> polarizations;
    vector<u8> key;
    int adjustment = 0;

    s.generate_photons(photons, 10);
    c.choose_polarization(photons.size(), polarizations);

    do {
        // Optimize with hill climb
        if (key.size() == s.key_size())
            break;
        else if (key.size() < s.key_size())
            adjustment++;
        else if (key.size() > s.key_size())
            adjustment--;

        renegotiate(s, c, photons, polarizations, key, adjustment);
    } while (!s.check_polarizations(photons, polarizations, key));

    ASSERT_EQ(128, key.size() * 8);
}
