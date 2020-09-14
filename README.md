# b-mqkd (In progress)

Header only C++ implementation of [Multiparty Quantum Cryptography with Block Cipher RC6 (B-MQKD)](https://gitlab.com/optimisticninja/b-mqkd/-/blob/master/doc/Multiparty%20Quantum%20Cryptography%20with%20Block%20Cipher%20RC6%20(B-MQKD).pdf). Currently only the BB84 **simulatied** protocol is written. 

To actually achieve quantum key derivation, hardware is needed and trust of the "witness" (node observing the no-cloning property from an analog device) is a requirement of this project.

See [issue #3 here](https://gitlab.com/optimisticninja/b-mqkd/-/issues/3) for investigations into cheap hardware implementations.

## Usage

### Documentation

Doxygen generated files are in [`doc/latex`](doc/latex) and [`doc/html`](doc/html).

### Key Exchange

```cpp
#include <b-mqkd/b-mqkd.h>

int main()
{
    // Server-side, generate photons
    bb84::server s;
    vector<pair<int, int>> photons;
    s.generate_photons(photons, 256);
    
    // Client-side, choose polarizations
    bb84::client c;
    vector<int> polarizations;
    c.choose_polarization(photons, polarizations);
    
    // Server-side, check polarizations and create  key
    vector<u8> key;
    s.check_polarizations(photons, polarizations, key);
    
    // TODO: Implement encryption/decryption process from B-MQKD paper
}
```

## Building/Installation

### Requirements

* CMake >= 3.13
* GTest
* GMock

### The Build

```bash
$ mkdir build && cd build # Create and move into build directory
$ cmake ..                # Configure
$ make                    # Build
$ sudo make install       # Install headers
```

The headers are now in `/usr/local/include/b-mqkd`.

### Uninstall

```bash
$ sudo rm -rf /usr/local/include/b-mqkd
```

## Running (from `build/` folder)

### Tests

Tests run a Google Test suite that test constraints of the paper as well as the test vectors.

```bash
$ ./tests/tests
[==========] Running X tests from Y test suites.
[----------] Global test environment set-up.
[----------] Z tests from BMQKD
[ RUN      ] ...
[       OK ] ... (0 ms)
...
...
...
[----------] Global test environment tear-down
[==========] X tests from Y test suites ran. (4 ms total)
[  PASSED  ] X tests.

...
[  PASSED  ] X tests.
```

## License

None.
