// $Id: stack_alloc_node.h,v 1.2 2019/03/20 18:28:20 ist186806 Exp $
#ifndef __M19_STACKALLOCNODE_H__
#define __M19_STACKALLOCNODE_H__

#include <cdk/ast/unary_expression_node.h>

namespace m19 {

  /**
   * Class for describing the stack allocation nodes.
   */
  class stack_alloc_node: public cdk::unary_expression_node {

  public:
    inline stack_alloc_node(int lineno, cdk::expression_node *argument) :
        cdk::unary_expression_node(lineno, argument) {
    }

  public:
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_stack_alloc_node(this, level);
    }

  };

} // m19

#endif
