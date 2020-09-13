#pragma once

#include <bitset>
#include <cmath>
#include <string>

#include "gates.h"
#include "qubit.h"

using std::bitset;
using std::string;

enum { STATUS_UNINITIALIZED = 0, STATUS_INITIALIZED = 1, STATUS_MEASURED = 2 };

class q_reg : private qubit, private gates
{

  private:
    int _num_qubits;
    int _status;

    gsl_matrix_complex* generate_gate_matrix(quantum_gate gate, int qubit, int qubit2 = -1)
    {

        gsl_matrix_complex* gate__matrix = get_gate_matrix(gate);

        if (_num_qubits == 1)
            return gate__matrix;

        gsl_matrix_complex* identity = gsl_matrix_complex_alloc(2, 2);
        gsl_matrix_complex_set_identity(identity);

        int i;

        gsl_matrix_complex* accum = NULL;

        if (gate == GATE_CNOT || gate == GATE_SWAP) {
            accum = this->compute_two_qubit_matrix(gate, qubit, qubit2);
        } else {
            for (i = 0; i < _num_qubits; i++) {

                if (_num_qubits - i - 1 == qubit) {

                    if (accum == NULL)
                        accum = gate__matrix;
                    else
                        accum = generate_kronecker(accum, gate__matrix);
                } else {
                    if (accum == NULL)
                        accum = identity;
                    else
                        accum = generate_kronecker(accum, identity);
                }
            }
        }

        gsl_matrix_complex_free(identity);
        gsl_matrix_complex_free(gate__matrix);

        return accum;
    }

    gsl_matrix_complex* compute_two_qubit_matrix(quantum_gate gate, int a, int b)
    {
        int i, m;

        gsl_matrix_complex* full_matrix = gsl_matrix_complex_alloc(pow(2, _num_qubits), pow(2, _num_qubits));

        gsl_matrix_complex* one_q_matrix = gsl_matrix_complex_alloc(1, 2);

        for (i = 0; i < pow(2, _num_qubits); i++) {

            gsl_matrix_complex* ind_matrix = NULL;

            bool a_th = (i & (1 << (a))) == 0 ? false : true;
            bool b_th = (i & (1 << (b))) == 0 ? false : true;

            for (m = 0; m < _num_qubits; m++) {

                bool m_th = (i & (1 << (_num_qubits - m - 1))) == 0 ? false : true;

                if (gate == GATE_SWAP) {

                    if (_num_qubits - 1 - m == a)
                        m_th = b_th;
                    else if (_num_qubits - 1 - m == b)
                        m_th = a_th;
                } else if (gate == GATE_CNOT) {

                    if (_num_qubits - 1 - m == b) {
                        if (a_th)
                            b_th = !b_th;
                        m_th = b_th ? 1 : 0;
                    }
                }

                if (m_th) {
                    gsl_matrix_complex_set(one_q_matrix, 0, 0, gsl_complex_rect(0, 0));
                    gsl_matrix_complex_set(one_q_matrix, 0, 1, gsl_complex_rect(1, 0));
                } else {
                    gsl_matrix_complex_set(one_q_matrix, 0, 0, gsl_complex_rect(1, 0));
                    gsl_matrix_complex_set(one_q_matrix, 0, 1, gsl_complex_rect(0, 0));
                }

                if (ind_matrix == NULL) {
                    ind_matrix = gsl_matrix_complex_alloc(1, 2);
                    gsl_matrix_complex_memcpy(ind_matrix, one_q_matrix);
                } else {
                    ind_matrix = generate_kronecker(ind_matrix, one_q_matrix);
                }
            }

            gsl_vector_complex* v = gsl_vector_complex_alloc(pow(2, _num_qubits));

            gsl_matrix_complex_get_row(v, ind_matrix, 0);
            gsl_matrix_complex_set_row(full_matrix, i, v);

            gsl_matrix_complex_free(ind_matrix);
            gsl_vector_complex_free(v);
        }
        gsl_matrix_complex_free(one_q_matrix);

        return full_matrix;
    }

    // TODO move to utils
    gsl_matrix_complex* generate_kronecker(gsl_matrix_complex* a, gsl_matrix_complex* b)
    {
        size_t i, j, k, l;

        gsl_matrix_complex* c = gsl_matrix_complex_alloc(a->size1 * b->size1, a->size2 * b->size2);
        gsl_complex first, second;

        for (i = 0; i < a->size1; i++) {
            for (j = 0; j < a->size2; j++) {
                first = gsl_matrix_complex_get(a, i, j);
                for (k = 0; k < b->size1; k++) {
                    for (l = 0; l < b->size2; l++) {
                        second = gsl_matrix_complex_get(b, k, l);
                        gsl_matrix_complex_set(c, b->size1 * i + k, b->size2 * j + l,
                                               gsl_complex_mul(first, second));
                    }
                }
            }
        }

        return c;
    }

  public:
    q_reg(int num_qubits = 5) : _num_qubits(num_qubits)
    {
        if (num_qubits == 0) {
            log::error("Quantum register size error, %d.", num_qubits);
            exit(-1);
        } else if (num_qubits > 5) {
            log::error("Register size error. Number of Qubits cannot exceed 5");
            exit(-1);
        }

        int num_states = pow(2, _num_qubits);
        _state = gsl_vector_complex_alloc(num_states);

        // Set state vector to |0>
        int i = 0;
        gsl_vector_complex_set(_state, i++, gsl_complex_rect(1, 0));

        for (; i < num_states; i++)
            gsl_vector_complex_set(_state, i, gsl_complex_rect(0, 0));

        _status = STATUS_INITIALIZED;
    }

    q_reg()
    {
        _num_qubits = 0;
        _status = STATUS_UNINITIALIZED;
    }

    int status() { return _status; }

    void dump_state()
    {
        stringstream ss;
        int num_states = pow(2, _num_qubits);
        ss << "Φ : ";

        for (int i = 0; i < num_states; i++) {

            // TODO find a better way to convert to binary
            bitset<16> binary(i);

            if (i > 0)
                ss << " + ";

            ss << GSL_REAL(gsl_vector_complex_get(_state, i));

            if (GSL_IMAG(gsl_vector_complex_get(_state, i)) != 0)
                ss << "+" << GSL_IMAG(gsl_vector_complex_get(_state, i)) << "i";

            ss << "|" << binary.to_string().substr(16 - _num_qubits) << "⟩";
        }

        log::info(ss.str());
    }

    void dump_p_amps()
    {

        int num_states = pow(2, _num_qubits);
        int i;

        for (i = 0; i < num_states; i++) {
            // TODO find a better way to convert to binary
            bitset<16> binary(i);
            cout << binary.to_string().substr(16 - _num_qubits) << " => "
                 << gsl_complex_abs2(gsl_vector_complex_get(_state, i)) << "\n";
        }

        cout << "\n";
    }

    void apply_gate(quantum_gate GATE, int qubit1, int qubit2 = -1)
    {

        int num_states = pow(2, _num_qubits);
        gsl_matrix_complex* gate_matrix = generate_gate_matrix(GATE, qubit1, qubit2);

        // Create an output temp qubit
        gsl_vector_complex* op_qubit = NULL;
        op_qubit = gsl_vector_complex_alloc(num_states);
        gsl_vector_complex_set_all(op_qubit, GSL_COMPLEX_ZERO);

        // get cross product
        gsl_blas_zgemv(CblasNoTrans, GSL_COMPLEX_ONE, gate_matrix, _state, GSL_COMPLEX_ZERO, op_qubit);
        gsl_matrix_complex_free(gate_matrix);
        gsl_vector_complex_memcpy(_state, op_qubit);
        gsl_vector_complex_free(op_qubit);
    }

    int measure()
    {
        int num_states = pow(2, _num_qubits);
        int i;
        const gsl_rng_type* type;
        gsl_rng* rng;
        gsl_rng_env_setup();
        gsl_rng_default_seed = time(NULL);
        type = gsl_rng_default;
        rng = gsl_rng_alloc(type);

        double u_rand = gsl_rng_uniform(rng);

        gsl_rng_free(rng);

        double circle = 0;

        for (i = 0; i < num_states; i++) {
            circle += gsl_pow_2(gsl_complex_abs(gsl_vector_complex_get(_state, i)));

            if (circle >= u_rand) {
                _status = STATUS_MEASURED;
                return i;
            }
        }

        _status = STATUS_MEASURED;
        return num_states;
    }

    int num_qubits() { return _status; }

    string measure_bit()
    {
        bitset<16> binary(measure());
        return binary.to_string().substr(16 - _num_qubits);
    }
};
