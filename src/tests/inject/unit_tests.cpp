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
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK

#include <iostream>

#include <boost/test/unit_test.hpp>

#include "inject/inject.h"

using namespace boost;
using namespace inject;
using namespace std;

BOOST_AUTO_TEST_SUITE(Inject)
    
class service {
public:
    virtual unique_id id() = 0;
};

class impl1 : public service {
public:
    unique_id id() {
        return id_of<impl1>::id();
    }
};

class impl2 : public service {
public:
    unique_id id() {
        return id_of<impl2>::id();
    }
};

BOOST_AUTO_TEST_CASE(simple_inject)
{
    context<>::component<service> x;
    context<>::component<impl1> xx;
    context<>::component<impl1>::provides<service> xxx;

    context<> c;
    c.bind<service, impl1>();

    context<>::injected<service> actual;
    BOOST_CHECK_EQUAL(actual->id(), id_of<impl1>::id());
}

BOOST_AUTO_TEST_CASE(default_bind_scope_is_none)
{
    context<>::component<service> x;
    context<>::component<impl1> xx;
    context<>::component<impl1>::provides<service> xxx;

    context<> c;
    c.bind<service, impl1>();

    context<>::injected<service> first;
    context<>::injected<service> second;
    BOOST_CHECK(first.get() != second.get());
}

BOOST_AUTO_TEST_CASE(bind_scope_none)
{
    context<>::component<service> x;
    context<>::component<impl1> xx;
    context<>::component<impl1>::provides<service> xxx;

    context<> c;
    c.bind<service, impl1, scope_none>();

    context<>::injected<service> first;
    context<>::injected<service> second;
    BOOST_CHECK(first.get() != second.get());
}

BOOST_AUTO_TEST_CASE(bind_scope_singleton)
{
    context<>::component<service> x;
    context<>::component<impl1> xx;
    context<>::component<impl1>::provides<service> xxx;

    context<> c;
    c.bind<service, impl1, scope_singleton>();

    context<>::injected<service> first;
    context<>::injected<service> second;
    BOOST_CHECK(first.get() == second.get());
}

BOOST_AUTO_TEST_CASE(implemented_by_scope_singleton)
{
    context<>::component<service> x;
    context<>::component<impl1> xx;
    context<>::component<impl1>::provides<service> xxx;
    context<>::component<service>::implemented_by<impl1, scope_singleton> xxxx;

    context<> c;

    context<>::injected<service> first;
    context<>::injected<service> second;
    BOOST_CHECK(first.get() == second.get());
}

BOOST_AUTO_TEST_CASE(inject_without_bind)
{
    context<>::component<service> x;

    context<> c;
    try {
        context<>::injected<service> x;
        BOOST_ERROR("no_binding not thrown");
    } catch (const no_binding& e) {
        BOOST_CHECK_EQUAL(e.component(), id_of<service>::id());
    }
}

BOOST_AUTO_TEST_CASE(inject_without_component)
{
    context<> c;
    try {
        context<>::injected<service> x;
        BOOST_ERROR("no_component not thrown");
    } catch (const no_component& e) {
        BOOST_CHECK_EQUAL(e.id(), id_of<service>::id());
    }
}

BOOST_AUTO_TEST_CASE(inject_bound_without_providing)
{
    context<>::component<service> x;
    context<>::component<impl1> xx;
    // impl1 does not provide service, but bound as implementation
    context<>::component<service>::implemented_by<impl1> xxx;
    
    context<> c;
    c.bind<service, impl1>();

    try {
        context<>::injected<service> y;
        BOOST_ERROR("not_providing not thrown");
    } catch (const not_providing& e) {
        BOOST_CHECK_EQUAL(e.bound(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(e.interface(), id_of<service>::id());
    }
}

BOOST_AUTO_TEST_CASE(context_nesting_override_binding)
{
    context<>::component<service> x;
    context<>::component<impl1> xx;
    context<>::component<impl1>::provides<service> xxx;
    context<>::component<impl2> y;
    context<>::component<impl2>::provides<service> yy;

    context<> c;
    c.bind<service, impl1>();
    context<>::injected<service> p;
    BOOST_CHECK_EQUAL(p->id(), id_of<impl1>::id());

    {
        context<> c;
        c.bind<service, impl2>();
        context<>::injected<service> p;
        BOOST_CHECK_EQUAL(p->id(), id_of<impl2>::id());
    }
    
    context<>::injected<service> p2;
    BOOST_CHECK_EQUAL(p2->id(), id_of<impl1>::id());
}

BOOST_AUTO_TEST_CASE(context_nesting_inherit_binding)
{
    context<>::component<service> x;
    context<>::component<impl1> xx;
    context<>::component<impl1>::provides<service> xxx;

    context<> c;
    c.bind<service, impl1>();
    context<>::injected<service> p;
    BOOST_CHECK_EQUAL(p->id(), id_of<impl1>::id());

    {
        context<> c;
        context<>::injected<service> p;
        BOOST_CHECK_EQUAL(p->id(), id_of<impl1>::id());
    }
    
    context<>::injected<service> p2;
    BOOST_CHECK_EQUAL(p2->id(), id_of<impl1>::id());
}

BOOST_AUTO_TEST_CASE(rebind)
{
    context<>::component<service> x;
    context<>::component<impl1> xx;
    context<>::component<impl1>::provides<service> xxx;
    context<>::component<impl2> y;
    context<>::component<impl2>::provides<service> yy;

    context<> c;
    c.bind<service, impl1>();
    context<>::injected<service> p;
    BOOST_CHECK_EQUAL(p->id(), id_of<impl1>::id());
    
    c.bind<service, impl2>();
    context<>::injected<service> p2;
    BOOST_CHECK_EQUAL(p2->id(), id_of<impl2>::id());
}

BOOST_AUTO_TEST_CASE(bind_by_name)
{
    context<>::component<service> x("s");
    context<>::component<impl1> xx("impl1");
    context<>::component<impl1>::provides<service> xxx;

    context<> c;
    c.bind("s", "impl1");

    context<>::injected<service> actual;
    BOOST_CHECK_EQUAL(actual->id(), id_of<impl1>::id());
}

BOOST_AUTO_TEST_CASE(bind_by_name_no_component)
{
    context<>::component<service> x("s");
    context<>::component<impl1> xx("impl1");
    context<>::component<impl1>::provides<service> xxx;

    context<> c;

    const string invalid_name = "this_is_not_a_component_name";

    try {
        c.bind(invalid_name, "impl1");
        BOOST_ERROR("no_component not thrown");
    } catch (const no_component& e) {
        BOOST_CHECK(e.name() == invalid_name);
        BOOST_CHECK_EQUAL(e.id(), INVALID_ID);
    }
}

BOOST_AUTO_TEST_CASE(configure_context)
{
    context<>::component<service> x("s");
    context<>::component<impl1> xx("impl1");
    context<>::component<impl1>::provides<service> xxx;

    class simple_config : public context_config {
    public:
        context_bindings bindings() {
            context_config::context_bindings result;
            result.push_back(config_bind("s", "impl1", scope_none));
            return result;
        }
    };

    simple_config s;
    context<> c(s);

    context<>::injected<service> actual;
    BOOST_CHECK_EQUAL(actual->id(), id_of<impl1>::id());
}

BOOST_AUTO_TEST_CASE(default_impl)
{
    context<>::component<service> x;
    context<>::component<service>::implemented_by<impl1> xx;

    context<>::component<impl1> y;
    context<>::component<impl1>::provides<service> yy;

    context<> c;

    context<>::injected<service> first;
    BOOST_CHECK_EQUAL(first->id(), id_of<impl1>::id());

    // check that default scope is scope_none (not singleton)
    context<>::injected<service> second;
    BOOST_CHECK(first.get() != second.get());
}

BOOST_AUTO_TEST_CASE(default_impl_override)
{
    context<>::component<service> x;
    context<>::component<service>::implemented_by<impl1> xx;

    context<>::component<impl1> y;
    context<>::component<impl1>::provides<service> yy;
    
    context<>::component<impl2> z;
    context<>::component<impl2>::provides<service> zz;

    context<> c;
    c.bind<service, impl2>();

    context<>::injected<service> actual;
    BOOST_CHECK_EQUAL(actual->id(), id_of<impl2>::id());
}

template<class T>
struct TestAlloc {

    static int allocate_times;
    static int deallocate_times;

    template<class U>
    struct rebind {
        typedef TestAlloc<U> other;
    };

    TestAlloc() throw() {}
    TestAlloc(const TestAlloc<T>&) throw() {}
    template<class U> TestAlloc(const TestAlloc<U>&) throw() {}

    T* allocate(size_t n, const T* hint=0) {
        allocate_times++;
        return (T*)operator new(sizeof(T));
    }

    void deallocate(T* p, size_t n) {
        deallocate_times++;
        operator delete(p);
    }

    void construct(T* p, const T& val) {
        new(p) T(val);
    }

    void destroy(T* p) {
        p->~T();
    }
};

template<class T> int TestAlloc<T>::allocate_times = 0;
template<class T> int TestAlloc<T>::deallocate_times = 0;

BOOST_AUTO_TEST_CASE(custom_allocator_simple_inject)
{
    context<>::component<service> x;
    context<>::component<impl1> xx;
    context<>::component<impl1>::allocator< TestAlloc<impl1> > xxx;
    context<>::component<impl1>::provides<service> xxxx;

    context<> c;

    // don't bind as singleton - we're about to release the instance
    c.bind<service, impl1>();

    // start a new scope so injected<> will be destroyed, releasing the injected
    // instance so we could test for destory() and deallocate()
    {
        context<>::injected<service> actual;
        BOOST_CHECK_EQUAL(actual->id(), id_of<impl1>::id());
    }

    // assert allocator was used to allocate and construct
    BOOST_CHECK_EQUAL(1, TestAlloc<impl1>::allocate_times);
    
    // assert allocator is used to destory and deallocate
    BOOST_CHECK_EQUAL(1, TestAlloc<impl1>::deallocate_times);
}

BOOST_AUTO_TEST_CASE(bind_to_self)
{
    context<>::component<impl1> b1;
    context<>::component<impl1>::provides<impl1> b2;

    context<> c;
    
    c.bind<impl1>();
    context<>::injected<impl1> p;
    BOOST_CHECK_EQUAL(p->id(), id_of<impl1>::id());
}

class with_injected_member {
public:
    context<>::injected<service> my_service;
};

BOOST_AUTO_TEST_CASE(injected_class_member)
{
    context<>::component<service> b1;
    context<>::component<impl1> b2;
    context<>::component<impl1>::provides<service> b3;
    context<>::component<with_injected_member> b4;
    context<>::component<with_injected_member>::provides<with_injected_member> b5;

    context<> c;
    
    c.bind<service, impl1>();
    c.bind<with_injected_member>();

    context<>::injected<with_injected_member> p;
    BOOST_CHECK_EQUAL(p->my_service->id(), id_of<impl1>::id());
}

// TODO: instantiating context is current during activation

class cdep2_ok;

class cdep1 {
private:
    context<>::injected<cdep2_ok> dep2;
};

class cdep2_ok {
};

class cdep2 : public cdep2_ok {
private:
    context<>::injected<cdep1> dep2;
};

BOOST_AUTO_TEST_CASE(test_circular_dependency)
{
    context<>::component<cdep1> d1;
    context<>::component<cdep1>::provides<cdep1> d2;
    context<>::component<cdep2_ok> d3;
    context<>::component<cdep2> d4;
    context<>::component<cdep2>::provides<cdep2_ok> d5;

    context<> c;
    c.bind<cdep1>();
    c.bind<cdep2_ok, cdep2>();
    c.bind<cdep2>();

    try {
        context<>::injected<cdep1> a;
        BOOST_ERROR("circular_dependency not thrown");
    } catch (const circular_dependency& e) {
        BOOST_CHECK_EQUAL(e.component(), id_of<cdep1>::id());
    }
    
    try {
        context<>::injected<cdep2> a;
        BOOST_ERROR("circular_dependency not thrown");
    } catch (const circular_dependency& e) {
        BOOST_CHECK_EQUAL(e.component(), id_of<cdep2>::id());
    }
}

BOOST_AUTO_TEST_CASE(test_can_activate_after_circular_dependency)
{
    context<>::component<cdep1> d1;
    context<>::component<cdep1>::provides<cdep1> d2;
    context<>::component<cdep2_ok> d3;
    context<>::component<cdep2_ok>::provides<cdep2_ok> d4;
    context<>::component<cdep2> d5;
    context<>::component<cdep2>::provides<cdep2_ok> d6;

    // fail on circular dependency
    context<> c;
    c.bind<cdep1>();
    c.bind<cdep2_ok, cdep2>();

    try {
        context<>::injected<cdep1> a;
        BOOST_ERROR("circular_dependency not thrown");
    } catch (const circular_dependency& e) {
        BOOST_CHECK_EQUAL(e.component(), id_of<cdep1>::id());
    }

    // make sure cdep1 can be instantiated after circular dep. has been resolved
    c.bind<cdep2_ok>();
    context<>::injected<cdep1> a;
}

template<typename T>
class template_service {
public:
    virtual T id() = 0;
};

template<typename T>
class template_impl : public template_service<T> {
public:
    T id() { return (T)42; }
};

BOOST_AUTO_TEST_CASE(test_template_inject)
{
    context<>::component< template_service<int> > x;
    context<>::component< template_impl<int> > xx;
    context<>::component< template_impl<int> >::provides< template_service<int> > xxx;
    
    context<> c;
    c.bind< template_service<int>, template_impl<int> >();

    context<>::injected< template_service<int> > actual_i;
    BOOST_CHECK_EQUAL(actual_i->id(), 42);
}

template<>
class template_impl<std::string> : public template_service<std::string> {
public:
    std::string id() { return "24"; }
};

BOOST_AUTO_TEST_CASE(test_specialized_template_inject)
{
    context<>::component< template_service<std::string> > y;
    context<>::component< template_impl<std::string> > yy;
    context<>::component< template_impl<std::string> >::provides< template_service<std::string> > yyy;

    context<> c;

    c.bind< template_service<std::string>, template_impl<std::string> >();

    context<>::injected< template_service<std::string> > actual_s;
    BOOST_CHECK_EQUAL(actual_s->id(), "24");
}

class service_a {
public:
    virtual int a() const = 0;
};

class service_b {
public:
    virtual int b() const = 0;
};

class ab_impl : public service_a, public service_b {
public:
    virtual int a() const { return 10; }
    virtual int b() const { return 20; }
};

BOOST_AUTO_TEST_CASE(test_component_multiple_inheritance)
{
    context<>::component<service_a> y;
    context<>::component<service_b> yy;

    context<>::component<ab_impl> yyy;
    context<>::component<ab_impl>::provides<service_a> yyyy;
    context<>::component<ab_impl>::provides<service_b> yyyyy;

    context<> c;

    c.bind<service_a, ab_impl>();
    c.bind<service_b, ab_impl>();

    context<>::injected<service_a> actual_a;
    context<>::injected<service_b> actual_b;

    BOOST_CHECK_EQUAL(actual_a->a(), 10);
    BOOST_CHECK_EQUAL(actual_b->b(), 20);
}

class ctor_inject {
private:
    int ctor_num;
public:
    ctor_inject() {
        BOOST_ERROR("default constructor should not be called!");
    }

    ctor_inject(
            context<>::ptr<service>::type service1) {
        ctor_num = 1;
        BOOST_CHECK_EQUAL(service1->id(), id_of<impl1>::id());
    }
    
    ctor_inject(
            context<>::ptr<service>::type service1,
            context<>::ptr<service>::type service2) {
        ctor_num = 2;
        BOOST_CHECK_EQUAL(service1->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service2->id(), id_of<impl1>::id());
    }
    
    ctor_inject(
            context<>::ptr<service>::type service1,
            context<>::ptr<service>::type service2,
            context<>::ptr<service>::type service3) {
        ctor_num = 3;
        BOOST_CHECK_EQUAL(service1->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service2->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service3->id(), id_of<impl1>::id());
    }
    
    ctor_inject(
            context<>::ptr<service>::type service1,
            context<>::ptr<service>::type service2,
            context<>::ptr<service>::type service3,
            context<>::ptr<service>::type service4) {
        ctor_num = 4;
        BOOST_CHECK_EQUAL(service1->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service2->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service3->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service4->id(), id_of<impl1>::id());
    }
    
    ctor_inject(
            context<>::ptr<service>::type service1,
            context<>::ptr<service>::type service2,
            context<>::ptr<service>::type service3,
            context<>::ptr<service>::type service4,
            context<>::ptr<service>::type service5) {
        ctor_num = 5;
        BOOST_CHECK_EQUAL(service1->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service2->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service3->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service4->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service5->id(), id_of<impl1>::id());
    }
    
    ctor_inject(
            context<>::ptr<service>::type service1,
            context<>::ptr<service>::type service2,
            context<>::ptr<service>::type service3,
            context<>::ptr<service>::type service4,
            context<>::ptr<service>::type service5,
            context<>::ptr<service>::type service6) {
        ctor_num = 6;
        BOOST_CHECK_EQUAL(service1->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service2->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service3->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service4->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service5->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service6->id(), id_of<impl1>::id());
    }
    
    ctor_inject(
            context<>::ptr<service>::type service1,
            context<>::ptr<service>::type service2,
            context<>::ptr<service>::type service3,
            context<>::ptr<service>::type service4,
            context<>::ptr<service>::type service5,
            context<>::ptr<service>::type service6,
            context<>::ptr<service>::type service7) {
        ctor_num = 7;
        BOOST_CHECK_EQUAL(service1->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service2->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service3->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service4->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service5->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service6->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service7->id(), id_of<impl1>::id());
    }
    
    ctor_inject(
            context<>::ptr<service>::type service1,
            context<>::ptr<service>::type service2,
            context<>::ptr<service>::type service3,
            context<>::ptr<service>::type service4,
            context<>::ptr<service>::type service5,
            context<>::ptr<service>::type service6,
            context<>::ptr<service>::type service7,
            context<>::ptr<service>::type service8) {
        ctor_num = 8;
        BOOST_CHECK_EQUAL(service1->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service2->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service3->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service4->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service5->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service6->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service7->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service8->id(), id_of<impl1>::id());
    }
    
    ctor_inject(
            context<>::ptr<service>::type service1,
            context<>::ptr<service>::type service2,
            context<>::ptr<service>::type service3,
            context<>::ptr<service>::type service4,
            context<>::ptr<service>::type service5,
            context<>::ptr<service>::type service6,
            context<>::ptr<service>::type service7,
            context<>::ptr<service>::type service8,
            context<>::ptr<service>::type service9) {
        ctor_num = 9;
        BOOST_CHECK_EQUAL(service1->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service2->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service3->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service4->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service5->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service6->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service7->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service8->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service9->id(), id_of<impl1>::id());
    }
    
    ctor_inject(
            context<>::ptr<service>::type service1,
            context<>::ptr<service>::type service2,
            context<>::ptr<service>::type service3,
            context<>::ptr<service>::type service4,
            context<>::ptr<service>::type service5,
            context<>::ptr<service>::type service6,
            context<>::ptr<service>::type service7,
            context<>::ptr<service>::type service8,
            context<>::ptr<service>::type service9,
            context<>::ptr<service>::type service10) {
        ctor_num = 10;
        BOOST_CHECK_EQUAL(service1->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service2->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service3->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service4->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service5->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service6->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service7->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service8->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service9->id(), id_of<impl1>::id());
        BOOST_CHECK_EQUAL(service10->id(), id_of<impl1>::id());
    }

    int which_ctor() const { return ctor_num; }
};

#define CTOR_TEST_CASE(num, ctor_spec) \
BOOST_AUTO_TEST_CASE(test_ctor_inject_##num) \
{ \
    context<>::component<service> x1; \
\
    context<>::component<impl1> x2; \
    context<>::component<impl1>::provides<service> x3; \
\
    context<>::component<ctor_inject> x4; \
    context<>::component<ctor_inject>::provides<ctor_inject> x5; \
    context<>::component<ctor_inject>::constructor<ctor_spec> x6; \
\
    context<> c; \
    c.bind<service, impl1>(); \
    c.bind<ctor_inject>(); \
\
    context<>::injected<ctor_inject> p; \
    BOOST_CHECK_EQUAL(p->which_ctor(), num); \
}

// a little trick from http://ingomueller.net/node/1203#comment-599
#define KO ,

CTOR_TEST_CASE(1,
    service)

CTOR_TEST_CASE(2,
    service KO
    service)

CTOR_TEST_CASE(3,
    service KO
    service KO
    service)

CTOR_TEST_CASE(4,
    service KO
    service KO
    service KO
    service)

CTOR_TEST_CASE(5,
    service KO
    service KO
    service KO
    service KO
    service)

CTOR_TEST_CASE(6,
    service KO
    service KO
    service KO
    service KO
    service KO
    service)

CTOR_TEST_CASE(7,
    service KO
    service KO
    service KO
    service KO
    service KO
    service KO
    service)

CTOR_TEST_CASE(8,
    service KO
    service KO
    service KO
    service KO
    service KO
    service KO
    service KO
    service)

CTOR_TEST_CASE(9,
    service KO
    service KO
    service KO
    service KO
    service KO
    service KO
    service KO
    service KO
    service)

CTOR_TEST_CASE(10,
    service KO
    service KO
    service KO
    service KO
    service KO
    service KO
    service KO
    service KO
    service KO
    service)

#undef KO
#undef CTOR_TEST_CASE

class with_setter {
    typedef context<>::ptr<service>::type service_ptr;
private:
    service_ptr _service1;
    service_ptr _service2;
public:
    with_setter() {
    }

    const service_ptr& ptr1() const { return _service1; }
    service_ptr& ptr1() { return _service1; }
    
    const service_ptr& ptr2() const { return _service2; }
    service_ptr& ptr2() { return _service2; }

    void set_ptr1(const service_ptr& ptr) { _service1 = ptr; }
    void set_ptr2(const service_ptr& ptr) { _service2 = ptr; }
};

BOOST_AUTO_TEST_CASE(test_inject_one_assign_setter) {
    context<>::component<service> x1;

    context<>::component<impl1> x2;
    context<>::component<impl1>::provides<service> x3;
    
    context<>::component<with_setter> x4;
    context<>::component<with_setter>::provides<with_setter> x5;
    context<>::component<with_setter>::assign_setter<service, &with_setter::ptr1> x6;

    context<> c;

    c.bind<service, impl1>();
    c.bind<with_setter>();

    context<>::injected<with_setter> p;

    BOOST_CHECK_EQUAL(p->ptr1()->id(), id_of<impl1>::id());
}

BOOST_AUTO_TEST_CASE(test_inject_multi_assign_setter)
{
    context<>::component<service> x1;

    context<>::component<impl1> x2;
    context<>::component<impl1>::provides<service> x3;
    
    context<>::component<with_setter> x4;
    context<>::component<with_setter>::provides<with_setter> x5;
    context<>::component<with_setter>::assign_setter<service, &with_setter::ptr1> x6;
    context<>::component<with_setter>::assign_setter<service, &with_setter::ptr2> x7;

    context<> c;

    c.bind<service, impl1>();
    c.bind<with_setter>();

    context<>::injected<with_setter> p;

    BOOST_CHECK_EQUAL(p->ptr1()->id(), id_of<impl1>::id());
    BOOST_CHECK_EQUAL(p->ptr2()->id(), id_of<impl1>::id());
}

BOOST_AUTO_TEST_CASE(test_inject_one_arg_setter)
{
    context<>::component<service> x1;

    context<>::component<impl1> x2;
    context<>::component<impl1>::provides<service> x3;
    
    context<>::component<with_setter> x4;
    context<>::component<with_setter>::provides<with_setter> x5;
    context<>::component<with_setter>::arg_setter<service, &with_setter::set_ptr1> x6;

    context<> c;

    c.bind<service, impl1>();
    c.bind<with_setter>();

    context<>::injected<with_setter> p;

    BOOST_CHECK_EQUAL(p->ptr1()->id(), id_of<impl1>::id());
}

BOOST_AUTO_TEST_CASE(test_inject_multi_arg_setter)
{
    context<>::component<service> x1;

    context<>::component<impl1> x2;
    context<>::component<impl1>::provides<service> x3;
    
    context<>::component<with_setter> x4;
    context<>::component<with_setter>::provides<with_setter> x5;
    context<>::component<with_setter>::arg_setter<service, &with_setter::set_ptr1> x6;
    context<>::component<with_setter>::arg_setter<service, &with_setter::set_ptr2> x7;

    context<> c;

    c.bind<service, impl1>();
    c.bind<with_setter>();

    context<>::injected<with_setter> p;

    BOOST_CHECK_EQUAL(p->ptr1()->id(), id_of<impl1>::id());
    BOOST_CHECK_EQUAL(p->ptr2()->id(), id_of<impl1>::id());
}

class variable {
public:
  int value;
};

class snapshot {
public:
  context<>::injected<variable> target;
  int value;

  snapshot() {
    value = target->value;
  }
};

BOOST_AUTO_TEST_CASE(test_lazy_inject)
{
  context<> c;

  context<>::component<variable> v0;
  context<>::component<variable>::provides<variable> v1;
  c.bind<variable, scope_singleton>();
  
  context<>::component<snapshot> s0;
  context<>::component<snapshot>::provides<snapshot> s1;
  c.bind<snapshot>();

  context<>::injected<variable> var;

  var->value = 0xcc;

  context<>::injected<snapshot> immediate_snapshot;
  context<>::injected<snapshot> lazy_snapshot(lazy);
  
  var->value = 0xdd;
    
  BOOST_CHECK_EQUAL(immediate_snapshot->value, 0xcc);
  BOOST_CHECK_EQUAL(lazy_snapshot->value, 0xdd);
}

BOOST_AUTO_TEST_SUITE_END()
