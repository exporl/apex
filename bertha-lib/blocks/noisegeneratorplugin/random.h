/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 * Documentation Copyright (C) 2002  Free Software Foundation, under GPL      *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or          *
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

/** @file
 * Java-style random generator declaration.
 */

#ifndef _BERTHA_SRC_BLOCKS_NOISEGENERATORPLUGIN_RANDOM_H_
#define _BERTHA_SRC_BLOCKS_NOISEGENERATORPLUGIN_RANDOM_H_

#include <QtGlobal>

class RandomPrivate;

/** This class generates pseudorandom numbers. It uses the same algorithm as the
 * Java java.util.Random class, so that your programs behave exactly the same
 * way, if started with the same seed. The algorithm is described in <em>The Art
 * of Computer Programming, Volume 2</em> by Donald Knuth in Section 3.2.1. It
 * is a 48-bit seed, linear congruential formula. If two instances of this class
 * are created with the same seed and the same calls to these classes are made,
 * they behave exactly the same way. If you want to implement your own
 * pseudorandom algorithm, you should extend this class and overload the
 * #next() and #setSeed() methods. This class shouldn't be used for security
 * sensitive purposes(like generating passwords or encryption keys).
 */
class Random
{
public:
    /** Creates a new pseudorandom number generator. The seed is initialized to
     * the current time, as if by
     * <tt>setSeed(QDateTime::currentDateTime().toTime_t());</tt>.
     */
    Random();

    /** Creates a new pseudorandom number generator, starting with the
     * specified seed, using <code>setSeed(seed);</code>.
     *
     * @param seed the initial seed
     */

    Random(quint64 seed);

    /** Virtual destructor to make the compiler happy. */
    virtual ~Random();

    /** Fills an array of bytes with random numbers. All possible values are
     * (approximately) equally likely.
     *
     * @param bytes the byte array that should be filled
     * @param size the number of random bytes that should be generated
     */
    void nextBytes(char *bytes, unsigned size);

    /** Generates the next pseudorandom boolean. True and false have the same
     * probability.
     *
     * @return the next pseudorandom boolean
     */
    bool nextBool();

    /** Generates the next pseudorandom double uniformly distributed between 0.0
     * (inclusive) and 1.0 (exclusive).
     *
     * @return the next pseudorandom double
     */
    double nextDouble();

    /** Generates the next pseudorandom double uniformly distributed between the
     * parameters @a min(inclusive) and @a max(exclusive).
     *
     * @param min minimum value for the pseudorandom double
     * @param max maximum value for the pseudorandom double
     * @return the next pseudorandom double from the given range
     */
    double nextDouble(double min, double max);

    /** Generates the next pseudorandom float uniformly distributed
     * between 0.0f(inclusive) and 1.0f(exclusive).
     *
     * @return the next pseudorandom float
     */
    float nextFloat();

    /** Generates the next pseudorandom float uniformly distributed between the
     * parameters @a min(inclusive) and @a max(exclusive).
     *
     * @param min minimum value for the pseudorandom float
     * @param max maximum value for the pseudorandom float
     * @return the next pseudorandom float from the given range
     */
    float nextFloat(double min, double max);

    /** Generates the next pseudorandom, Gaussian(normally) distributed double
     * value, with mean 0.0 and standard deviation 1.0.
     *
     * This is described in section 3.4.1 of <em>The Art of Computer
     * Programming, Volume 2</em> by Donald Knuth.
     *
     * @return the next pseudorandom Gaussian distributed double
     */
    double nextGaussian();

    /** Generates the next pseudorandom number. This returns an int value whose
     * 32 bits are independent chosen random bits(0 and 1 are equally likely).
     *
     * @return the next pseudorandom value
     */
    int nextInt();

    /** Generates the next pseudorandom number. This returns an unsigned value
     * whose 32 bits are independent chosen random bits(0 and 1 are equally
     * likely).
     *
     * @return the next pseudorandom value
     */
    unsigned nextUInt();

    /** Generates the next pseudorandom number. This returns
     * a value between 0(inclusive) and <code>n</code>(exclusive), and
     * each value has the same likelihodd(1/<code>n</code>).
     *
     * The algorithm would return every value with exactly the same probability,
     * if the #next()-method would be a perfect random number generator.
     *
     * The loop at the bottom of the implementation only accepts a value, if the
     * random number was between 0 and the highest number less then 1<<32, which
     * is divisible by n. The probability for this is high for small n, and the
     * worst case is 1/2 (for n=(1<<31)+1).
     *
     * A special treatment for n = power of 2, selects the high bits of the
     * random number(the loop at the bottom would select the low order bits).
     * This is done, because the low order bits of linear congruential number
     * generators(like the one used in this class) are known to be "less
     * random" than the high order bits.
     *
     * @param n the upper bound
     * @return the next pseudorandom value
     */
    unsigned nextUInt(unsigned n);

    /** Generates the next pseudorandom long long(64 bit) number. All bits of
     * this long are independently chosen and 0 and 1 have equal likelihood.
     *
     * @return the next pseudorandom value
     */
    qint64 nextLongLong();

    /** Generates the next pseudorandom unsigned long long(64 bit) number. All
     * bits of this long are independently chosen and 0 and 1 have equal
     * likelihood.
     *
     * @return the next pseudorandom value
     */
    quint64 nextULongLong();

    /** Sets the seed for this pseudorandom number generator.  As described
     * above, two instances of the same random class, starting with the same
     * seed, should produce the same results, if the same methods are called.
     *
     * @param seed the new seed
     */
    virtual void setSeed(quint64 seed);

protected:
    /**
     * Generates the next pseudorandom number.  This returns an unsigned value
     * whose <code>bits</code> low order bits are independent chosen random bits
     * (0 and 1 are equally likely).
     *
     * @param bits the number of random bits to generate, in the range 1..32
     * @return the next pseudorandom value
     */
    virtual unsigned next(unsigned bits);

private:
    RandomPrivate *const d;
};

#endif
