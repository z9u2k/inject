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
#ifndef __INJECT_EXCEPTIONS_H__
#define __INJECT_EXCEPTIONS_H__

#include <exception>
#include <sstream>
#include <string>

namespace inject {

class no_component : public std::exception { 
private:
    unique_id _id;
    std::string _name;
    std::string _msg;
public:
    no_component(unique_id id) throw() : exception(), _id(id) {
        std::stringstream oss;
        oss << "no such component " << id;
        _msg = oss.str();
    }
    no_component(const std::string& name) throw() :
            exception(), _id(INVALID_ID), _name(name) {
        std::stringstream oss;
        oss << "no such component '" << name << "'";
        _msg = oss.str();
    }
    virtual ~no_component() throw() { }
    virtual const char* what() const throw() { return _msg.c_str(); }
    virtual const char* name() const throw() { return _name.c_str(); }
    virtual const unique_id id() const throw() { return _id; }
};

class no_binding : public std::exception { 
private:
    unique_id _component;
    std::string _msg;
public:
    no_binding(unique_id component) throw() :
            exception(), _component(component) {
        std::stringstream oss;
        oss << "component " << component << " has no binding";
        _msg = oss.str();
    }
    virtual ~no_binding() throw() { }
    virtual const char* what() const throw() { return _msg.c_str(); }
    virtual const unique_id component() const throw() {
        return _component;
    }
};

class not_providing : public std::exception { 
private:
    unique_id _bound_id;
    unique_id _interface_id;
    std::string _msg;
public:
    not_providing(
            unique_id bound_component_id,
            unique_id interface_id) throw() :
            exception(),
            _bound_id(bound_component_id),
            _interface_id(interface_id) {
        std::stringstream oss;
        oss <<
            "component " << bound_component_id <<
            " is bound as implementing component " << interface_id <<
            " but isn't registered as providing it";
        _msg = oss.str();
    }
    virtual ~not_providing() throw() { }
    virtual const char* what() const throw() {
        return _msg.c_str();
    }

    virtual const unique_id bound() const throw() {
        return _bound_id;
    }
    
    virtual const unique_id interface() const throw() {
        return _interface_id;
    }
};

class circular_dependency : public std::exception { 
private:
    unique_id _component;
    std::string _msg;
public:
    circular_dependency(unique_id component) throw() :
            exception(), _component(component) {
        std::stringstream oss;
        oss << "circular dependency detected in component " << component;
        _msg = oss.str();
    }
    virtual ~circular_dependency() throw() { }
    virtual const char* what() const throw() { return _msg.c_str(); }
    
    virtual const unique_id component() const throw() {
        return _component;
    }
};

} // namespace inject


#endif // __INJECT_EXCEPTIONS_H__
