// $Id: continue_node.h,v 1.1 2019/03/20 18:28:19 ist186806 Exp $ -*- c++ -*-
#ifndef __M19_CONTINUENODE_H__
#define __M19_CONTINUENODE_H__

namespace m19 {

  /**
   * Class for describing continue nodes.
   */
  class continue_node: public cdk::basic_node {

  public:
    inline continue_node(int lineno) :
        cdk::basic_node(lineno) {
    }

  public:

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_continue_node(this, level);
    }

  };

} // m19

#endif
