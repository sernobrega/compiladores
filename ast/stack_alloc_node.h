// $Id: stack_alloc_node.h,v 1.1 2019/02/28 21:26:01 linux Exp $
#ifndef __M19_STACKALLOCNODE_H__
#define __M19_STACKALLOCNODE_H__

#include <cdk/ast/unary_expression_node.h>

namespace m19 {

  /**
   * Class for describing the stack allocation nodes.
   */
  class stack_alloc_node: public cdk::basic_node {

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
