// $Id: block_node.h,v 1.1 2019/02/28 21:26:01 linux Exp $ -*- c++ -*-
#ifndef __M19_BLOCKNODE_H__
#define __M19_BLOCKNODE_H__

#include <cdk/ast/sequence_node.h>

namespace m19 {

  /**
   * Class for describing block nodes.
   */
  class block_node: public cdk::basic_node {
    cdk::sequence_node *_declaration;
    cdk::sequence_node *_instruction;

  public:
    inline block_node(int lineno, cdk::sequence_node *declaration, cdk::sequence_node *instruction) :
        cdk::basic_node(lineno), _declaration(declaration), _instruction(instruction) {
    }

  public:
    inline cdk::sequence_node *declaration() {
      return _declaration;
    }
    inline cdk::sequence_node *instruction() {
      return _instruction;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_block_node(this, level);
    }

  };

} // m19

#endif
