// $Id: variable_declaration_node.h,v 1.2 2019/03/20 18:28:21 ist186806 Exp $ -*- c++ -*-
#ifndef __M19_VARIABLEDECLARATIONNODE_H__
#define __M19_VARIABLEDECLARATIONNODE_H__

#include <cdk/ast/expression_node.h>
#include <cdk/basic_type.h>
#include <string>

namespace m19 {

  /**
   * Class for describing variable declaration nodes.
   */
  class variable_declaration_node: public cdk::basic_node {
    int _scope;
    basic_type *_type;
    std::string _id;
    cdk::expression_node *_expr;

  public:
    inline variable_declaration_node(int lineno, int scope, basic_type *type, const std::string &id, cdk::expression_node *expr) :
        cdk::basic_node(lineno), _scope(scope), _type(type), _id(id), _expr(expr) {
    }

    inline variable_declaration_node(int lineno, int scope, const std::string &id, cdk::expression_node *expr) :
        cdk::basic_node(lineno), _scope(scope), _type(new basic_type(0, basic_type::TYPE_VOID)), _id(id), _expr(expr) {
    }

  public:
    inline int scope() {
      return _scope;
    }

    inline std::string id() const {
      return _id;
    }

    inline basic_type * type() {
      return _type;
    }

    inline cdk::expression_node *expr() {
      return _expr;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_variable_declaration_node(this, level);
    }

  };

} // m19

#endif
