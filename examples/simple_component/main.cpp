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

#include "inject/inject.h"

#include "service.h"

using namespace std;
using namespace inject;

// to separate context configuration from the code using it, we'll implement all
// binding in a separate file. see: 'context_config.cpp'
extern void configure_context();

int main() {

    // we start by binding components - main doesn't know which implementation
    // will be bound to which interface
    configure_context();

    // request an injection of the implementation for 'service' from the default
    // context
    context<>::injected<service> inst;

    // if 'service_impl' was bound successfully to 'service', this should print
    // '42'
    cout <<
        "The implementation bound to the 'service' interface identifies "
        "itself with the number " << inst->id() << endl;

    // note that no where in this file we made any reference to service_impl -
    // at all

    return 0;
}

