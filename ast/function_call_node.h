// $Id: function_call_node.h,v 1.2 2019/03/21 20:03:53 ist186806 Exp $ -*- c++ -*-
#ifndef __M19_FUNCTIONCALL_H__
#define __M19_FUNCTIONCALL_H__

#include <cdk/ast/sequence_node.h>
#include <cdk/basic_type.h>
#include <string>

namespace m19 {

  /**
   * Class for describing function call nodes.
   */
  class function_call_node: public cdk::basic_node {
    std::string _id;
    cdk::sequence_node * _arguments;
    basic_type *_retval;

  public:
    inline function_call_node(int lineno, const std::string &id, cdk::sequence_node *arguments) :
        cdk::basic_node(lineno), _id(id), _arguments(arguments), _retval(new basic_type(0, 
basic_type::TYPE_VOID)) {
    }

    inline function_call_node(int lineno, const std::string &id, cdk::sequence_node 
*arguments, basic_type* retval) : cdk::basic_node(lineno), _id(id), _arguments(arguments), 
_retval(retval) {
}

  public:
    inline std::string id() const {
      return _id;
    }

    inline cdk::sequence_node *arguments() {
      return _arguments;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_call_node(this, level);
    }

  };

} // m19

#endif
