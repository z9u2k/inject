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
#ifndef __INJECT_EXAMPLE_SERVICE_IMPL_H__
#define __INJECT_EXAMPLE_SERVICE_IMPL_H__

#include "service.h"

// this will be that class we'll bind to 'service' interface to be used at
// runtime.

class service_impl : public service {
public:

    // this is implicit in this example, but Inject implementation componenets
    // must fulfill two requirements:
    // 1. Their default-constructor must be visible (they will get instantiated
    //    through it)
    // 2. They must have a static cast operator to the interface type their
    //    implementing
    //
    //
    // both of these requirements are met by inheriting the interface and
    // leaving constructor definition to the compiler

    // just implement the method
    int id() const;
};

#endif // __INJECT_EXAMPLE_SERVICE_IMPL_H__

