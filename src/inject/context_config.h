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
#ifndef __INJECT_CONTEXT_CONFIG_H__
#define __INJECT_CONTEXT_CONFIG_H__

#include <istream>
#include <fstream>
#include <string>
#include <list>

#include "types.h"

namespace inject {

class context_config {
public:
    class config_bind {
    private:
        std::string _what;
        std::string _to;
        component_scope _scope;
    public:
        config_bind(const std::string& what,
                const std::string& to,
                component_scope scope) :
            _what(what), _to(to), _scope(scope) { }
    public:
        const std::string what() const { return _what; }
        const std::string to() const { return _to; }
        const component_scope scope() const { return _scope; }
    };
public:
    typedef std::list<config_bind> context_bindings;
public:
    virtual context_bindings bindings() = 0;
};

} // namespace inject

#endif // __INJECT_CONTEXT_CONFIG_H__
