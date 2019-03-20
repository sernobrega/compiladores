// $Id: section_end_node.h,v 1.1 2019/02/28 21:26:01 linux Exp $ -*- c++ -*-
#ifndef __M19_SECTIONENDNODE_H__
#define __M19_SECTIONENDNODE_H__

namespace m19 {

  /**
   * Class for describing section end nodes.
   */
  class section_end_node: public cdk::basic_node {
    int _qualifier;
    cdk::expression_node * _expr;
    m19::block_node *_block;
  
  public: //Qualifier contains information if the section is exclusive, inclusive or just a block.
    inline section_end_node(int lineno, int qualifier, cdk::expression_node * expr, cdk::block_node *block) :
        cdk::basic_node(lineno), _qualifier(qualifier), _expr(expr), _block(block) {
    }

  public:
    inline m19::block_node *block() {
      return _block;
    }

    inline int qualifier() {
      return _qualifier;
    }

    inline cdk::expression_node * expr() {
      return _expr;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_section_end_node(this, level);
    }

  };

} // m19

#endif
