#pragma once

#include <gsl/gsl_blas.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include <iostream>

#include "qubit.h"

typedef enum {
    GATE_HARDAMARD = 1,
    GATE_PAULI_X = 2,
    GATE_PAULI_Y = 6,
    GATE_PAULI_Z = 7,
    GATE_CNOT = 3,
    GATE_SWAP = 4,
    GATE_T = 5
} quantum_gate;

class gates
{
  protected:
    gsl_matrix_complex* get_gate_matrix(quantum_gate gate_name)
    {
        switch (gate_name) {
        case GATE_HARDAMARD: {
            gsl_matrix_complex* hardmard_matrix = gsl_matrix_complex_alloc(2, 2);
            gsl_matrix_complex_set(hardmard_matrix, 0, 0, gsl_complex_rect(1, 0));
            gsl_matrix_complex_set(hardmard_matrix, 0, 1, gsl_complex_rect(1, 0));
            gsl_matrix_complex_set(hardmard_matrix, 1, 0, gsl_complex_rect(1, 0));
            gsl_matrix_complex_set(hardmard_matrix, 1, 1, gsl_complex_rect(-1, 0));
            gsl_matrix_complex_scale(hardmard_matrix, gsl_complex_rect(M_SQRT1_2, 0));
            return hardmard_matrix;
        }
        case GATE_PAULI_X: {
            gsl_matrix_complex* pauli_x_matrix = gsl_matrix_complex_alloc(2, 2);
            gsl_matrix_complex_set(pauli_x_matrix, 0, 0, gsl_complex_rect(0, 0));
            gsl_matrix_complex_set(pauli_x_matrix, 0, 1, gsl_complex_rect(1, 0));
            gsl_matrix_complex_set(pauli_x_matrix, 1, 0, gsl_complex_rect(1, 0));
            gsl_matrix_complex_set(pauli_x_matrix, 1, 1, gsl_complex_rect(0, 0));
            return pauli_x_matrix;
        }
        case GATE_PAULI_Y: {
            gsl_matrix_complex* pauli_y_matrix = gsl_matrix_complex_alloc(2, 2);
            gsl_matrix_complex_set(pauli_y_matrix, 0, 0, gsl_complex_rect(0, 0));
            gsl_matrix_complex_set(pauli_y_matrix, 0, 1, gsl_complex_rect(0, -1));
            gsl_matrix_complex_set(pauli_y_matrix, 1, 0, gsl_complex_rect(1, 0));
            gsl_matrix_complex_set(pauli_y_matrix, 1, 1, gsl_complex_rect(0, 0));
            return pauli_y_matrix;
        }
        case GATE_PAULI_Z: {
            gsl_matrix_complex* pauli_z_matrix = gsl_matrix_complex_alloc(2, 2);
            gsl_matrix_complex_set(pauli_z_matrix, 0, 0, gsl_complex_rect(1, 0));
            gsl_matrix_complex_set(pauli_z_matrix, 0, 1, gsl_complex_rect(0, 0));
            gsl_matrix_complex_set(pauli_z_matrix, 1, 0, gsl_complex_rect(0, 0));
            gsl_matrix_complex_set(pauli_z_matrix, 1, 1, gsl_complex_rect(-1, 0));
            return pauli_z_matrix;
        }
        case GATE_T: {
            double gate_angle = 0;

            if (gate_name == GATE_T)
                gate_angle = M_PI_4;

            gsl_matrix_complex* gate_matrix = gsl_matrix_complex_alloc(2, 2);
            gsl_matrix_complex_set_all(gate_matrix, gsl_complex_rect(0, 0));
            gsl_matrix_complex_set(gate_matrix, 0, 0, gsl_complex_rect(1, 0));
            gsl_matrix_complex_set(gate_matrix, 1, 1, gsl_complex_rect(cos(gate_angle), sin(gate_angle)));
            return gate_matrix;
        }
        default:
            break;
        }

        return NULL;
    }

  public:
    gates() {}
    virtual ~gates() {}

    static bool get_measure(qubit& q)
    {

        // TODO Better random and measure implementation

        gsl_vector_complex* state = q.state();
        double amp_0 = gsl_complex_abs2(gsl_vector_complex_get(state, 0));

        const gsl_rng_type* T;
        gsl_rng* r;
        gsl_rng_env_setup();
        gsl_rng_default_seed = time(NULL);
        T = gsl_rng_default;
        r = gsl_rng_alloc(T);

        double u = gsl_rng_uniform(r);

        gsl_rng_free(r);

        if (u >= amp_0)
            return true;

        return false;
    }
};
