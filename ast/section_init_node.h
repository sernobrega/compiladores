// $Id: section_init_node.h,v 1.1 2019/03/20 18:28:20 ist186806 Exp $ -*- c++ -*-
#ifndef __M19_SECTIONINITNODE_H__
#define __M19_SECTIONINITNODE_H__

namespace m19 {

  /**
   * Class for describing section init nodes.
   */
  class section_init_node: public cdk::basic_node {
    m19::block_node *_block;
  
  public:
    inline section_init_node(int lineno, m19::block_node *block) :
        cdk::basic_node(lineno), _block(block) {
    }

  public:
    inline m19::block_node *block() {
      return _block;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_section_init_node(this, level);
    }

  };

} // m19

#endif
