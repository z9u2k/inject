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
#ifndef __INJECT_CONTEXT_H__
#define __INJECT_CONTEXT_H__

#include <assert.h>

#include <string>
#include <memory>
#include <map>

#include <boost/shared_ptr.hpp>

#include "id_of.h"
#include "exceptions.h"
#include "context_config.h"
#include "types.h"
#include "binding.h"

#include "debug.h"

namespace inject {

template<int ID = 0>
class context {
private: // classes
    class generic_component_cast;

    template< class From, class To >
    class component_cast;

    class generic_activator;
    struct component_descriptor;

    class components_registry;

    //template<class Allocator, class Activated>
    //class activator;
    
    template<class Allocator, class Activated>
    class allocator_activator;
    
    template<class Activated>
    class default_constructor_activator;

public: // classes
    template<class T>
    class component;
    
    template<class T>
    class injected;
public: // component pointer type
    template<class T>
    struct ptr {
        typedef boost::shared_ptr<T> type;
    };
private: // types
    typedef typename ptr<unknown_component>::type unknown_ptr;
    typedef std::map<unique_id, binding> bindings_map;
    typedef std::map<unique_id, unknown_ptr> instances_map;
private: // members
    bindings_map _bindings;
    instances_map _singletons;
    context<ID>* _parent;
private:
    unknown_ptr instance(unique_id interface_id);
    binding find_binding(unique_id interface_id);
    void init();
private: // disallow copy-ctor and assogn operator
    context(const context<ID>& other) : _parent(other.parent) { }
    context<ID>& operator=(const context<ID>& other) {
        _parent = other._parent;
        return *this;
    }
public: // constructors
    context() { init(); }
    context(context_config& config);
    virtual ~context();
public: // methods

    void configure(context_config& config);

    template<class Impl>
    void bind() {
        bind<Impl, Impl, scope_none>();
    }
    
    template<class Impl, component_scope Scope>
    void bind() {
        bind<Impl, Impl, Scope>();
    }
    
    template<class Interface, class Impl>
    void bind() {
        bind<Interface, Impl, scope_none>();
    }

    template<class Interface, class Impl, component_scope Scope>
    void bind() {
        _bindings[id_of<Interface>::id()] = binding(
            id_of<Interface>::id(),
            id_of<Impl>::id(),
            Scope);
    }

    void bind(const std::string& name) {
        bind(name, name);
    }

    void bind(const std::string& what, const std::string& to) {
        bind(what, to, scope_none);
    }

    void bind(const std::string& what, const std::string& to,
            component_scope scope) {
        unique_id what_id = registry()[what].id;
        unique_id to_id = registry()[to].id;
        _bindings[what_id] = binding(what_id, to_id, scope);
    }

    template<class Interface>
    typename ptr<Interface>::type instance();

private:
    unknown_ptr instantiate(component_descriptor& desc);

public: // static methods
    static context<ID>& get_current();
private: // context list, components registry
    static context<ID>*& head();
    static context<ID>*& current();
    static components_registry& registry();
};

/**
 * Abstract component cast. Used as a polymorphic base class for all concrete
 * component cast providers.
 *
 * Component pointer need to be cast at runtime from implementing type to
 * interface type. Since implementing type may have multiple inheritance, the
 * cast should be know at compile time.
 *
 * Allows storage of different-type cast providers in the same container
 */
template<int ID>
class context<ID>::generic_component_cast {

    /* --- Members --- */

private:

    /** Component this cast casts from */
    unique_id _from;
    
    /** Component this cast casts to */
    unique_id _to;

    /* --- Constructor/destructor --- */

public:

    /**
     * Constructor.
     * @param from Component this class casts from
     * @param to Component this class casts to
     */
    generic_component_cast(unique_id from, unique_id to) throw() :
        _from(from), _to(to) { }

    /**
     * Destructor, does nothing
     */
    virtual ~generic_component_cast() { }

    /* --- Properties --- */

public:

    /** @return Component this cast provider casts from */
    unique_id from() { return _from; }
    
    /** @return Component this cast provider casts to */
    unique_id to() { return _to; }

    /* --- Abstract methods --- */

public:

    /**
     * Casts the given component pointer to the appropriate type component
     * pointer.
     *
     * @param ptr Pointer to cast. Must be of the correct type, otherwise a
     *        <code>bad_cast</code> exception will be thrown.
     * @throws bad_cast If the given pointer is not a valid pointer, or doesn't
     *         point to the expected type
     */
    virtual unknown_ptr cast(unknown_ptr ptr) = 0;
};

/**
 * A concrete component cast provider. Used to evalute
 * implementation-to-interface cast at compile time, in case the implementing
 * class has multiple inheritance (e.g., the same implementation used for
 * different interfaces)
 */
template<int ID>
template< class From, class To >
class context<ID>::component_cast : public generic_component_cast {

    /* --- Constructor --- */

public:

    /**
     * Initialized base type with known types
     */
    component_cast() throw() : generic_component_cast(
        id_of<From>::id(), id_of<To>::id()) { }

    /* --- generic_component_cast overrides --- */

public:

    /**
     * @see generic_component_cast::cast(const unknown_ptr)
     */
    unknown_ptr cast(unknown_ptr instance);
};

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

/**
 * TODO
 */
template<int ID>
class context<ID>::generic_activator {
private:
    generic_activator* _next;
public:
    generic_activator() : _next(0) { }
    virtual ~generic_activator() { }
public:
    virtual unknown_ptr activate(unknown_ptr instance) = 0;

    const generic_activator* next() const { return _next; }
    generic_activator*& next() { return _next; }
};

/**
 * Describes a registered component
 */
template<int ID>
struct context<ID>::component_descriptor {

    /* --- Types --- */

public:

    /** map target component to appropriate case provider */
    typedef std::map<unique_id, generic_component_cast*> component_cast_map;
    
    /* --- Constructor --- */

public:

    /** initialize component with an invalid id */
    component_descriptor() :
        last_activator(0),
        id(INVALID_ID),
        allocator(0),
        constructor(0),
        first_activator(0),
        activating(false) { }

    /* --- Fields --- */

private:

    generic_activator* last_activator;

public:

    /** component id */
    unique_id id;

    /** component activator */
    generic_activator* allocator;
    generic_activator* constructor;
    generic_activator* first_activator;

    /** component's name */
    std::string component_name;

    /** default binding */
    binding default_binding;

    /**
     * cast providers to the different types this component is decalred as a
     * provider of
     */
    component_cast_map component_cast;

    /** whether descriptor is currently being activated */
    bool activating;

    /* --- Methods --- */

public:

    void append_activator(generic_activator* activator);

};

template<int ID>
void context<ID>::component_descriptor::
append_activator(generic_activator* activator) {
    if (first_activator == 0) {
        first_activator = activator;
    } else {
        last_activator->next() = activator;
    }
        
    last_activator = activator;
}

/**
 * The centralized components registry
 */
template<int ID>
class context<ID>::components_registry {

    /* --- Types --- */

private:

    /** map unique ids and component descriptors */
    typedef std::map<unique_id, component_descriptor> id_to_descriptor_map;

    /** map component names and unique ids */
    typedef std::map<std::string, unique_id> name_to_id_map;

    /* --- Members --- */

private:

    /** ids to descriptors map */
    id_to_descriptor_map _descriptors;

    /** name to ids map */
    name_to_id_map _names;

    /* --- Constructor --- */

public:

    /** Private - it's a singleton (there's only one registry) */
    components_registry() { }
    
    /* --- Public methods and operators --- */

public:

    /**
     * Retrieves the component's descriptor, or registers an empty descriptor
     * if the component's not found, and returns it.
     *
     * @param component_id component id
     * @return component descriptor
     */
    component_descriptor& operator[](unique_id component_id);

    /**
     * Retrieves the component's descriptor by the component's name.
     *
     * @param name component name
     * @return component descriptor
     * @throws no_component If o component by that name was found
     */
    component_descriptor& operator[](const std::string& name);

    /**
     * Register a component name
     *
     * @param name component name
     * @param component_id component id
     */
    void register_name(const std::string& name, unique_id component_id);

    /**
     * Unregister a component
     *
     * @param component_id component to unregister
     */
    void unregister(unique_id component_id);
};

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

        generic_activator* current = desc.first_activator;
        while (current != 0) {
            p = current->activate(p);
            current = current->next();
        }

        desc.activating = false;
    
        return p;
    } catch (...) {
        desc.activating = false;
        throw;
    }
}
    
} // namespace inject

#endif // __INJECT_CONTEXT_H__
