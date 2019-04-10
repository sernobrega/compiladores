// $Id: identity_node.h,v 1.1 2019/03/20 18:28:19 ist186806 Exp $ -*- c++ -*-
#ifndef __M19_IDENTITYNODE_H__
#define __M19_IDENTITYNODE_H__

namespace m19 {

  /**
   * Class for describing identity nodes.
   */
  class identity_node: public cdk::unary_expression_node {

  public:
    inline identity_node(int lineno, cdk::expression_node* arg) :
         cdk::unary_expression_node(lineno, arg) {
    }

  public:
    
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_identity_node(this, level);
    }

  };

} // m19

#endif
