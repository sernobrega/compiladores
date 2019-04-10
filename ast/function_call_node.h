// $Id: function_call_node.h,v 1.2 2019/03/21 20:03:53 ist186806 Exp $ -*- c++ -*-
#ifndef __M19_FUNCTIONCALL_H__
#define __M19_FUNCTIONCALL_H__

#include <cdk/ast/sequence_node.h>
#include <cdk/ast/literal_node.h>
#include <string>

namespace m19 {

  /**
   * Class for describing function call nodes.
   */
  class function_call_node: public cdk::expresion_node {
    std::string _id;
    cdk::sequence_node * _arguments;

  public:
    inline function_call_node(int lineno, const std::string &id, cdk::sequence_node *arguments) :
        cdk::expresion_node(lineno), _id(id), _arguments(arguments) {
		}

    inline function_call_node(int lineno,  const std::string &id: cdk::expresion_node(lineno), _id(id), _arguments(new cdk::sequence_node(lineno)),  {
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
