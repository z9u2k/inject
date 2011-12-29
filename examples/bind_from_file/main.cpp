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
#include "simple_format_config.h"

using namespace std;
using namespace inject;

int main() {
    // start by declaring the program's context. note that this is not the
    // global context, but an inner context derived from the global context. as
    // long as this context exists, it is the effective context. since we
    // declare it first thing in main(), it will effectively be the program's
    // context
    context<> ctx;

    // start by loading the first file
    simple_format_config cfg1("bind1.conf");
    ctx.configure(cfg1);

    // look at which was bound
    context<>::injected<service> bind1_1;
    context<>::injected<service> bind1_2;
    cout << "after loading bind1.conf, 'service' is implemeneted by " <<
        bind1_1->name();

    if (bind1_1.get() == bind1_2.get()) {
        cout << " (binding is a singleton)" << endl;
    } else {
        cout << " (binding is not a singleton)" << endl;
    }
    
    // now load second file, overwriting the previous binding
    simple_format_config cfg2("bind2.conf");
    ctx.configure(cfg2);

    // look at which was bound
    context<>::injected<service> bind2_1;
    context<>::injected<service> bind2_2;
    cout << "after (re)loading bind2.conf, 'service' is implemeneted by " <<
        bind2_1->name();
    
    if (bind2_1.get() == bind2_2.get()) {
        cout << " (binding is a singleton)" << endl;
    } else {
        cout << " (binding is not a singleton)" << endl;
    }

    return 0;
}
