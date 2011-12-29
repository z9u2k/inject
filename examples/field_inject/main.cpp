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
    context<>::injected<service> _service;
public:
    consumer() {
    }

    const service_ptr serv() const { return _service; }
};

context<>::component<consumer>
consumer_decl;

context<>::component<consumer>::provides<consumer>
consumer_provides_decl;

context<>::component<consumer>::implemented_by<consumer>
consumer_impl_decl;

int main() {

    // request an instance with injected service
    context<>::injected<consumer> c;

    cout << "private field service is '" <<
        c->serv()->name() << "'" << endl;
    
    return 0;
}

