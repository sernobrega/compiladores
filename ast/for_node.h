// $Id: for_node.h,v 1.1 2019/03/19 17:35:56 ist186806 Exp $ -*- c++ -*-
#ifndef __M19_FORNODE_H__
#define __M19_FORNODE_H__

#include <cdk/ast/expression_node.h>
#include <cdk/ast/variable_node.h>

namespace m19 {

  /**
   * Class for describing for-cycle nodes.
   */
  class for_node: public cdk::basic_node {
    cdk::sequence_node *_init;
    cdk::sequence_node *_stop;
    cdk::sequence_node *_step;
    cdk::basic_node *_instruction;

  public:
    inline for_node(int lineno, cdk::sequence_node *init, cdk::sequence_node *stop, cdk::sequence_node *step, cdk::basic_node *instruction) :
        cdk::basic_node(lineno), _init(init), _stop(stop), _step(step), _instruction(instruction) {
    }

  public:
    inline cdk::sequence_node *init() {
      return _init;
    }

    inline cdk::sequence_node *stop() {
      return _stop;
    }

    inline cdk::sequence_node *step() {
      return _step;
    }

    inline cdk::basic_node *instruction() {
      return _instruction;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_for_node(this, level);
    }

  };

} // m19

#endif
