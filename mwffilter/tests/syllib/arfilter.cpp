/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#include "common/testutils.h"

#include "syllib/arfilter.h"

#include "tester.h"

using namespace syl;

void TestSyl::arFilter()
{
    TEST_EXCEPTIONS_TRY

    const double input[] = {-1.07159e-08, 4.40756e-08, -3.64765e-08,
        -9.13221e-08, 3.95817e-07, -7.26875e-07, -8.48536e-07, 1.31107e-06,
        9.32937e-07, -7.63544e-07, -7.95249e-07, 1.19425e-06, -2.41517e-06,
        4.66906e-06, -4.51008e-06, 3.87592e-06, -4.84247e-06, 7.15745e-06,
        -4.88649e-06, 2.81052e-06, -5.55279e-08, 1.9198e-06, -2.41794e-06,
        -7.38881e-07, 2.16369e-06, -1.12417e-06, -1.76223e-07, -1.39782e-06,
        2.12709e-06, -1.10498e-06, 2.72675e-06, -3.57742e-06, 5.3766e-06,
        -7.88675e-06, 8.84003e-06, -1.25658e-05, 1.58757e-05, -1.81965e-05,
        1.84746e-05, -1.67807e-05, 1.44413e-05, -1.14095e-05, 7.85082e-06,
        -4.88669e-06, 7.16487e-06, -8.89711e-06, 8.95455e-06, -8.49837e-06,
        7.23954e-06, -6.45112e-06, 6.80961e-06, -5.83037e-06, 5.41707e-06,
        -1.90748e-06, -7.11505e-08, -3.76491e-06, 7.63236e-06, -8.8041e-06,
        7.61853e-06, -1.26971e-05, 1.45091e-05, -1.94783e-05, 2.26864e-05,
        -2.72527e-05, 3.31434e-05, -4.09498e-05, 4.16457e-05, -3.80992e-05,
        3.76889e-05, -3.56194e-05, 1.79422e-05, -1.89789e-05, 0.000116532,
        -0.000194934, 0.000172274, 0.00138248, -0.000136119, -0.00487172,
        0.00179492, 0.00459847, -0.004687, 0.00450238, -0.00303145, -0.00235924,
        0.00314965, -0.000501862, -0.000279213, -0.00375209, 0.00641429,
        -0.00468293, -0.000403625, 0.0025209, 0.00531052, -0.00525882,
        -0.000761567, 0.00485603, -0.00343879, -0.000302219, -0.00164165,
        0.00124785, 0.00353358, -0.00809175, 0.00902654, -0.00235171,
        0.000132198, -0.00349098, 0.000930123, 0.0086896, -0.0181185, 0.0146843,
        -0.00532384, 3.27968e-05, 0.00150592, -0.00538494, 0.00488736,
        0.00303286, -0.00752211, 0.00493045, 0.00421056, -0.00716618,
        -0.00272028, 0.00893032, -0.00675333, -0.00011744, 0.00918774,
        -0.0103622, 0.00986736, -0.0116148};

    const double output[] = {-1.07159e-08, 2.3946e-08, 2.48685e-08,
        -9.05497e-08, 2.14839e-07, -1.82935e-07, -1.59336e-06, -1.17109e-06,
        8.3369e-07, 1.48655e-06, 3.08576e-07, 2.97582e-07, -1.28179e-06,
        1.58285e-06, 5.9476e-07, 1.21524e-06, -1.00004e-06, 1.92795e-06,
        3.07541e-06, 2.58995e-06, 3.90699e-06, 5.25191e-06, 4.26197e-06,
        8.12136e-07, 8.25735e-07, 1.08524e-06, 2.50701e-07, -2.20227e-06,
        -1.27941e-06, -2.60413e-07, 2.46928e-06, 1.12248e-06, 3.5826e-06,
        -3.47454e-07, 1.78991e-06, -4.61012e-06, 1.02468e-06, -4.76682e-06,
        -1.02806e-07, -2.81831e-06, -5.22508e-07, -1.94447e-07, -1.10316e-06,
        -6.18232e-08, 3.37591e-06, 3.6878e-07, 2.47015e-06, -9.21177e-08,
        4.51753e-07, -1.0072e-07, 2.66977e-07, 7.46723e-07, 1.31775e-06,
        3.39437e-06, 2.66056e-06, -1.75907e-06, 1.90643e-06, -1.04206e-06,
        2.47194e-07, -7.21707e-06, -3.30469e-06, -1.02294e-05, -2.10804e-06,
        -1.09778e-05, 2.8154e-06, -1.16141e-05, -5.94752e-07, -6.72798e-06,
        3.56246e-06, -1.73068e-06, -1.21533e-05, -1.90078e-05, 8.2638e-05,
        -8.68811e-06, 1.00094e-05, 0.00149881, 0.00260027, -0.00220522,
        -0.00508372, -9.62559e-05, 0.000263636, 0.00243306, 0.00190643,
        -0.0025128, -0.00122262, -0.000483836, 7.71103e-05, -0.0035662,
        -0.0018421, -0.000448467, -0.00289659, -0.00123081, 0.0062729,
        0.00654271, 0.00177678, 0.00324372, 0.0034924, 0.00152484, -0.00297213,
        -0.00431259, 0.00099267, -0.00324188, -7.25402e-05, 0.00460346,
        0.00446772, 0.00147187, -0.00301581, 0.00577203, -0.00371592,
        -0.00463377, -0.000711473, -0.00257981, 0.000735009, -0.00504802,
        -0.00419492, 0.00401777, -0.00143363, -0.00082753, 0.00555228,
        0.00217326, -0.00358941, -0.00210505, -0.00107819, -0.00319265,
        0.00268643, 0.000726246, 0.00505924, -0.000491618};

    const unsigned char dump[] = {
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x35, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0xf0, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0xf0, 0x3f, 0xa2, 0x47, 0xb9, 0x7a, 0x46, 0x0e, 0xfe, 0xbf, 0x73,
        0x7e, 0xdd, 0x61, 0x7e, 0x6e, 0xf8, 0x3f, 0x55, 0x79, 0x90, 0x69, 0x06,
        0x01, 0xec, 0xbf, 0x94, 0x4c, 0xfa, 0x58, 0xe5, 0x52, 0xe2, 0x3f, 0xc3,
        0x77, 0x1c, 0xc5, 0xc0, 0xc4, 0xe1, 0xbf, 0xd0, 0x9d, 0xc3, 0x82, 0xaa,
        0x12, 0xe5, 0x3f, 0x75, 0x49, 0xcc, 0xaa, 0x76, 0xc8, 0xe2, 0xbf, 0x4d,
        0x8b, 0xce, 0x0b, 0x9c, 0xc4, 0xd3, 0x3f, 0x3c, 0x11, 0xc7, 0xc5, 0x4d,
        0x69, 0xb0, 0xbf, 0x81, 0x2d, 0x38, 0x81, 0x30, 0x20, 0xb3, 0xbf, 0x79,
        0x0e, 0xaa, 0xfa, 0x81, 0xe3, 0x97, 0x3f, 0x55, 0xf2, 0xec, 0xd9, 0x3a,
        0x44, 0xbe, 0x3f, 0xa5, 0xec, 0xb8, 0x10, 0xd9, 0xf6, 0xc4, 0xbf, 0x01,
        0x86, 0x6b, 0x75, 0xe8, 0xa7, 0xc6, 0x3f, 0x60, 0xf0, 0x54, 0x46, 0xc8,
        0x2c, 0xc8, 0xbf, 0xdb, 0x96, 0x63, 0x0b, 0x46, 0x0d, 0xc2, 0x3f, 0x65,
        0x6a, 0x85, 0x50, 0x14, 0x46, 0xb7, 0xbf, 0x9d, 0x85, 0x23, 0xc9, 0x52,
        0x46, 0xba, 0x3f, 0xef, 0xd6, 0xff, 0x5b, 0x6e, 0xcc, 0xb1, 0xbf, 0xcc,
        0x18, 0x6f, 0x43, 0xbb, 0xec, 0x9e, 0x3f};

    const double taps[] = {       1.0000000000000000e+00,
        -1.8784851831337410e+00,  1.5269759962901872e+00,
        -8.7512512796437625e-01,  5.7261912706816753e-01,
        -5.5526770113260004e-01,  6.5852857150906097e-01,
        -5.8697064742072647e-01,  3.0887509491929138e-01,
        -6.4106808457897679e-02, -7.4709922359195616e-02,
         2.3328810619672489e-02,  1.1822860546539833e-01,
        -1.6378320042153863e-01,  1.7699914678000542e-01,
        -1.8886664804620334e-01,  1.4103007846931956e-01,
        -9.0913076075062785e-02,  1.0263554964651296e-01,
        -6.9525620899948612e-02,  3.0199933997004533e-02};

    QByteArray array (reinterpret_cast<const char*> (dump), sizeof (dump));

    { // Basic loading
        QBuffer buffer (&array);
        ArFilter filter (ArFilter::load (&buffer));
        QCOMPARE (filter.length(), 21u);
        ARRAYCOMP (filter.taps(), taps, 21);
    }

    { // Invalid number of channels
        QByteArray copy (array);
        copy[6] = char (0xf0);
        copy[7] = char (0x3f);
        QBuffer buffer (&copy);
        try {
            ArFilter filter (ArFilter::load (&buffer));
            QFAIL ("No std::exception for invalid channel number");
        } catch (const std::exception &) {
            // expected
        }
    }

    { // Invalid coefficients, b_0 != 1
        QByteArray copy (array);
        copy[23] = 0x40;
        QBuffer buffer (&copy);
        try {
            ArFilter filter (ArFilter::load (&buffer));
            QFAIL ("No std::exception for invalid coefficients");
        } catch (const std::exception &) {
            // expected
        }
    }

    { // Invalid coefficients, b_i != 0 | i > 0
        QByteArray copy (array);
        copy[31] = 0x40;
        QBuffer buffer (&copy);
        try {
            ArFilter filter (ArFilter::load (&buffer));
            QFAIL ("No std::exception for invalid coefficients");
        } catch (const std::exception &) {
            // expected
        }
    }

    { // Invalid coefficients, a_0 != 1
        QByteArray copy (array);
        copy[191] = 0x40;
        QBuffer buffer (&copy);
        try {
            ArFilter filter (ArFilter::load (&buffer));
            QFAIL ("No std::exception for invalid coefficients");
        } catch (const std::exception &) {
            // expected
        }
    }

    { // Output
        double copy[128];
        memcpy (copy, input, sizeof (copy));

        QBuffer buffer (&array);
        ArFilter filter (ArFilter::load (&buffer));
        QCOMPARE (filter.position(), 0u);
        filter.process (copy, 128);
        ARRAYFUZZCOMP (copy, output, 1e-7, 128);
        QCOMPARE (filter.position(), 128u);
    }

    TEST_EXCEPTIONS_CATCH
}
