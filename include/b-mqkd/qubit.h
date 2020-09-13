#pragma once

#include <exception>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_rng.h>
#include <iostream>
#include <random>
#include <vector>

#include <log/log.h>

using std::cout;
using std::default_random_engine;
using std::exception;
using std::random_device;
using std::uniform_real_distribution;
using std::vector;

/*
ok i could write that more clearly

it is 2 outcomes on a single superposition, which is a state, that carries 2 corresponding square rooted
probabilities that add up to 1

with something like |psi> = 1/sqrt(2)|0> + 1/sqrt(2)|1> you would not have any particular determined outcome
of 0 or 1 if i told you to measure it, but over time, the outcomes of 0 and 1 would roughly equal each other,
while in the case of 1/2|0> + sqrt(3)/2|1> you'd get the 0 state overall a quarter of the time, while getting
the 1 state 3/4ths of the time
*/

typedef vector<vector<float>> hilbert_space;

class qubit
{
  public:
    qubit() : _state(gsl_vector_complex_alloc(2))
    {
        gsl_complex alpha = gsl_complex_rect(1, 0);
        gsl_complex beta = gsl_complex_rect(0, 0);
        gsl_vector_complex_set(_state, 0, alpha);
        gsl_vector_complex_set(_state, 1, beta);
    }

    qubit(double l_alpha, double l_beta, double l_alpha_imag = 0, double l_beta_imag = 0)
    {
        _state = gsl_vector_complex_alloc(2);
        gsl_complex alpha = gsl_complex_rect(l_alpha, l_alpha_imag);
        gsl_complex beta = gsl_complex_rect(l_beta, l_beta_imag);
        gsl_vector_complex_set(_state, 0, alpha);
        gsl_vector_complex_set(_state, 1, beta);
    }

    gsl_vector_complex* state() { return _state; }

    bool state(gsl_vector_complex* new_state)
    {
        gsl_vector_complex_free(_state);
        _state = new_state;
        return true;
    }

  protected:
    gsl_vector_complex* _state;
};
