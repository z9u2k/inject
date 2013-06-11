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
#ifndef __INJECT_ACTIVATOR_H__
#define __INJECT_ACTIVATOR_H__

#include "context.h"

namespace inject {

/**
 * allocates an instance using the given allocator.this should probably be the
 * first activator called in the construction chain.
 *
 * @tparam ID context ID
 * @tparam Allocator allocator to use - should behave like
 *         <code>std::allocator</code>
 * @tparam Activated type to allocate
 */
template<int ID>
template<class Allocator, class Activated>
class context<ID>::allocator_activator : public generic_activator {
public:
    allocator_activator() : generic_activator() { }
    virtual ~allocator_activator() { }
public:
    /**
     * allocates a new instance of Activated using Allocator, and returns a
     * pointer to it.
     *
     * @param instance ignored
     * @return pointer to allocated, uninitialized, instance
     */
    context<ID>::unknown_ptr activate(context<ID>::unknown_ptr instance);
};

/**
 * initializes an allocated instance using its default constructor.
 * @tparam ID context ID
 * @tparam Activated type to activate
 */
template<int ID>
template<class Activated>
class context<ID>::default_constructor_activator : public generic_activator {
public:
    default_constructor_activator() : generic_activator() { }
    virtual ~default_constructor_activator() { }
public:
    /**
     * @param instance an allocated, uninitialized pointer to an
     *        <code>Activated</code> instance
     * @return given instance, after initialization using default constructor
     */
    context<ID>::unknown_ptr activate(context<ID>::unknown_ptr instance);
};

/**
 * deletes an instance using the supplied allocator by calling the instance's
 * destructor and deallocating the instance's memory
 *
 * this serves as a <code>Deleter</code> for <code>shared_ptr</code>, which is
 * used internally by the context
 *
 * @tparam Allocator allocator to delete with. should behave like
 *         <code>std::allocator</code>
 * @tparam T type to delete
 * @todo isn't this an implementation detail that should be hidden from the
 * user?
 */
template<class Allocator, class T>
class allocator_deleter {
private:
    Allocator _allocator;
public:
    /** @param allocator allocator to use when deallocating */
    allocator_deleter(const Allocator& allocator) throw() 
        : _allocator(allocator) {}

    /** @param other instance to copy */
    allocator_deleter(const allocator_deleter& other) throw() 
        : _allocator(other._allocator) {}

    /**
     * destroys and deallocates the given instance using the class'
     * <code>Allocator</code>
     * @param p instance to destroy and deallocate. the pointer will be reset to
     *        <code>null</code>
     */
    void operator()(T*& p) {
        p->~T();
        _allocator.deallocate(p, 1);
        p = 0;
    }
};

} // namespace inject

#include "activator.inl"

#endif // __INJECT_ACTIVATOR_H__

