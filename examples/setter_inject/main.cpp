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

#include <iostream>
#include <string>

#include "inject/inject.h"

using namespace std;
using namespace boost;
using namespace inject;

class service {
public:
    virtual const string name() const = 0;
};
context<>::component<service> service_decl;

class impl : public service {
    virtual const string name() const {
        return "component_impl";
    }
};

context<>::component<impl>
impl_decl;

context<>::component<impl>::provides<service>
impl_provides_decl;

context<>::component<service>::implemented_by<impl>
service_impl_decl;

// this classes uses an instance of 'service'
class consumer {
public:
    typedef context<>::ptr<service>::type service_ptr;
private:
    service_ptr _assign_service;
    service_ptr _arg_service;
public:
    consumer() {
    }

    // c++ style property accessors
    const service_ptr& assign_serv() const { return _assign_service; }
    service_ptr& assign_serv() { return _assign_service; }

    // java style getter/setter
    void set_arg_serv(const service_ptr& service) { _arg_service = service; }
    service_ptr&  get_arg_serv() { return _arg_service; }
    const service_ptr&  get_arg_serv() const { return _arg_service; }
};

context<>::component<consumer>
consumer_decl;

context<>::component<consumer>::provides<consumer>
consumer_provides_decl;

context<>::component<consumer>::implemented_by<consumer>
consumer_impl_decl;

// inject to c++ style property
context<>::component<consumer>::assign_setter<service, &consumer::assign_serv>
consumer_assign_setter_decl;

// inject to java style setter
context<>::component<consumer>::arg_setter<service, &consumer::set_arg_serv>
consumer_arg_setter_decl;

int main() {

    // request an instance with injected service
    context<>::injected<consumer> c;

    cout << "assigned service is '" <<
        c->assign_serv()->name() << "'" << endl;
    
    cout << "setter service is '" <<
        c->get_arg_serv()->name() << "'" << endl;

    return 0;
}

