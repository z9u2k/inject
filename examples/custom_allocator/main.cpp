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

#include <iostream>
#include <string>

#include "inject/inject.h"

using namespace std;
using namespace boost;
using namespace inject;

template<class T>
struct MyAllocator {

    template<class U>
    struct rebind {
        typedef MyAllocator<U> other;
    };

    MyAllocator() throw() {}
    MyAllocator(const MyAllocator<T>&) throw() { }
    template<class U> MyAllocator(const MyAllocator<U>&) throw() { }

    T* allocate(size_t n, const T* hint=0) {
        T* p = (T*)operator new(sizeof(T));
        cout << "allocating " << n << " instance(s)" << endl;
        return p;
    }

    void deallocate(T* p, size_t n) {
        cout << "deallocating " << n << " instance(s)" << endl;
        operator delete(p);
    }

    void construct(T* p, const T& val) {
        cout << "constructing instance" << endl;
        new(p) T(val);
    }

    void destroy(T* p) {
        cout << "destorying instance" << endl;
        p->~T();
    }
};

class service {
public:
    virtual const string name() const = 0;
};
context<>::component<service> service_decl;

class impl : public service {
private:
    string _name;
public:
    impl() : _name("impl") {
        cout << "in: impl::impl()" << endl;
    }
    
    impl(const impl& other) : _name(other._name) {
        cout << "in: impl::impl(const impl&)" << endl;
    }

    virtual ~impl() {
        cout << "in: impl::~impl()" << endl;
    }

    virtual const string name() const {
        return "impl";
    }
};

context<>::component<impl>
impl_decl;

context<>::component<impl>::provides<service>
impl_provides_decl;

// this is where we declare an allocator for the component
context<>::component<impl>::allocator< MyAllocator<impl> >
impl_allocator_decl;

context<>::component<service>::implemented_by<impl>
service_impl_decl;

int main() {

    cout << "requesting service..." << endl;

    context<>::injected<service> p;

    cout << "service name is: " << p->name() << endl;

    return 0;
}

