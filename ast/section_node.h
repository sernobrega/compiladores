// $Id: section_node.h,v 1.1 2019/02/28 21:26:01 linux Exp $ -*- c++ -*-
#ifndef __M19_SECTIONNODE_H__
#define __M19_SECTIONNODE_H__

namespace m19 {

  /**
   * Class for describing section nodes.
   */
  class section_node: public cdk::basic_node {
    cdk::expression_node *_expr;
    m19::block_node *_block;
  
  public:
    inline section_node(int lineno, cdk::expression_node *expr, m19::block_node *block) :
        cdk::basic_node(lineno), _expr(expr), _block(block) {
    }

  public:
    inline cdk::expression_node * expr() {
      return _expr;
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
