// $Id: index_node.h,v 1.1 2019/02/28 21:26:01 linux Exp $ -*- c++ -*-
#ifndef __M19_INDEXNODE_H__
#define __M19_INDEXNODE_H__

namespace m19 {

  /**
   * Class for describing indexation nodes.
   */
  class index_node: public cdk::expression_node {
    cdk::expression_node *_expr;
    cdk::expression_node *_index;
  
  public:
    inline index_node(int lineno, cdk::expression_node *expr, cdk::expression_node *index) :
        cdk::expression_node(lineno), _expr(expr), _index(index) {
    }

  public:
    inline cdk::expression_node * expr() {
      return _expr;
    }

    inline cdk::expression_node * index() {
      return _index;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_index_node(this, level);
    }

  };

} // m19

#endif
