// $Id: function_definition_node.h,v 1.4 2019/03/22 16:26:23 ist186806 Exp $ -*- c++ -*-
#ifndef __M19_FUNCTIONDEFINITION_H__
#define __M19_FUNCTIONDEFINITION_H__

#include <cdk/ast/basic_node.h>
#include <cdk/ast/sequence_node.h>
#include <cdk/basic_type.h>
#include <string>

namespace m19 {

  /**
   * Class for describing function definition nodes.
   */
  class function_definition_node: public cdk::basic_node {
    int _scope;
    basic_type *_type;
    std::string _id;
    cdk::sequence_node * _arguments;
    cdk::sequence_node * _body;
    cdk::basic_node * _retval;
  
  public:
    function_definition_node(int lineno, int scope, basic_type *type, const std::string &id, cdk::sequence_node *arguments, cdk::sequence_node * body) :
        cdk::basic_node(lineno), _scope(scope), _type(type), _id(id), _arguments(arguments), _body(body) {
    }

    function_definition_node(int lineno, int scope, basic_type *type, const std::string &id, cdk::sequence_node *arguments, cdk::sequence_node * body, cdk::basic_node * retval) :
        cdk::basic_node(lineno), _scope(scope), _type(type), _id(id), _arguments(arguments), _body(body), _retval(retval) {
    }

    function_definition_node(int lineno, int scope, const std::string &id, cdk::sequence_node *arguments, cdk::sequence_node * body) :
        cdk::basic_node(lineno), _scope(scope), _type(new basic_type(0, basic_type::TYPE_VOID)), _id(id), _arguments(arguments), _body(body) {
    }

    function_definition_node(int lineno, int scope, const std::string &id, cdk::sequence_node *arguments, cdk::sequence_node * body, cdk::basic_node * retval) :
        cdk::basic_node(lineno), _scope(scope), _type(new basic_type(0, basic_type::TYPE_VOID)), _id(id), _arguments(arguments), _body(body), _retval(retval) {
    }
  public:
    inline int scope() {
      return _scope;
    }

    inline cdk::basic_node * retval() {
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
