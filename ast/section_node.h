// $Id: section_node.h,v 1.1 2019/03/20 18:28:20 ist186806 Exp $ -*- c++ -*-
#ifndef __M19_SECTIONNODE_H__
#define __M19_SECTIONNODE_H__

namespace m19 {

  /**
   * Class for describing section nodes.
   */
  class section_node: public cdk::basic_node {
    int _qualifier;
    cdk::expression_node *_expr;
    m19::block_node *_block;
  
  public:
    inline section_node(int lineno, int qualifier, cdk::expression_node * expr, m19::block_node *block) :
        cdk::basic_node(lineno), _qualifier(qualifier), _expr(expr), _block(block) {
    }

    inline section_node(int lineno, int qualifier, m19::block_node *block) :
        cdk::basic_node(lineno), _qualifier(qualifier), _expr(cdk::expression_node(lineno)), _block(block) {
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
