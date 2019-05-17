
#include <string>
#include <sstream>
#include "targets/type_checker.h"
#include "targets/postfix_writer.h"
#include "ast/all.h"  // all.h is automatically generated
#include "m19_parser.tab.h"

//---------------------------------------------------------------------------

void m19::postfix_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  // EMPTY
}
void m19::postfix_writer::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}
void m19::postfix_writer::do_double_node(cdk::double_node * const node, int lvl) {
  // EMPTY
}
void m19::postfix_writer::do_not_node(cdk::not_node * const node, int lvl) {
  // EMPTY
}
void m19::postfix_writer::do_and_node(cdk::and_node * const node, int lvl) {
  // EMPTY
}
void m19::postfix_writer::do_or_node(cdk::or_node * const node, int lvl) {
  // EMPTY
}

//---------------------------------------------------------------------------

void m19::postfix_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    node->node(i)->accept(this, lvl);
  }
}

//---------------------------------------------------------------------------

void m19::postfix_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  _pf.INT(node->value()); // push an integer
}

void m19::postfix_writer::do_string_node(cdk::string_node * const node, int lvl) {
  int lbl1;

  /* generate the string */
  _pf.RODATA(); // strings are DATA readonly
  _pf.ALIGN(); // make sure we are aligned
  _pf.LABEL(mklbl(lbl1 = ++_lbl)); // give the string a name
  _pf.SSTRING(node->value()); // output string characters

  /* leave the address on the stack */
  _pf.TEXT(); // return to the TEXT segment
  _pf.ADDR(mklbl(lbl1)); // the string to be printed
}

//---------------------------------------------------------------------------

void m19::postfix_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  _pf.NEG(); // 2-complement
}

//---------------------------------------------------------------------------

void m19::postfix_writer::do_add_node(cdk::add_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.ADD();
}
void m19::postfix_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.SUB();
}
void m19::postfix_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MUL();
}
void m19::postfix_writer::do_div_node(cdk::div_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.DIV();
}
void m19::postfix_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MOD();
}
void m19::postfix_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.LT();
}
void m19::postfix_writer::do_le_node(cdk::le_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.LE();
}
void m19::postfix_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.GE();
}
void m19::postfix_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.GT();
}
void m19::postfix_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.NE();
}
void m19::postfix_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.EQ();
}

//---------------------------------------------------------------------------

void m19::postfix_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  // simplified generation: all variables are global
  _pf.ADDR(node->name());
}

void m19::postfix_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl);
  _pf.LDINT(); // depends on type size
}

void m19::postfix_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->rvalue()->accept(this, lvl); // determine the new value
  _pf.DUP32();
  if (new_symbol() == nullptr) {
    node->lvalue()->accept(this, lvl); // where to store the value
  } else {
    _pf.DATA(); // variables are all global and live in DATA
    _pf.ALIGN(); // make sure we are aligned
    _pf.LABEL(new_symbol()->name()); // name variable location
    reset_new_symbol();
    _pf.SINT(0); // initialize it to 0 (zero)
    _pf.TEXT(); // return to the TEXT segment
    node->lvalue()->accept(this, lvl);  //DAVID: bah!
  }
  _pf.STINT(); // store the value at address
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void m19::postfix_writer::do_evaluation_node(m19::evaluation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  if (node->argument()->type()->name() == basic_type::TYPE_INT) {
    _pf.TRASH(4); // delete the evaluated value
  } else if (node->argument()->type()->name() == basic_type::TYPE_STRING) {
    _pf.TRASH(4); // delete the evaluated value's address
  } else {
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }
}

void m19::postfix_writer::do_print_node(m19::print_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value to print
  if (node->argument()->type()->name() == basic_type::TYPE_INT) {
    _pf.CALL("printi");
    _pf.TRASH(4); // delete the printed value
  } else if (node->argument()->type()->name() == basic_type::TYPE_STRING) {
    _pf.CALL("prints");
    _pf.TRASH(4); // delete the printed value's address
  } else {
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }
  _pf.CALL("println"); // print a newline
}

//---------------------------------------------------------------------------

void m19::postfix_writer::do_read_node(m19::read_node * const node, int lvl) {
  // ASSERT_SAFE_EXPRESSIONS;
  // _pf.CALL("readi");
  // _pf.LDFVAL32();
  // node->argument()->accept(this, lvl);
  // _pf.STINT();
}

//---------------------------------------------------------------------------

void m19::postfix_writer::do_variable_declaration_node(m19::variable_declaration_node * const node, int lvl) {
  //
}

void m19::postfix_writer::do_function_declaration_node(m19::function_declaration_node * const node, int lvl) {
  //
}

void m19::postfix_writer::do_function_call_node(m19::function_call_node * const node, int lvl) {
  //
}

void m19::postfix_writer::do_block_node(m19::block_node * const node, int lvl) {
  //
}

void m19::postfix_writer::do_stack_alloc_node(m19::stack_alloc_node * const node, int lvl) {
  //
}

void m19::postfix_writer::do_address_node(m19::address_node * const node, int lvl) {
  //
}

void m19::postfix_writer::do_index_node(m19::index_node * const node, int lvl) {
  //
}

void m19::postfix_writer::do_section_node(m19::section_node * const node, int lvl) {
  //
}

void m19::postfix_writer::do_section_end_node(m19::section_end_node * const node, int lvl) {
  //
}

void m19::postfix_writer::do_section_init_node(m19::section_init_node * const node, int lvl) {
  //
}

void m19::postfix_writer::do_continue_node(m19::continue_node * const node, int lvl) {
  //
}

void m19::postfix_writer::do_return_node(m19::return_node * const node, int lvl) {
  //
}

void m19::postfix_writer::do_stop_node(m19::stop_node * const node, int lvl) {
  //
}

void m19::postfix_writer::do_identity_node(m19::identity_node * const node, int lvl) {
  //
}

//---------------------------------------------------------------------------

void m19::postfix_writer::do_for_node(m19::for_node * const node, int lvl) {
  // ASSERT_SAFE_EXPRESSIONS;
  // int lbl1, lbl2;
  // _pf.LABEL(mklbl(lbl1 = ++_lbl));
  // node->condition()->accept(this, lvl);
  // _pf.JZ(mklbl(lbl2 = ++_lbl));
  // node->block()->accept(this, lvl + 2);
  // _pf.JMP(mklbl(lbl1));
  // _pf.LABEL(mklbl(lbl2));
}

//---------------------------------------------------------------------------

void m19::postfix_writer::do_if_node(m19::if_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1));
}

//---------------------------------------------------------------------------

void m19::postfix_writer::do_if_else_node(m19::if_else_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->thenblock()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl2 = ++_lbl));
  _pf.LABEL(mklbl(lbl1));
  node->elseblock()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1 = lbl2));
}


//--
void m19::postfix_writer::do_function_definition_node(m19::function_definition_node * const node, int lvl) {
  //ASSERT_SAFE;
  //FIXME: naive approach - what if functions are defined inside a block or in an argument
  bool isMain = (node->id() == "m19");

  _function = new_symbol();
  _functions_to_declare.erase(_function->name());
  reset_new_symbol();

  _offset = 8; //FP IP
  _symtab.push();
  if(node->arguments()) {
    for(size_t ix = 0; ix < node->arguments()->size(); ix++) {
      cdk::basic_node * arg = node->arguments()->node(ix);
      if(arg == nullptr) break;
      arg->accept(this, 0);
    }
  }

  _pf.TEXT();
  _pf.ALIGN();
  if(isMain) {
    _pf.GLOBAL("_main", _pf.FUNC());
    _pf.LABEL("_main");
  } else if(node->scope() == tPUBLIC) {
    _pf.GLOBAL(_function->name(), _pf.FUNC());
    _pf.LABEL(_function->name());
  } else { //private function
    _pf.LABEL(_function->name());
  }
  
  //RETURN VALUE
  //SECTIONS
  //SOMETHING FOR ENTER
  _pf.ENTER(0)

  _offset = -_function->type()->size(); //retval

  //sections
  if(node->init()) node->init()->accept(this, lvl + 4);
  if(node->section()) {
    for(size_t ix = 0; ix < node->section()->size(); ix++) {
      m19::section_node * sec = (m19::section_node *)node->section()->node(ix);
      if(sec == nullptr) break;
      sec->accept(this, lvl + 6);
    }
  }
  if(node->end()) node->end()->accept(this, lvl + 4);

  // end the main function
  _pf.INT(0);
  _pf.STFVAL32();
  _pf.LEAVE();
  _pf.RET();

  // _pf.LEAVE();
  // _pf.RET();
  
  //main function (m19) is being defined, functions to be declared are extern
  if(isMain) 
    for(std::string s: _functions_to_declare)
      _pf.EXTERN(s)
}
