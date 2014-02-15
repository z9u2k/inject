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
#include <list>

#include <boost/shared_ptr.hpp>

#include "id_of.h"
#include "exceptions.h"
#include "context_config.h"
#include "types.h"
#include "binding.h"

#include "debug.h"

namespace inject {

/**
 * an injection context. the context manages instance creation, initialization
 * and injection processes. the context knows about all available components,
 * they provisions and how they are implemented.
 *
 * @tparam ID used to support multiple injection contexts in the same
 *         application
 *
 * @todo explain about the current context stack
 */
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
    /**
     * abstracts the actual pointer used, in case we want to change it sometime
     * in the future
     * @todo C++11 std::shared_ptr if available ?
     */
    template<class T>
    struct ptr {
        /** the pointer type */
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
private: // disallow copy-ctor and assign operator
    context(const context<ID>& other) : _parent(other._parent) { }
    context<ID>& operator=(const context<ID>& other) {
        _parent = other._parent;
        return *this;
    }
public: // constructors
    /** constructs an empty context, with no components */
    context() { init(); }
    /** @param config a source of initial component bindings */
    context(context_config& config);
    virtual ~context();
public: // methods

    /**
     * configures the context with the given bindings
     * @param config component binding source
     */
    void configure(context_config& config);

    /**
     * binds a type as its own implementation
     * @tparam Impl implementation type
     */
    template<class Impl>
    void bind() {
        bind<Impl, Impl, scope_none>();
    }
    
    /**
     * binds a type as its own implementation in the given scope
     * @tparam Impl implementation type
     * @tparam Scope binding scope
     */
    template<class Impl, component_scope Scope>
    void bind() {
        bind<Impl, Impl, Scope>();
    }
    
    /**
     * binds a type as the implementation of an interface
     * @tparam Interface implemented interface
     * @tparam Impl implementation type
     */
    template<class Interface, class Impl>
    void bind() {
        bind<Interface, Impl, scope_none>();
    }

    /**
     * binds a type as the implementation of an interface in the given scope
     * @tparam Interface implemented interface
     * @tparam Impl implementation type
     * @tparam Scope binding scope
     */
    template<class Interface, class Impl, component_scope Scope>
    void bind() {
        _bindings[id_of<Interface>::id()] = binding(
            id_of<Interface>::id(),
            id_of<Impl>::id(),
            Scope);
    }

    /**
     * binds a type as its own implementation
     * @param name component to bind
     */
    void bind(const std::string& name) {
        bind(name, name);
    }

    /**
     * binds a type as the implementation of an interface
     * @param what name of implementation
     * @param to name of implemented interface
     */
    void bind(const std::string& what, const std::string& to) {
        bind(what, to, scope_none);
    }

    /**
     * binds a type as the implementation of an interface in the given scope
     * @param what name of implementation
     * @param to name of implemented interface
     * @param scope binding scope
     */
    void bind(const std::string& what, const std::string& to,
            component_scope scope) {
        unique_id what_id = registry()[what].id;
        unique_id to_id = registry()[to].id;
        _bindings[what_id] = binding(what_id, to_id, scope);
    }

    /**
     * obtains a pointer to an instance that is registered as the implementation
     * of the given interface
     * @return pointer to implementing instance
     * @tparam Interface type to obtain pointer to
     * @throws no_component
     * @throws no_binding
     * @throws not_providing
     * @throws circular_dependency
     */
    template<class Interface>
    typename ptr<Interface>::type instance();

private:
    unknown_ptr instantiate(component_descriptor& desc);

public: // static methods
    /** @return reference to current context */
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
     * @return cast pointer
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

    unknown_ptr cast(unknown_ptr instance);
};

/**
 * A (non-template) base class for all activators. an activator is part of a
 * chain, that prepares an instance.
 *
 * activating a type means executing, in order, the chain of its activators
 * (which may allocate, construct or inject dependencies)
 */
template<int ID>
class context<ID>::generic_activator {
private:
    generic_activator* _next;
public:
    generic_activator() : _next(0) { }
    virtual ~generic_activator() { }
public:
    /**
     * activate the given instance, performing some part of the initialization
     * process.
     *
     * @param instance instance to activate
     * @return instance to pass to next activator
     */
    virtual unknown_ptr activate(unknown_ptr instance) = 0;

    /** @return next activator in activation chain */
    const generic_activator* next() const { return _next; }
    
    /** @return next activator in activation chain */
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
    
    /** list of activators */
    typedef std::list<generic_activator*> activators_list;

    /* --- Constructor --- */

public:

    /** initialize component with an invalid id */
    component_descriptor() :
        id(INVALID_ID),
        allocator(0),
        constructor(0),
        activating(false) { }

    /* --- Fields --- */

public:

    /** component id */
    unique_id id;

    /** mandatory activator which allocates an uninitialized instance */
    generic_activator* allocator;

    /**
     * mandatory activator which initializes an allocated instance using one of
     * its constructors
     */
    generic_activator* constructor;

    /**
     * optional activators list
     */
    activators_list activators;

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
};

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

} // namespace inject

#include "context.inl"

#endif // __INJECT_CONTEXT_H__
