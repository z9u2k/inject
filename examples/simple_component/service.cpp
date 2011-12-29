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

using namespace inject;

// here we declare 'service' as a component. everything in Inject is a
// component. since Inject supports nested and multiple injections contexts, we
// have to explicitly specify which context we'd like to work with. we'll use
// the default context for this example.
static context<>::component<service>
    service_component_decl;

// creating the component decleration object is enough to register it with
// Inject. we'll never use this object for anything. note that component
// declerations should always be done in compilation units, not header files,
// since we want them to be initialized before main() starts.
//
// declaring the component at the compilation-unit level ensures that our
// runtime will execute its constructor before main() begins, so when our
// program starts, all the components are already registered.

