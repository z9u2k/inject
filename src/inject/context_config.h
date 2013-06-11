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

/**
 * an interface used to configure a context binding. this can be used to load
 * bindings from a file, or any other source, without declaring component
 * implementation inside the code.
 *
 * note that this onle serves to replace
 * {@link context::component::implemented_by} declaration. components must still
 * be registered with the context, with correct allocators, constructors,
 * setters and provision dclarations.
 *
 * this class binds compoenent by name, so any component that should be
 * configured externally, should be decaled with the constructor that provides a
 * name in {@link context::component} declaration
 *
 * Example which loads bindings from a configuration file:
 *
 * <code>bind1.conf</code>
 * @include bind_from_file/bind1.conf
 *
 * <code>bind2.conf</code>
 * @include bind_from_file/bind2.conf
 *
 * <code>main.cpp</code>
 * @include bind_from_file/main.cpp
 *
 * Output:
 * <pre>
 * after loading bind1.conf, 'service' is implemeneted by service_impl1 (binding is not a singleton)
 * after (re)loading bind2.conf, 'service' is implemeneted by service_impl2 (binding is a singleton)
 * </pre>
 */
class context_config {
public:
    /**
     * a binding configuration entry
     */
    class config_bind {
    private:
        std::string _what;
        std::string _to;
        component_scope _scope;
    public:
        /**
         * @param what component to bind
         * @param to component to bind to
         * @param scope bind scope
         */
        config_bind(const std::string& what,
                const std::string& to,
                component_scope scope) :
            _what(what), _to(to), _scope(scope) { }
    public:
        /** @return name of component name to bind */
        const std::string what() const { return _what; }
        /** @return name of component name to bind to */
        const std::string to() const { return _to; }
        /** @return binding scope */
        const component_scope scope() const { return _scope; }
    };
public:
    /** list of bindings */
    typedef std::list<config_bind> context_bindings;
public:
    /**
     * @return list of binding-by-name
     */
    virtual context_bindings bindings() = 0;
};

} // namespace inject

#endif // __INJECT_CONTEXT_CONFIG_H__
