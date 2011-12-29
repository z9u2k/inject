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
#ifndef __INJECT_INJECTED_H__
#define __INJECT_INJECTED_H__

#include "context.h"
#include "types.h"

namespace inject {

template<int ID>
template<class T>
class context<ID>::injected {
private:
    typedef typename ptr<T>::type ptr_type;
private:
    ptr_type _ptr;
public:
    injected() : _ptr(context<ID>::get_current().instance<T>()) { }
    injected(const injected<T>& other) : _ptr(other._ptr) { }
    injected(const ptr_type& ptr) : _ptr(ptr) { }

    injected<T>& operator=(const injected<T>& other) {
        _ptr = other._ptr;
        return (*this);
    }

    bool operator==(const injected<T>& other) {
        return _ptr == other._ptr;
    }

    bool operator!=(const injected<T>& other) {
        return !((*this) == other);
    }

    T& operator*() const throw() { return (*_ptr); }
    T* operator->() const throw() { return _ptr.get(); }

    T* get() { return _ptr.get(); }

    operator ptr_type() {
        return _ptr;
    }
    
    operator const ptr_type() const {
        return _ptr;
    }
};

} // namespace inject

#endif // __INJECT_INJECTED_H__
