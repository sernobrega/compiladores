// $Id: function_definition_node.h,v 1.8 2019/04/12 11:53:21 ist186806 Exp $ -*- c++ -*-
#ifndef __M19_FUNCTIONDEFINITION_H__
#define __M19_FUNCTIONDEFINITION_H__

#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/basic_node.h>
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
    m19::section_init_node * _init;
    cdk::sequence_node * _section;
    m19::section_end_node * _end;
    cdk::expression_node * _retval;
  
  public:
    function_definition_node(int lineno, int scope, basic_type *type, const std::string &id, cdk::sequence_node *arguments, m19::section_init_node * init, cdk::sequence_node * section, m19::section_end_node * end) :
        cdk::basic_node(lineno), _scope(scope), _type(type), _id(id), _arguments(arguments), _init(init), _section(section), _end(end) {
    }

    function_definition_node(int lineno, int scope, basic_type *type, const std::string &id, cdk::sequence_node *arguments, cdk::expression_node * retval, m19::section_init_node * init, cdk::sequence_node * section, m19::section_end_node * end) :
        cdk::basic_node(lineno), _scope(scope), _type(type), _id(id), _arguments(arguments), _init(init), _section(section), _end(end), _retval(retval) {
    }

    function_definition_node(int lineno, int scope, const std::string &id, cdk::sequence_node *arguments, m19::section_init_node * init, cdk::sequence_node * section, m19::section_end_node * end) :
        cdk::basic_node(lineno), _scope(scope), _type(new basic_type(0, basic_type::TYPE_VOID)), _id(id), _arguments(arguments), _init(init), _section(section), _end(end) {
    }
    
  public:
    inline int scope() {
      return _scope;
    }

    basic_type *type() {
      return _type;
    }

    inline cdk::expression_node * retval() {
      return _retval;
    }

    inline std::string id() const {
      return _id;
    }
    inline cdk::sequence_node *arguments() {
      return _arguments;
    }

    m19::section_init_node *init() {
      return _init;
    }

    m19::section_end_node *end() {
      return _end;
    }

    cdk::sequence_node *section() {
      return _section;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_definition_node(this, level);
    }

  };

} // m19

#endif
