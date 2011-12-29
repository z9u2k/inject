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
#ifndef __INJECT_BINDING_H__
#define __INJECT_BINDING_H__

#include "types.h"
#include "id_of.h"

namespace inject {

class binding {
private:
    unique_id _what; 
    unique_id _to; 
    component_scope _scope;
public:
    binding() :
        _what(INVALID_ID),
        _to(INVALID_ID),
        _scope(scope_none) { }

    binding(unique_id what, unique_id to, component_scope scope) :
        _what(what), _to(to), _scope(scope) { }

    virtual ~binding() {}

public:

    unique_id what() const { return _what; }
    unique_id to() const { return _to; }
    component_scope scope() const { return _scope; }
};

} // namespace inject

#endif // __INJECT_BINDING_H__
