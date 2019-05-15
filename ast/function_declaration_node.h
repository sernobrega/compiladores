
// $Id: function_declaration_node.h,v 1.4 2019/04/12 11:53:21 ist186806 Exp $ -*- c++ -*-
#ifndef __M19_FUNCTIONDECLARATION_H__
#define __M19_FUNCTIONDECLARATION_H__

#include <cdk/ast/sequence_node.h>
#include <cdk/basic_type.h>
#include <string>

namespace m19 {

  /**
   * Class for describing function declaration nodes.
   */
  class function_declaration_node: public cdk::basic_node {
    int _qualifier;
    basic_type *_type;
    std::string _id;
    cdk::sequence_node *_arguments;
    m19::section_init_node * _init;
    cdk::sequence_node * _section;
    m19::section_end_node * _end;
    
  public:
    function_declaration_node(int lineno, int qualifier, basic_type *type, const std::string &id, m19::section_init_node * init, m19::sequence_node * section, m19::section_end_node * end) :
        cdk::basic_node(lineno), _qualifier(qualifier), _type(type), _id(id), _init(init), _section(section), _end(end) {
    }

    function_declaration_node(int lineno, int qualifier, const std::string &id, m19::section_init_node * init, m19::sequence_node * section, m19::section_end_node * end) :
        cdk::basic_node(lineno), _qualifier(qualifier), _type(new basic_type(0, basic_type::TYPE_VOID)), _id(id), _init(init), _section(section), _end(end) {
    }

  public:
    int qualifier() {
      return _qualifier;
    }

    basic_type * type() {
      return _type;
    }

    inline std::string id() const {
      return _id;
    }

    m19::section_init_node *init() {
      return _init;
    }

    m19::section_end_node *end() {
      return _end;
    }

    m19::sequence_node *section() {
      return _section;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_declaration_node(this, level);
    }

  };

} // m19

#endif
