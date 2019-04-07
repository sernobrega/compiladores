// $Id: stop_node.h,v 1.1 2019/03/20 18:28:21 ist186806 Exp $ -*- c++ -*-
#ifndef __M19_STOPNODE_H__
#define __M19_STOPNODE_H__

namespace m19 {

  /**
   * Class for describing stop nodes.
   */
  class stop_node: public cdk::basic_node {

  public:
    inline stop_node(int lineno) :
        cdk::basic_node(lineno) {
    }

  public:

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_stop_node(this, level);
    }

  };

} // m19

#endif
