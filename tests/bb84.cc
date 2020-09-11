#include <gtest/gtest.h>

#include "b-mqkd/bb84.h"

TEST(BB84, GenerateRawKey)
{
    bb84::server s;
    vector<pair<int, int>> photons;
    s.generate_photons(photons, 256);
    bb84::client c;
    vector<int> polarizations;
    c.choose_polarization(photons, polarizations);
    vector<u8> key;
    s.check_polarizations(photons, polarizations, key);
}
