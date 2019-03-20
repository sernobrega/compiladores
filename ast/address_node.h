// $Id: address_node.h,v 1.1 2019/02/28 21:26:01 linux Exp $ -*- c++ -*-
#ifndef __M19_ADDRESSNODE_H__
#define __M19_ADDRESSNODE_H__

#include <cdk/ast/lvalue_node.h>

namespace m19 {

  /**
   * Class for describing address nodes.
   */
  class address_node: public cdk::expression_node {
    cdk::lvalue_node *_lval;
  
  public:
    inline address_node(int lineno, cdk::lvalue_node* lval) :
        cdk::expression_node(lineno), _lval(lval) {
    }

  public:
    inline cdk::lvalue_node* lval() {
      return _lval;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_address_node(this, level);
    }

  };

} // m19

#endif
