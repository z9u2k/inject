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

#include "singleton.h"

using namespace inject;

// we declare the singleton_impl in a compilation unit to make sure no one can
// instantiate it without Inject
class singleton_impl : public singleton {
private:
    int attr_val;
public:
    singleton_impl() : attr_val(0) { }
    int attr() const;
    int& attr();
};

int singleton_impl::attr() const {
    return attr_val;
}

int& singleton_impl::attr() {
    return attr_val;
}

// declare components, and bind a default implementation in the singleton scope
static context<>::
component<singleton>
singleton_component_decl;

static context<>::
component<singleton_impl>
singleton_impl_component_decl;

static context<>::
component<singleton_impl>::
provides<singleton>
singleton_impl_provides_decl;

// note the second argument to implemented_by which states the implementation
// scope
static context<>::
component<singleton>::
implemented_by<singleton_impl, scope_singleton>
singleton_is_singleton_impl_bind_decl;

