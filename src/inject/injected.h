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

class lazy_type {};
static lazy_type lazy;

/**
 * obtains the component implementing <code>T</code> from the current context.
 * simply constructing this pointer-wrapper is enough to obtain the pointer.
 *
 * if you require obtaining a pointer from a specific context instance, use
 * {@link context::instance()}.
 * 
 * @tparam ID context ID to obtain implementations from
 * @tparam T component type to obtain implementations for
 *
 * Example:
 * @code
 * injected<some_service> s;
 * s->some_service_method();
 * @endcode
 */
template<int ID>
template<class T>
class context<ID>::injected {
private:
    typedef typename ptr<T>::type ptr_type;

private:
    ptr_type _ptr;

public:

    /**
     * @throws no_component
     * @throws no_binding
     * @throws not_providing
     * @throws circular_dependency
     */
    injected() : _ptr(context<ID>::get_current().template instance<T>()) { }

    /**
     * obtain component lazily - i.e., only the first time it is needed, and not
     * at construction time.
     */
    injected(const lazy_type&) {}

    /** @param other copy from */
    injected(const injected<T>& other) : _ptr(other._ptr) { }

    /**
     * @param ptr pointer to wrap
     */
    injected(const ptr_type& ptr) : _ptr(ptr) { }

    /**
     * @param other instance to assign
     * @return reference to this
     */
    injected<T>& operator=(const injected<T>& other) {
        _ptr = other._ptr;
        return (*this);
    }

    /**
     * @param other pointer wrapper to compare to
     * @return whether wrapped pointer equals
     */
    bool operator==(const injected<T>& other) {
        return ptr() == other._ptr;
    }

    /**
     * @param other pointer wrapper to compare to
     * @return whether wrapped pointer differ
     */
    bool operator!=(const injected<T>& other) {
        return !((*this) == other);
    }

    /** @return instance pointer by wrapper pointer */
    T& operator*() const throw() { return (*ptr()); }

    /** @return wrapper pointer */
    T* operator->() const throw() { return ptr().get(); }

    /** @return wrapper pointer */
    T* get() { return ptr().get(); }

    /** @return wrapper pointer */
    const T* get() const { return ptr().get(); }

    /** @return implicit cast to wrapper pointer type */
    operator ptr_type() {
        return ptr();
    }
    
    /** @return implicit cast to wrapper pointer type */
    operator const ptr_type() const {
        return ptr();
    }

private:
    ptr_type& ptr() {
      if (_ptr.get() == 0) {
        _ptr = context<ID>::get_current().template instance<T>();
      }
      return _ptr;
    }

    const ptr_type& ptr() const {
      if (_ptr.get() == 0) {
        *const_cast<ptr_type*>(&_ptr) =
          context<ID>::get_current().template instance<T>();
      }
      return _ptr;
    }
};

} // namespace inject

#endif // __INJECT_INJECTED_H__
