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
#ifndef __INJECT_CONTEXT_INL__
#define __INJECT_CONTEXT_INL__

namespace inject {

template<int ID>
template<class From, class To>
typename context<ID>::unknown_ptr context<ID>::component_cast<From, To>::cast(unknown_ptr instance) {

    // TODO: validate that given pointer instance if a From, and throw bad_cast
    // if not

    // for when there's multiple inheritance in the implementing class, a static
    // cast from the implementation to the base is a must - this will also make
    // sure that classes declared as providing a component actually inherit that
    // component (a check done at compile time)
    return
        boost::static_pointer_cast<unknown_component>(
            boost::static_pointer_cast<To>(
                boost::static_pointer_cast<From>(instance)));
}

template<int ID>
typename context<ID>::component_descriptor&
context<ID>::components_registry::operator[](unique_id component_id) {
    return _descriptors[component_id];
}

template<int ID>
typename context<ID>::component_descriptor&
context<ID>::components_registry::operator[](const std::string& name) {
    name_to_id_map::iterator iter = _names.find(name);
    if (iter == _names.end()) {
        throw no_component(name);
    }

    return operator[](iter->second);
}
    
template<int ID>
void context<ID>::components_registry::unregister(unique_id component_id) {
    component_descriptor& desc = _descriptors[component_id];
    if (desc.id != INVALID_ID) {
        _names.erase(desc.component_name);
        _descriptors.erase(component_id);
    }
}
    
template<int ID>
void context<ID>::components_registry::
register_name(const std::string& name, unique_id component_id) {
    _names[name] = component_id;
}

template<int ID>
context<ID>*& context<ID>::current() {
    static context<ID>* _current = 0;
    return _current;
}

template<int ID>
context<ID>& context<ID>::get_current() {
    static context<ID> _global;
    return (*current());
}

template<int ID>    
context<ID>*& context<ID>::head() {
    static context<ID>* _head = 0;
    return _head;
}
    
template<int ID>    
typename context<ID>::components_registry& context<ID>::registry() {
    static components_registry _registry;
    return _registry;
}

template<int ID>    
context<ID>::~context() {
    // pop <this> from stack
    context<ID>::head() = _parent;
    context<ID>::current() = _parent;
}

template<int ID>
context<ID>::context(context_config& config) {
    configure(config);
    init();
}

template<int ID>
void context<ID>::init() {
    // push <this> to stack and make current
    _parent = head();
    context<ID>::head() = this;
    context<ID>::current() = this;
}

template<int ID>
void context<ID>::configure(context_config& config) {
    context_config::context_bindings m = config.bindings();

    for (context_config::context_bindings::iterator i = m.begin() ;
            i != m.end() ; ++i) {
        bind(i->what(), i->to(), i->scope());
    }
}

template<int ID>
template<class Interface>
typename context<ID>::template ptr<Interface>::type context<ID>::instance() {
    return boost::static_pointer_cast<Interface>(
        instance(id_of<Interface>::id()));
}

template<int ID>
binding context<ID>::find_binding(unique_id interface_id) {
    bindings_map::iterator bind_iter = _bindings.find(interface_id);
    if (bind_iter == _bindings.end()) {
        if (_parent != 0) {
            return _parent->find_binding(interface_id);
        } else {
            component_descriptor& desc =
                registry()[interface_id];

            if (desc.default_binding.what() == interface_id) {
                return desc.default_binding;
            } else if (desc.id == INVALID_ID) {
                throw no_component(interface_id);
            } else {
                throw no_binding(interface_id);
            }
        }
    }

    return bind_iter->second;
}

template<int ID>
typename context<ID>::unknown_ptr
context<ID>::instance(unique_id interface_id) {

    const binding& bind = find_binding(interface_id);
    component_descriptor& desc = registry()[bind.to()];

    if (desc.id == INVALID_ID) {
        throw no_binding(interface_id);
    }
    
    if (desc.allocator == 0) {
        throw not_providing(desc.id, interface_id);
    }
    
    unknown_ptr instance;
    typename instances_map::iterator iter;

    // save current "current" context, and set <this> to the current context,
    // so injected fields will use the context that's being used for
    // instantiation and not some other unrelated context
    context<ID>* backup_current = context<ID>::current();
    context<ID>::current() = this;

    // activate instace
    switch (bind.scope()) {
    case scope_none:
        instance = instantiate(desc);
        break;

    case scope_singleton:
        iter = _singletons.find(bind.to());
        if (iter == _singletons.end()) {
            instance = instantiate(desc);
            // TODO: register singletons in global context? may cause having
            // multiple instances in different scopes, or scoping cannot be done
            // per-context. (same component can be a singleton in one context,
            // and non-scoped in another...)
            //
            // maybe use local binding for scope resolution, but provide
            // singleton from global context?
            _singletons[bind.to()] = instance;
        } else {
            instance = iter->second;
        }
        break;

    default:
        break;
    }

    // find cast provider
    typename component_descriptor::component_cast_map::iterator cast_iter = 
        desc.component_cast.find(interface_id);

    if (cast_iter == desc.component_cast.end()) {
        // TODO: a component that doesn provide that interface was bound to it,
        //       can this even compile? binding should try and create a cast
        //       provider, and this should fail compilation...
    }
    
    // restor current
    context<ID>::current() = backup_current;

    return cast_iter->second->cast(instance);
}
    
template<int ID>
typename context<ID>::unknown_ptr
context<ID>::instantiate(component_descriptor& desc) {
    try {
        if (desc.activating) {
            throw circular_dependency(desc.id);
        }

        desc.activating = true;

        unknown_ptr p = desc.allocator->activate(unknown_ptr());
        p = desc.constructor->activate(p);

        for (typename component_descriptor::activators_list::iterator iter =
                desc.activators.begin();
                iter != desc.activators.end();
                ++iter) {
            p = (*iter)->activate(p);
        }

        desc.activating = false;
    
        return p;
    } catch (...) {
        desc.activating = false;
        throw;
    }
}

} // namespace inject

#endif // __INJECT_CONTEXT_INL__
