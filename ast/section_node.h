// $Id: section_node.h,v 1.3 2019/04/12 11:53:21 ist186806 Exp $ -*- c++ -*-
#ifndef __M19_SECTIONNODE_H__
#define __M19_SECTIONNODE_H__

#include <cdk/ast/expression_node.h>

namespace m19 {

  /**
   * Class for describing section nodes.
   */
  class section_node: public cdk::basic_node {
    int _qualifier;
    m19::block_node *_block;
    cdk::expression_node *_expr;
  
  public:
    inline section_node(int lineno, int qualifier, cdk::expression_node * expr, m19::block_node *block) :
        cdk::basic_node(lineno), _qualifier(qualifier), _block(block), _expr(expr) {
    }

    inline section_node(int lineno, int qualifier, m19::block_node *block) :
        cdk::basic_node(lineno), _qualifier(qualifier), _block(block) {
    }

  public:
    inline cdk::expression_node * expr() {
      return _expr;
    }

    inline int qualifier() {
      return _qualifier;
    }

    inline m19::block_node *block() {
      return _block;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_section_node(this, level);
    }

  };

} // m19

#endif
