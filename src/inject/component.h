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
#ifndef __INJECT_COMPONENT_H__
#define __INJECT_COMPONENT_H__

#include <memory>
#include <string>
#include <map>
#include <list>

#include "debug.h"

#include "id_of.h"
#include "types.h"
#include "binding.h"
#include "context.h"

namespace inject {

/**
 * a component describes a type in a context, which can be provided by other
 * components, implemented by other components, injector to other components
 * etc.
 *
 * a component is what the context manages. creating a component registers this
 * type with the context, making it available for binding and instantiation.
 *
 * @tparam ID context ID to register to
 * @tparam T component type. must have a default constructor.
 */
template<int ID>
template<class T>
class context<ID>::component {

    /* --- constructors --- */

public:

    /**
     * registers an unnamed component with the context
     */
    component();

    /**
     * registers a named component with the context
     * @param name component's name
     */
    component(const std::string& name);

    /** unregisters the component from the context */
    virtual ~component();

    /* --- classes --- */

public:

    /**
     * a scoped object, that indicated the component provides a certain
     * interface
     *
     * internaly, this is required for generating a "cast-provider", which is
     * able to cast the implementing type to the interface type dynamically - an
     * implementing type may implement many interfaces, so the context must be
     * able to cast that instance correctly to each interface. registering a
     * compoenent as providing an interface allows the context to prepare the
     * casting code necessary
     *
     * @tparam Interface interface type provided by this component
     */
    template<class Interface>
    class provides {
    public:
        /**
         * registers with the context that the component provides an interface
         */
        provides();

        /**
         * deletes provision registration from the context
         */
        virtual ~provides();
    };

    /**
     * indicates an interface component can be implemented by the current
     * component
     *
     * there can only be one implementing component for a certain component.
     * conflicts are resolved so that the latest registration wins
     *
     * when component A is implemented using component B in some context,
     * instantiations of A in that context will result in an instance of B
     *
     * @tparam Impl implementing type
     * @tparam Scope implementation scope (default: none)
     */
    template<class Impl, component_scope Scope = scope_none>
    class implemented_by {
    public:
        /**
         * registers with the context that the current component is implemented
         * by the given component
         */
        implemented_by();

        /**
         * removed implementation registration from the context
         * @todo restoring previous registration?
         */
        virtual ~implemented_by();
    };

    /**
     * indicates that new instances of the current component should be allocated
     * using the given allocator
     * @tparam Allocator allocator to use for allocating new components of this
     *         type - should behave like <code>std::allocator</code>
     *
     * Example:
     * @include custom_allocator/main.cpp
     * 
     * Output:
     * <pre>
     * requesting service...
     * allocating 1 instance(s)
     * in: impl::impl()
     * service name is: impl
     * in: impl::~impl()
     * deallocating 1 instance(s)
     * </pre>
     */
    template<class Allocator>
    class allocator {
    private:
        generic_activator* _prev_activator;
    public:
        allocator();
        virtual ~allocator();
    };

    /**
     * indicates the current component should be initialized with a constructor
     * different than the default constructor. up to 10 constructor arguments
     * can be specified in this way
     *
     * @tparam A1 first constructor argument
     * @tparam A2 second constructor argument (optional)
     * @tparam A3 third constructor argument (optional)
     * @tparam A4 fourth constructor argument (optional)
     * @tparam A5 fifth constructor argument (optional)
     * @tparam A6 sixth constructor argument (optional)
     * @tparam A7 seventh constructor argument (optional)
     * @tparam A8 eighth constructor argument (optional)
     * @tparam A9 ninth constructor argument (optional)
     * @tparam A10 tenth constructor argument (optional)
     * @todo C++11 vardiac templates could probably be used here ...
     *
     * Example:
     * @include ctor_inject/main.cpp
     *
     * Output:
     * <pre>
     * with injected struct service name is 'component_impl'
     * with test struct service name is 'test_impl'
     * </pre>
     */
    template<
        class A1,
        class A2=void,
        class A3=void,
        class A4=void,
        class A5=void,
        class A6=void,
        class A7=void,
        class A8=void,
        class A9=void,
        class A10=void>
    class constructor {
    private:
        generic_activator* _prev;
    private:
        /** activates the component using the correct number of arguments */
        class activator : public generic_activator {
        public:
            unknown_ptr activate(unknown_ptr instance);
        };
    public:
        constructor();
        virtual ~constructor();
    };

    // constructor partial specialization decleration macro
#define CONSTRUCTOR_PART_SPEC_DECL(a2, a3, a4, a5, a6, a7, a8, a9) \
    class constructor<A1, a2, a3, a4, a5, a6, a7, a8, a9, void> { \
    private: \
        generic_activator* _prev; \
    private: \
        class activator : public generic_activator { \
        public: \
            unknown_ptr activate(unknown_ptr instance); \
        }; \
    public: \
        constructor(); \
        virtual ~constructor(); \
    };

    template<
        class A1>
    CONSTRUCTOR_PART_SPEC_DECL(void, void, void, void, void, void, void, void)
    
    template<
        class A1,
        class A2>
    CONSTRUCTOR_PART_SPEC_DECL(A2, void, void, void, void, void, void, void)
    
    template<
        class A1,
        class A2,
        class A3>
    CONSTRUCTOR_PART_SPEC_DECL(A2, A3, void, void, void, void, void, void)
    
    template<
        class A1,
        class A2,
        class A3,
        class A4>
    CONSTRUCTOR_PART_SPEC_DECL(A2, A3, A4, void, void, void, void, void)
    
    template<
        class A1,
        class A2,
        class A3,
        class A4,
        class A5>
    CONSTRUCTOR_PART_SPEC_DECL(A2, A3, A4, A5, void, void, void, void)
    
    template<
        class A1,
        class A2,
        class A3,
        class A4,
        class A5,
        class A6>
    CONSTRUCTOR_PART_SPEC_DECL(A2, A3, A4, A5, A6, void, void, void)
    
    template<
        class A1,
        class A2,
        class A3,
        class A4,
        class A5,
        class A6,
        class A7>
    CONSTRUCTOR_PART_SPEC_DECL(A2, A3, A4, A5, A6, A7, void, void)
    
    template<
        class A1,
        class A2,
        class A3,
        class A4,
        class A5,
        class A6,
        class A7,
        class A8>
    CONSTRUCTOR_PART_SPEC_DECL(A2, A3, A4, A5, A6, A7, A8, void)
    
    template<
        class A1,
        class A2,
        class A3,
        class A4,
        class A5,
        class A6,
        class A7,
        class A8,
        class A9>
    CONSTRUCTOR_PART_SPEC_DECL(A2, A3, A4, A5, A6, A7, A8, A9)

#undef CONSTRUCTOR_SPEC_DECL

    /**
     * injects an interface implementation into a C++ style setter. this
     * activator will assign the context implementation of
     * <code>Interface</code> into the given <code>Setter</code> method.
     *
     * @tparam Interface interface instance assign into setter
     * @tparam Setter setter to call
     *
     * Example:
     * @include setter_inject/main.cpp
     *
     * Output:
     * <pre>
     * assigned service is 'component_impl'
     * setter service is 'component_impl'
     * </pre>
     */
    template<class Interface, typename ptr<Interface>::type& (T::*Setter)()>
    class assign_setter {
    private:
        class activator : public generic_activator {
        public:
            unknown_ptr activate(unknown_ptr instance) {
                T* activated = reinterpret_cast<T*>(instance.get());
                (activated->*Setter)() = injected<Interface>();
                return instance;
            }
        };
    public:
        assign_setter() {
            component_descriptor& desc = registry()[id_of<T>::id()];
            desc.activators.push_back(new activator());
        }
    };

    /**
     * injects an interface implementation into a Java-style roperty setter.
     * this activator will pass the context implementation of
     * <code>Interface</code> as the single argument of the given
     * <code>Setter</code> method.
     *
     * @tparam Interface interface instance to pass as setter parameter
     * @tparam Setter setter to call
     *
     * Example:
     * @include setter_inject/main.cpp
     *
     * Output:
     * <pre>
     * assigned service is 'component_impl'
     * setter service is 'component_impl'
     * </pre>
     */
    template<class Interface,
        void (T::*Setter)(const typename ptr<Interface>::type&)>
    class arg_setter {
    private:
        class activator : public generic_activator {
        public:
            unknown_ptr activate(unknown_ptr instance) {
                T* activated = reinterpret_cast<T*>(instance.get());
                (activated->*Setter)(injected<Interface>());
                return instance;
            }
        };
    public:
        arg_setter() {
            component_descriptor& desc = registry()[id_of<T>::id()];
            desc.activators.push_back(new activator());
        }
    };
};

} // namespace inject

#include "component.inl"

#endif // __INJECT_COMPONENT_H__
