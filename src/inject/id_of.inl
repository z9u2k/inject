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
#ifndef __INJECT_ID_OF_INL__
#define __INJECT_ID_OF_INL__

namespace inject {

/**
 * a monotonic counter
 * @tparam T ignored, used as a workaround to avoid a cpp file
 * @note do not use this class - it is an internal implementation detail
 * @note this class is not thread safe
 */
template<typename T = void>
class monotonic_counter {
public:
    /** @return a monotonically increasing integer */
    static unique_id next_unique_id() {
        // TODO: if we could find a way to make sure each monotonic_counter
        // generates unique values, regardless of the number of
        // monotonic_counters in the system, we could use inject across module
        // boundaries

        // the counter begins at the address of unique_id_counter to partially
        // address the issue above. this way there's smaller chance of
        // identifier overlap across module boundaries (though this still
        // doesn't guarantee uniqueness) since each module should have a
        // different memory location for this counter
        static unique_id unique_id_counter =
            reinterpret_cast<unique_id>(&unique_id_counter);

        return unique_id_counter++;
    }
};

template<class T>
unique_id id_of<T>::id() {
    static unique_id id = monotonic_counter<>::next_unique_id();
    return id;
}

} // namespace inject

#endif // __INJECT_ID_OF_INL__

