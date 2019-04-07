// $Id: body_node.h,v 1.1 2019/03/20 18:28:19 ist186806 Exp $ -*- c++ -*-
#ifndef __M19_BODYNODE_H__
#define __M19_BODYNODE_H__

#include <cdk/ast/sequence_node.h>

namespace m19 {

  /**
   * Class for describing body nodes.
   */
  class body_node: public cdk::basic_node {
    m19::section_init_node *_init;
    m19::section_node *_sec;
    m19::section_end_node *_end;

  public:
    inline body_node(int lineno, m19::section_init_node *init, m19::section_node *sec, m19::section_end_node *end) :
        cdk::basic_node(lineno), _init(init), _sec(sec), _end(end) {
    }

  public:
    inline m19::section_init_node *init() {
      return _init;
    }
    
    inline m19::section_node *sec() {
      return _sec;
    }

    inline m19::section_end_node *end() {
      return _end;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_body_node(this, level);
    }

  };

} // m19

#endif
