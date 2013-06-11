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
#ifndef __INJECT_COMPONENT_INL__
#define __INJECT_COMPONENT_INL__

namespace inject {

template<int ID>
template<class T>
context<ID>::component<T>::component() {
    component_descriptor& desc = registry()[id_of<T>::id()];
    desc.id = id_of<T>::id();
}

template<int ID>
template<class T>
context<ID>::component<T>::component(const std::string& name) {
    component_descriptor& desc = registry()[id_of<T>::id()];

    desc.id = id_of<T>::id();
    desc.component_name = name;

    registry().register_name(name, desc.id);
}

template<int ID>
template<class T>
context<ID>::component<T>::~component() {
    registry().unregister(id_of<T>::id());
}

template<int ID>
template<class T>
template<class Interface>
context<ID>::component<T>::provides<Interface>::provides() {
    // register cast provider
    component_descriptor& desc = registry()[id_of<T>::id()];
    
    // initialize default allocator and constructor the first time a component
    // is declared as concrete - we can't do it in component() since the class
    // may be abstract and we can't instantiate and construct abstract classes
    if (desc.allocator == 0) {
        desc.allocator = new allocator_activator< std::allocator<void>, T>();
    }

    if (desc.constructor == 0) {
        desc.constructor = new default_constructor_activator<T>();
    }

    desc.component_cast[id_of<Interface>::id()] = 
        new component_cast<T, Interface>();
}

template<int ID>
template<class T>
template<class Interface>
context<ID>::component<T>::provides<Interface>::~provides() {
    // TODO: remove cast provider
}

template<int ID>
template<class T>
template<class Impl, component_scope Scope>
context<ID>::component<T>::implemented_by<Impl, Scope>::implemented_by() {
    component_descriptor& desc = registry()[id_of<T>::id()];
    desc.default_binding = binding(
        id_of<T>::id(), id_of<Impl>::id(), Scope);
}

template<int ID>
template<class T>
template<class Impl, component_scope Scope>
context<ID>::component<T>::implemented_by<Impl, Scope>::~implemented_by() {
    // remove default binding
    component_descriptor& desc = registry()[id_of<T>::id()];
    desc.default_binding = binding();
}

template<int ID>
template<class T>
template<class Allocator>
context<ID>::component<T>::allocator<Allocator>::allocator() {
    component_descriptor& desc = registry()[id_of<T>::id()];
    _prev_activator = desc.allocator;
    desc.allocator = new allocator_activator<Allocator, T>();
}

template<int ID>
template<class T>
template<class Allocator>
context<ID>::component<T>::allocator<Allocator>::~allocator() {
    component_descriptor& desc = registry()[id_of<T>::id()];
    desc.allocator = _prev_activator;
}

#define CONSTRUCTOR_PARTIAL_SPEC_IMPL(tmpl_decl, spec_args, ctor_args) \
template<int ID> \
template<class T> \
tmpl_decl \
context<ID>::component<T>::constructor<A1, spec_args>::constructor() { \
    component_descriptor& desc = registry()[id_of<T>::id()]; \
    _prev = desc.constructor; \
    desc.constructor = new activator(); \
} \
 \
template<int ID> \
template<class T> \
tmpl_decl \
context<ID>::component<T>::constructor<A1, spec_args>::~constructor() { \
    component_descriptor& desc = registry()[id_of<T>::id()]; \
    desc.constructor = _prev; \
} \
 \
template<int ID> \
template<class T> \
tmpl_decl \
typename context<ID>::unknown_ptr \
context<ID>::component<T>::constructor<A1, spec_args>::activator:: \
activate(unknown_ptr instance) { \
    T* activated = reinterpret_cast<T*>(instance.get()); \
    new(activated) T( \
        ctor_args \
    ); \
    return instance; \
}

// a little trick from http://ingomueller.net/node/1203#comment-599
#define KO ,

CONSTRUCTOR_PARTIAL_SPEC_IMPL(
    // tmpl_decl
    template<
        class A1
    >,
    // spec_args
    void KO void KO void KO void KO void KO void KO void KO void KO void,
    // ctor_args
    context<>::injected<A1>()
)

CONSTRUCTOR_PARTIAL_SPEC_IMPL(
    // tmpl_decl
    template<
        class A1 KO
        class A2
    >,
    // spec_args
    A2 KO void KO void KO void KO void KO void KO void KO void KO void,
    // ctor_args
    context<>::injected<A1>() KO
    context<>::injected<A2>()
)

CONSTRUCTOR_PARTIAL_SPEC_IMPL(
    // tmpl_decl
    template<
        class A1 KO
        class A2 KO
        class A3
    >,
    // spec_args
    A2 KO A3 KO void KO void KO void KO void KO void KO void KO void,
    // ctor_args
    context<>::injected<A1>() KO
    context<>::injected<A2>() KO
    context<>::injected<A3>()
)

CONSTRUCTOR_PARTIAL_SPEC_IMPL(
    // tmpl_decl
    template<
        class A1 KO
        class A2 KO
        class A3 KO
        class A4
    >,
    // spec_args
    A2 KO A3 KO A4 KO void KO void KO void KO void KO void KO void,
    // ctor_args
    context<>::injected<A1>() KO
    context<>::injected<A2>() KO
    context<>::injected<A3>() KO
    context<>::injected<A4>()
)

CONSTRUCTOR_PARTIAL_SPEC_IMPL(
    // tmpl_decl
    template<
        class A1 KO
        class A2 KO
        class A3 KO
        class A4 KO
        class A5
    >,
    // spec_args
    A2 KO A3 KO A4 KO A5 KO void KO void KO void KO void KO void,
    // ctor_args
    context<>::injected<A1>() KO
    context<>::injected<A2>() KO
    context<>::injected<A3>() KO
    context<>::injected<A4>() KO
    context<>::injected<A5>()
)

CONSTRUCTOR_PARTIAL_SPEC_IMPL(
    // tmpl_decl
    template<
        class A1 KO
        class A2 KO
        class A3 KO
        class A4 KO
        class A5 KO
        class A6
    >,
    // spec_args
    A2 KO A3 KO A4 KO A5 KO A6 KO void KO void KO void KO void,
    // ctor_args
    context<>::injected<A1>() KO
    context<>::injected<A2>() KO
    context<>::injected<A3>() KO
    context<>::injected<A4>() KO
    context<>::injected<A5>() KO
    context<>::injected<A6>()
)

CONSTRUCTOR_PARTIAL_SPEC_IMPL(
    // tmpl_decl
    template<
        class A1 KO
        class A2 KO
        class A3 KO
        class A4 KO
        class A5 KO
        class A6 KO
        class A7
    >,
    // spec_args
    A2 KO A3 KO A4 KO A5 KO A6 KO A7 KO void KO void KO void,
    // ctor_args
    context<>::injected<A1>() KO
    context<>::injected<A2>() KO
    context<>::injected<A3>() KO
    context<>::injected<A4>() KO
    context<>::injected<A5>() KO
    context<>::injected<A6>() KO
    context<>::injected<A7>()
)

CONSTRUCTOR_PARTIAL_SPEC_IMPL(
    // tmpl_decl
    template<
        class A1 KO
        class A2 KO
        class A3 KO
        class A4 KO
        class A5 KO
        class A6 KO
        class A7 KO
        class A8
    >,
    // spec_args
    A2 KO A3 KO A4 KO A5 KO A6 KO A7 KO A8 KO void KO void,
    // ctor_args
    context<>::injected<A1>() KO
    context<>::injected<A2>() KO
    context<>::injected<A3>() KO
    context<>::injected<A4>() KO
    context<>::injected<A5>() KO
    context<>::injected<A6>() KO
    context<>::injected<A7>() KO
    context<>::injected<A8>()
)

CONSTRUCTOR_PARTIAL_SPEC_IMPL(
    // tmpl_decl
    template<
        class A1 KO
        class A2 KO
        class A3 KO
        class A4 KO
        class A5 KO
        class A6 KO
        class A7 KO
        class A8 KO
        class A9
    >,
    // spec_args
    A2 KO A3 KO A4 KO A5 KO A6 KO A7 KO A8 KO A9 KO void,
    // ctor_args
    context<>::injected<A1>() KO
    context<>::injected<A2>() KO
    context<>::injected<A3>() KO
    context<>::injected<A4>() KO
    context<>::injected<A5>() KO
    context<>::injected<A6>() KO
    context<>::injected<A7>() KO
    context<>::injected<A8>() KO
    context<>::injected<A9>()
)

CONSTRUCTOR_PARTIAL_SPEC_IMPL(
    // tmpl_decl
    template<
        class A1 KO
        class A2 KO
        class A3 KO
        class A4 KO
        class A5 KO
        class A6 KO
        class A7 KO
        class A8 KO
        class A9 KO
        class A10
    >,
    // spec_args
    A2 KO A3 KO A4 KO A5 KO A6 KO A7 KO A8 KO A9 KO A10,
    // ctor_args
    context<>::injected<A1>() KO
    context<>::injected<A2>() KO
    context<>::injected<A3>() KO
    context<>::injected<A4>() KO
    context<>::injected<A5>() KO
    context<>::injected<A6>() KO
    context<>::injected<A7>() KO
    context<>::injected<A8>() KO
    context<>::injected<A9>() KO
    context<>::injected<A10>()
)

#undef KO
#undef CONSTRUCTOR_PARTIAL_SPEC_IMPL

} // namespace inject

#endif // __INJECT_ACTIVATOR_INL__

