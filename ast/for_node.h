// $Id: for_node.h,v 1.1 2019/02/28 21:26:01 linux Exp $ -*- c++ -*-
#ifndef __M19_FORNODE_H__
#define __M19_FORNODE_H__

#include <cdk/ast/expression_node.h>
#include <cdk/ast/variable_node.h>

namespace m19 {

  /**
   * Class for describing for-cycle nodes.
   */
  class for_node: public cdk::basic_node {
    cdk::variable_node *_variable;
    cdk::expression_node *_init;
    cdk::expression_node *_stop;
    cdk::expression_node *_step;
    cdk::basic_node *_instruction;

  public:
    inline for_node(int lineno, cdk::expression_node *init, cdk::expression_node *stop, cdk::expression_node *step, cdk::basic_node *instruction) :
        cdk::basic_node(lineno), _init(init), _stop(stop), _step(step), _instruction(instruction) {
    }

    inline for_node(int lineno, cdk::variable_node *variable, cdk::expression_node *stop, cdk::expression_node *step, cdk::basic_node *instruction) :
        cdk::basic_node(lineno), _variable(variable), _stop(stop), _step(step), _instruction(instruction) {
    }

  public:
    inline cdk::variable_node *variable() {
      return _variable;
    }

    inline cdk::expression_node *init() {
      return _init;
    }

    inline cdk::expression_node *stop() {
      return _stop;
    }

    inline cdk::expression_node *step() {
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
