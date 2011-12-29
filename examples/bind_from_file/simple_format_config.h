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
#ifndef __INJECT_EXAMPLE_BIND_FROM_FILE_SIMPLE_FORMAT_CONFIG_H__
#define __INJECT_EXAMPLE_BIND_FROM_FILE_SIMPLE_FORMAT_CONFIG_H__

#include <string>

#include "inject/inject.h"

class simple_format_config : public inject::context_config {
private:
    std::string _what;
    std::string _to;
    inject::component_scope _scope;
public:
    simple_format_config(const std::string& file);
    virtual ~simple_format_config();
    virtual context_bindings bindings();
};

#endif // __INJECT_EXAMPLE_BIND_FROM_FILE_SIMPLE_FORMAT_CONFIG_H__

