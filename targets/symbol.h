#ifndef __M19_SEMANTICS_SYMBOL_H__
#define __M19_SEMANTICS_SYMBOL_H__

#include <string>
#include <cdk/basic_type.h>

namespace m19 {

  class symbol {

    std::string _name;
    int _value;

    bool _constant;
    int _scope;
    basic_type *_type;
    bool _initialized;
    int _offset = 0; // 0 (zero) means global variable/function
    bool _function; // false for variables
    bool _fundecl = false;
    std::vector<basic_type *> _args;

  public:
    symbol(bool constant, int scope, basic_type *type, const std::string &name, bool initialized, bool function, bool fundecl =
               false) :
        _name(name), _value(0), _constant(constant), _scope(scope), _type(type), _initialized(initialized), _function(
            function), _fundecl(fundecl) {
    }

    ~symbol() {
    }

    const std::string &name() const {
      return _name;
    }
    int value() const {
      return _value;
    }
    int value(int v) {
      return _value = v;
    }
    bool constant() const {
      return _constant;
    }
    int scope() const {
      return _scope;
    }
    basic_type *type() {
      return _type;
    }
    const std::string &identifier() const {
      return name();
    }
    bool initialized() const {
      return _initialized;
    }
    int offset() const {
      return _offset;
    }
    void set_offset(int offset) {
      _offset = offset;
    }

    void set_args(std::vector<basic_type *> args) {
      _args = args;
    }

    std::vector<basic_type *> args() {
      return _args;
    }

    bool isFunction() const {
      return _function;
    }

    bool global() const {
      return _offset == 0;
    }
    bool isVariable() const {
      return !_function;
    }

    bool fundecl() const {
      return _fundecl;
    }

  };

} // m19

#endif
