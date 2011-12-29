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
#include "inject/inject.h"

#include "service.h"
#include "service_impl.h"

using namespace inject;

// binding in Inject is done by declaring an 'implemented_by' object. these
// object maintain binding as long as they exist. for that reason, we want to to
// exist throughout the program. since registration is done by the runtime's
// static initializer before main() executes, and initialization order is not
// guaranteed by the runtime, and binding must have all components registered
// prior to binding them, we'll gather all 'implemented_by' declarations to a
// single instance, which we'll keep alive throughout the program.
//
// this is that class
struct my_context_config {
    // mark 'service' as implemented by 'service_impl'
    context<>::component<service>::implemented_by<service_impl>
        service_to_service_impl_bind;

    // ... more bindings ...
};

// the method will simply initialize the 'context_config' declared above as a
// static method filed, making sure it is initialized the first time the method
// is called, and maintained as long as the application is running
void configure_context() {
    static my_context_config config;
}
