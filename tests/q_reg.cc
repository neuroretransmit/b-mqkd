#include <gtest/gtest.h>

#include <iostream>

#include <log/log.h>

#include "b-mqkd/q_reg.h"

using std::cout;

TEST(QReg, Measure)
{
    q_reg reg(5);
    reg.dump_state();

    // Expected to print
    if (reg.status() != STATUS_MEASURED)
        log::warn("Register not measured. use 'MEASURE'");

    if (reg.status() == STATUS_MEASURED)
        reg.dump_p_amps();

    if (reg.status() == STATUS_MEASURED) {
        bitset<16> binary(reg.measure());
        log::info(binary.to_string().substr(16 - reg.num_qubits()));
    } else {
        log::info("N/A");
    }

    log::info("Measuring the register : %d", reg.measure());
}
