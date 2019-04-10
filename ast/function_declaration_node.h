
// $Id: function_declaration_node.h,v 1.2 2019/03/20 18:28:19 ist186806 Exp $ -*- c++ -*-
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
  
  public:
    function_declaration_node(int lineno, int qualifier, basic_type *type, const std::string &id, cdk::sequence_node *arguments) :
        cdk::basic_node(lineno), _qualifier(qualifier), _type(type), _id(id), _arguments(arguments) {
    }

    function_declaration_node(int lineno, int qualifier, const std::string &id, cdk::sequence_node *arguments) :
        cdk::basic_node(lineno), _qualifier(qualifier), _type(new basic_type(0, basic_type::TYPE_VOID)), _id(id), _arguments(arguments) {
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

    cdk::sequence_node *arguments() {
      return _arguments;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_declaration_node(this, level);
    }

  };

} // m19

#endif
