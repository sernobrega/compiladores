// $Id: print_node.h,v 1.2 2019/03/20 18:28:19 ist186806 Exp $ -*- c++ -*-
#ifndef __M19_PRINTNODE_H__
#define __M19_PRINTNODE_H__

#include <cdk/ast/expression_node.h>

namespace m19 {

  /**
   * Class for describing print nodes.
   */
  class print_node: public cdk::basic_node {
    cdk::expression_node *_argument;
    bool _nl = false;

  public:
    print_node(int lineno, cdk::expression_node *argument, bool nl = false) :
        cdk::basic_node(lineno), _argument(argument), _nl(nl) {
    }

  public:
    inline cdk::expression_node *argument() {
      return _argument;
    }

    bool newline() {
      return _nl;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_print_node(this, level);
    }

  };

} // m19

#endif
