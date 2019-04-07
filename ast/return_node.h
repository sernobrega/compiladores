// $Id: return_node.h,v 1.1 2019/03/20 18:28:19 ist186806 Exp $ -*- c++ -*-
#ifndef __M19_RETURNNODE_H__
#define __M19_RETURNNODE_H__

namespace m19 {

  /**
   * Class for describing return nodes.
   */
  class return_node: public cdk::basic_node {

  public:
    inline return_node(int lineno) :
        cdk::basic_node(lineno) {
    }

  public:

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_return_node(this, level);
    }

  };

} // m19

#endif
