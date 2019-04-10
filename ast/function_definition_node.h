// $Id: function_definition_node.h,v 1.4 2019/03/22 16:26:23 ist186806 Exp $ -*- c++ -*-
#ifndef __M19_FUNCTIONDEFINITION_H__
#define __M19_FUNCTIONDEFINITION_H__

#include <cdk/ast/sequence_node.h>
#include <cdk/ast/literal_node.h>
#include <string>

namespace m19 {

  /**
   * Class for describing function definition nodes.
   */
  class function_definition_node: public cdk::basic_node {
    int _scope;
    cdk::literal_node * _retval;
    std::string _id;
    cdk::sequence_node * _arguments;
    cdk::sequence_node * _body;
  
  public:
    inline function_definition_node(int lineno, int scope, const std::string &id, 
cdk::sequence_node *arguments, cdk::sequence_node * body) :
        cdk::basic_node(lineno), _retval(new cdk::literal_node(lineno)) _scope(scope), _id(id), _arguments(arguments), _body(body) {
    }

    inline function_definition_node(int lineno, cdk::literal_node retval, int scope, basic_type *type, const 
std::string &id, cdk::sequence_node *arguments, cdk::sequence_node * body) :
        cdk::basic_node(lineno), _retval(new cdk::literal_node(lineno, retval)), _scope(scope), _id(id), _arguments(arguments), _body(body) {
    }
  public:
    inline int scope() {
      return _scope;
    }

    inline cdk::literal_node * retval() {
      return _retval;
    }

    inline std::string id() const {
      return _id;
    }

    inline cdk::sequence_node *arguments() {
      return _arguments;
    }

    inline cdk::sequence_node * body() {
      return _body;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_definition_node(this, level);
    }

  };

} // m19

#endif
