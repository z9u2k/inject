/*
 * Copyright (c) 2012 Itay Duvdevani
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef __INJECT_ID_OF_H__
#define __INJECT_ID_OF_H__

#include <cstddef>

namespace inject {

/**
 * we use pointer arithmetics to identify types, to allow the same type to
 * appear in two different dynamic modules without a collision. See comment
 * inside monotonic_counter::next_unique_id for further details
 */
typedef std::ptrdiff_t unique_id;

/** an invalid unique id - doesn't identiify any component */
const unique_id INVALID_ID = -1;

/**
 * provides a way of uniquely identifying types in the system
 * @tparam T type to get id of
 *
 * Example
 * @code
 * unique_id the_id = inject::id_of<some_component>::id();
 * @endcode
 */
template<class T>
class id_of {
public:
    /** @return the unique identifier of <code>T</code> */
    static unique_id id();
};

} // namespace inject

#include "id_of.inl"

#endif // __INJECT_ID_OF_H__

