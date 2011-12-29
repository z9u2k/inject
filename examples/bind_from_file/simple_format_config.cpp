#include "simple_format_config.h"

#include <fstream>
#include <stdexcept>
#include <list>

using namespace std;
using namespace inject;

simple_format_config::simple_format_config(const string& file) {
    ifstream in(file.c_str(), ios::in);

    string in_scope;

    string dev_null;
    
    // read 'component'
    in >> dev_null;
    // read '='
    in >> dev_null;
    // read component name
    in >> _what;
    
    // read 'provided_by'
    in >> dev_null;
    // read '='
    in >> dev_null;
    // read provider
    in >> _to;
    
    // read 'in_scope'
    in >> dev_null;
    // read '='
    in >> dev_null;
    // read scope
    in >> in_scope;

    in.close();

    // parse in_scope
    if (in_scope == "none") {
        _scope = scope_none;
    } else if (in_scope == "singleton") {
        _scope = scope_singleton;
    } else {
        throw runtime_error(in_scope);
    }
}

simple_format_config::~simple_format_config() {
}

context_config::context_bindings simple_format_config::bindings() {
    context_bindings res;
    res.push_back(config_bind(_what, _to, _scope));
    return res;
}
