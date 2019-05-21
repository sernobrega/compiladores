#include <string>
#include "targets/type_checker.h"
#include "ast/all.h"  // automatically generated
#include "m19_parser.tab.h"

#define ASSERT_UNSPEC \
    { if (node->type() != nullptr && \
          node->type()->name() != basic_type::TYPE_UNSPEC) return; }

/*****************************           NOT USED           *****************************/

void m19::type_checker::do_nil_node(cdk::nil_node * const node, int lvl) {
  // EMPTY
}
void m19::type_checker::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}

/*****************************           SEQUENCE           *****************************/
void m19::type_checker::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++)
    node->node(i)->accept(this, lvl);
}

/****************************************************************************************
 *****************************       VARIABLE RELATED       *****************************
 ****************************************************************************************/
void m19::type_checker::do_variable_declaration_node(m19::variable_declaration_node * const node, int lvl) {
  if(node->expr() != nullptr) {
    node->expr()->accept(this, lvl+2);

    if (node->type()->name() == basic_type::TYPE_INT) {
      if (node->expr()->type()->name() != basic_type::TYPE_INT) throw std::string(
          "wrong type for expr (integer expected).");
    } else if (node->type()->name() == basic_type::TYPE_DOUBLE) {
      if (node->expr()->type()->name() != basic_type::TYPE_INT
          && node->expr()->type()->name() != basic_type::TYPE_DOUBLE) throw std::string(
          "wrong type for expr (integer or double expected).");
    } else if (node->type()->name() == basic_type::TYPE_STRING) {
      if (node->expr()->type()->name() != basic_type::TYPE_STRING) throw std::string(
          "wrong type for expr (string expected).");
    } else if (node->type()->name() == basic_type::TYPE_POINTER) {
      //DAVID: FIXME: trouble!!!
      if (node->expr()->type()->name() != basic_type::TYPE_POINTER) throw std::string(
          "wrong type for expr (pointer expected).");
    } else {
      throw std::string("unknown type for expr.");
    }
  }

  const std::string &id = node->id();
  std::shared_ptr<m19::symbol> symbol = std::make_shared < m19::symbol > (node->constant(), // is it a constant?
  node->scope(), // scope: public, forward, "private" (i.e., none)
  node->type(), // type (type id + type size)
  id, // identifier
  (bool)node->expr(), // initialized?
  false); // is it a function?
  if (_symtab.insert(id, symbol)) {
    //std::cout << id << " has been declared!" << std::endl;
    _parent->set_new_symbol(symbol);  // advise parent that a symbol has been inserted
  } else {
    throw std::string("variable '" + id + "' redeclared");
  }
}

void m19::type_checker::do_variable_node(cdk::variable_node * const node, int lvl) {
  ASSERT_UNSPEC;
  const std::string &id = node->name();
  std::shared_ptr<m19::symbol> symbol = _symtab.find(id);
  if (symbol) {
    node->type(symbol->type());
  } else {
    throw std::string("undeclared variable '" + id + "'");
  }
}

void m19::type_checker::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_UNSPEC;
  try {
    node->lvalue()->accept(this, lvl);
    node->type(node->lvalue()->type());
  } catch (const std::string &id) {
    throw "undeclared variable '" + id + "'";
  }
}

void m19::type_checker::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->lvalue()->accept(this, lvl + 4);
  node->rvalue()->accept(this, lvl + 4);

  //Integer
  if (node->lvalue()->type()->name() == basic_type::TYPE_INT) {
    if (node->rvalue()->type()->name() == basic_type::TYPE_INT) {
      node->type(new basic_type(4, basic_type::TYPE_INT));
    } else if (node->rvalue()->type()->name() == basic_type::TYPE_UNSPEC) {
      node->type(new basic_type(4, basic_type::TYPE_INT));
      node->rvalue()->type(new basic_type(4, basic_type::TYPE_INT));
    } else
      throw std::string("wrong assignment to integer");
  //Pointer
  } else if (node->lvalue()->type()->name() == basic_type::TYPE_POINTER) {

//TODO: check pointer level

    if (node->rvalue()->type()->name() == basic_type::TYPE_POINTER) {
      node->type(new basic_type(4, basic_type::TYPE_POINTER));
    } else if (node->rvalue()->type()->name() == basic_type::TYPE_INT) {
      //TODO: check that the integer is a literal and that it is zero
      node->type(new basic_type(4, basic_type::TYPE_POINTER));
    } else if (node->rvalue()->type()->name() == basic_type::TYPE_UNSPEC) {
      node->type(new basic_type(4, basic_type::TYPE_ERROR));
      node->rvalue()->type(new basic_type(4, basic_type::TYPE_ERROR));
    } else
      throw std::string("wrong assignment to pointer");
  //Double
  } else if (node->lvalue()->type()->name() == basic_type::TYPE_DOUBLE) {

    if (node->rvalue()->type()->name() == basic_type::TYPE_DOUBLE || node->rvalue()->type()->name() == basic_type::TYPE_INT) {
      node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
    } else if (node->rvalue()->type()->name() == basic_type::TYPE_UNSPEC) {
      node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
      node->rvalue()->type(new basic_type(8, basic_type::TYPE_DOUBLE));
    } else
      throw std::string("wrong assignment to real");

  //String
  } else if (node->lvalue()->type()->name() == basic_type::TYPE_STRING) {
    if (node->rvalue()->type()->name() == basic_type::TYPE_STRING) {
      node->type(new basic_type(4, basic_type::TYPE_STRING));
    } else if (node->rvalue()->type()->name() == basic_type::TYPE_UNSPEC) {
      node->type(new basic_type(4, basic_type::TYPE_STRING));
      node->rvalue()->type(new basic_type(4, basic_type::TYPE_STRING));
    } else
      throw std::string("wrong assignment to string");

  } else {
    throw std::string("wrong types in assignment");
  }
}

//---------------------------------------------------------------------------

void m19::type_checker::do_evaluation_node(m19::evaluation_node * const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}

void m19::type_checker::do_print_node(m19::print_node * const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}

//---------------------------------------------------------------------------

void m19::type_checker::do_read_node(m19::read_node * const node, int lvl) {
  // try {
  //   node->argument()->accept(this, lvl);
  // } catch (const std::string &id) {
  //   throw "undeclared variable '" + id + "'";
  // }
}

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

void m19::type_checker::do_function_declaration_node(m19::function_declaration_node * const node, int lvl) {
  //
}

void m19::type_checker::do_function_call_node(m19::function_call_node * const node, int lvl) {
  ASSERT_UNSPEC;
  const std::string &id = node->id();
  std::shared_ptr<m19::symbol> symbol = _symtab.find(id);

  if (symbol == nullptr) throw std::string("symbol '" + id + "' is undeclared.");

  if (!symbol->isFunction()) throw std::string("symbol '" + id + "' is not a function.");

  node->type(symbol->type());

  //DAVID: FIXME: should also validate args against symbol
  if (node->arguments()) {
    node->arguments()->accept(this, lvl + 4);
  }
}

void m19::type_checker::do_block_node(m19::block_node * const node, int lvl) {
  //
}

void m19::type_checker::do_stack_alloc_node(m19::stack_alloc_node * const node, int lvl) {
  //
}

void m19::type_checker::do_address_node(m19::address_node * const node, int lvl) {
  //
}

void m19::type_checker::do_index_node(m19::index_node * const node, int lvl) {
  //
}

/****************************************************************************************
 *****************************       ITERATION RELATED       ****************************
 *****************************        IF-ELSE RELATED       *****************************
 ****************************************************************************************/
void m19::type_checker::do_if_node(m19::if_node * const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  if (node->condition()->type()->name() != basic_type::TYPE_INT) throw std::string("expected integer condition");
}

void m19::type_checker::do_if_else_node(m19::if_else_node * const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  if (node->condition()->type()->name() != basic_type::TYPE_INT) throw std::string("expected integer condition");
}

void m19::type_checker::do_for_node(m19::for_node * const node, int lvl) {
  node->init()->accept(this, lvl + 4);

  for (size_t i = 0; i < node->stop()->size(); i++) {
      cdk::expression_node * expr = (cdk::expression_node *)node->stop()->node(i);
      if(expr == nullptr) break;
      expr->accept(this, lvl + 4);
      if (expr->type()->name() != basic_type::TYPE_INT) throw std::string(
      "expected integer expression as stop condition of for cycle");
  }
  for (size_t i = 0; i < node->step()->size(); i++) {
      cdk::expression_node * expr = (cdk::expression_node *)node->step()->node(i);
      if(expr == nullptr) break;
      expr->accept(this, lvl + 4);
      if (expr->type()->name() != basic_type::TYPE_INT) throw std::string(
      "expected integer expression as step condition of for cycle");
  }
}

/****************************************************************************************
 *****************************       LOGICAL RELATED        *****************************
 ****************************************************************************************/
void m19::type_checker::do_ScalarLogicalExpression(cdk::binary_expression_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() != basic_type::TYPE_INT && node->left()->type()->name() != basic_type::TYPE_DOUBLE) throw std::string(
      "integer or double expression expected in binary logical expression (left)");

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() != basic_type::TYPE_INT && node->left()->type()->name() != basic_type::TYPE_DOUBLE) throw std::string(
      "integer expression expected in binary logical expression (right)");

  if(node->right()->type()->name() == basic_type::TYPE_DOUBLE || node->left()->type()->name() == basic_type::TYPE_DOUBLE)
    node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
  else
    node->type(new basic_type(4, basic_type::TYPE_INT));
}

void m19::type_checker::do_lt_node(cdk::lt_node * const node, int lvl) {
  do_ScalarLogicalExpression(node, lvl);
}
void m19::type_checker::do_le_node(cdk::le_node * const node, int lvl) {
  do_ScalarLogicalExpression(node, lvl);
}
void m19::type_checker::do_ge_node(cdk::ge_node * const node, int lvl) {
  do_ScalarLogicalExpression(node, lvl);
}
void m19::type_checker::do_gt_node(cdk::gt_node * const node, int lvl) {
  do_ScalarLogicalExpression(node, lvl);
}

void m19::type_checker::do_GeneralLogicalExpression(cdk::binary_expression_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  if (node->left()->type()->name() != node->right()->type()->name()) throw std::string(
      "same type expected on both sides of equality operator");
  node->type(new basic_type(4, basic_type::TYPE_INT));
}

void m19::type_checker::do_ne_node(cdk::ne_node * const node, int lvl) {
  do_GeneralLogicalExpression(node, lvl);
}
void m19::type_checker::do_eq_node(cdk::eq_node * const node, int lvl) {
  do_GeneralLogicalExpression(node, lvl);
}

void m19::type_checker::do_BooleanLogicalExpression(cdk::binary_expression_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() != basic_type::TYPE_INT) throw std::string("integer expression expected in binary expression");

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() != basic_type::TYPE_INT) throw std::string("integer expression expected in binary expression");

  node->type(new basic_type(4, basic_type::TYPE_INT));
}

void m19::type_checker::do_and_node(cdk::and_node * const node, int lvl) {
  do_BooleanLogicalExpression(node, lvl);
}
void m19::type_checker::do_or_node(cdk::or_node * const node, int lvl) {
  do_BooleanLogicalExpression(node, lvl);
}
void m19::type_checker::do_not_node(cdk::not_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (node->argument()->type()->name() == basic_type::TYPE_INT)
    node->type(new basic_type(4, basic_type::TYPE_INT));
  else if (node->argument()->type()->name() == basic_type::TYPE_UNSPEC) {
    node->type(new basic_type(4, basic_type::TYPE_INT));
    node->argument()->type(new basic_type(4, basic_type::TYPE_INT));
  } else
    throw std::string("wrong type in unary logical expression");
}
/****************************************************************************************
 *****************************     ARITHMETIC RELATED       *****************************
 ****************************************************************************************/
void m19::type_checker::do_IntOnlyExpression(cdk::binary_expression_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() != basic_type::TYPE_INT) throw std::string(
      "integer expression expected in binary operator (left)");

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() != basic_type::TYPE_INT) throw std::string(
      "integer expression expected in binary operator (right)");

  node->type(new basic_type(4, basic_type::TYPE_INT));
}

void m19::type_checker::do_IDExpression(cdk::binary_expression_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (node->left()->type()->name() == basic_type::TYPE_DOUBLE && node->right()->type()->name() == basic_type::TYPE_DOUBLE)
    node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
  else if (node->left()->type()->name() == basic_type::TYPE_DOUBLE && node->right()->type()->name() == basic_type::TYPE_INT)
    node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
  else if (node->left()->type()->name() == basic_type::TYPE_INT && node->right()->type()->name() == basic_type::TYPE_DOUBLE)
    node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
  else if (node->left()->type()->name() == basic_type::TYPE_INT && node->right()->type()->name() == basic_type::TYPE_INT)
    node->type(new basic_type(4, basic_type::TYPE_INT));
  else if (node->left()->type()->name() == basic_type::TYPE_UNSPEC && node->right()->type()->name() == basic_type::TYPE_UNSPEC) {
    node->type(new basic_type(4, basic_type::TYPE_INT));
    node->left()->type(new basic_type(4, basic_type::TYPE_INT));
    node->right()->type(new basic_type(4, basic_type::TYPE_INT));
  } else
    throw std::string("wrong types in binary expression");
}

void m19::type_checker::do_PIDExpression(cdk::binary_expression_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (node->left()->type()->name() == basic_type::TYPE_DOUBLE && node->right()->type()->name() == basic_type::TYPE_DOUBLE)
    node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
  else if (node->left()->type()->name() == basic_type::TYPE_DOUBLE && node->right()->type()->name() == basic_type::TYPE_INT)
    node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
  else if (node->left()->type()->name() == basic_type::TYPE_INT && node->right()->type()->name() == basic_type::TYPE_DOUBLE)
    node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
  else if (node->left()->type()->name() == basic_type::TYPE_POINTER && node->right()->type()->name() == basic_type::TYPE_INT)
    node->type(new basic_type(4, basic_type::TYPE_POINTER));
  else if (node->left()->type()->name() == basic_type::TYPE_INT && node->right()->type()->name() == basic_type::TYPE_POINTER)
    node->type(new basic_type(4, basic_type::TYPE_POINTER));
  else if (node->left()->type()->name() == basic_type::TYPE_INT && node->right()->type()->name() == basic_type::TYPE_INT)
    node->type(new basic_type(4, basic_type::TYPE_INT));
  else if (node->left()->type()->name() == basic_type::TYPE_UNSPEC && node->right()->type()->name() == basic_type::TYPE_UNSPEC) {
    node->type(new basic_type(4, basic_type::TYPE_INT));
    node->left()->type(new basic_type(4, basic_type::TYPE_INT));
    node->right()->type(new basic_type(4, basic_type::TYPE_INT));
  } else
    throw std::string("wrong types in binary expression");
}

void m19::type_checker::do_add_node(cdk::add_node * const node, int lvl) {
  do_PIDExpression(node, lvl);
}
void m19::type_checker::do_sub_node(cdk::sub_node * const node, int lvl) {
  do_PIDExpression(node, lvl);
}
void m19::type_checker::do_mul_node(cdk::mul_node * const node, int lvl) {
  do_IDExpression(node, lvl);
}
void m19::type_checker::do_div_node(cdk::div_node * const node, int lvl) {
  do_IDExpression(node, lvl);
}
void m19::type_checker::do_mod_node(cdk::mod_node * const node, int lvl) {
  do_IntOnlyExpression(node, lvl);
}

void m19::type_checker::do_unaryIDExpression(cdk::unary_expression_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl);
  if (node->argument()->type()->name() == basic_type::TYPE_INT || node->argument()->type()->name() == basic_type::TYPE_DOUBLE)
    node->type(node->argument()->type());
  else
    throw std::string("integer or vector expressions expected");
}
void m19::type_checker::do_neg_node(cdk::neg_node * const node, int lvl) {
  do_unaryIDExpression(node, lvl);
}
void m19::type_checker::do_identity_node(m19::identity_node * const node, int lvl) {
  do_unaryIDExpression(node, lvl);
}

/****************************************************************************************
 *****************************        TYPES RELATED         *****************************
 ****************************************************************************************/
void m19::type_checker::do_integer_node(cdk::integer_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type(4, basic_type::TYPE_INT));
}

void m19::type_checker::do_double_node(cdk::double_node * const node, int lvl) {
  // EMPTY
}

void m19::type_checker::do_string_node(cdk::string_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type(4, basic_type::TYPE_STRING));
}

/****************************************************************************************
 *****************************       FUNCTION RELATED       *****************************
 ****************************************************************************************/
void m19::type_checker::do_function_definition_node(m19::function_definition_node * const node, int lvl) {
  std::string id;

  if(node->id() == "m19")
    id = "_main";
  else if(node->id() == "_main")
    id = "._main";
  else
    id = node->id();

  std::shared_ptr<m19::symbol> function = 
      std::make_shared < m19::symbol> (false, node->scope(), node->type(), id, false, true);

  if(node->type()->name != basic_type::TYPE_VOID)    
    function->set_offset(-node->type()->size()); //return val

  std::shared_ptr<m19::symbol> previous = _symtab.find(function->name());

  /* Symbol doesn't exist */
  if(!previous) {
    _symtab.insert(function->name(), function);
    _parent->set_new_symbol(function);
    return;
  }

  /* Checking if previous declaration is not function */
  if(previous != nullptr && !previous->isFunction())
    throw std::string("Redefinition of function " + function->name() + " is invalid. Variable declared with same name.");

  /* Checking if previous has been defined already */
  if(previous != nullptr && !previous->fundecl())
    throw std::string("Function " + function->name() + " has already been defined.");

  /* Checking for conflicts with previous definitions */
  if(previous->scope() != node->scope() || previous->type() != node->type()) {
    throw std::string("Redefinition of function " + function->name() + " is invalid. Function declared with the same name but incompatible.");
  }

  //FIXME: check arguments?

  _symtab.replace(function->name(), function);
  _parent->set_new_symbol(function);
}

void m19::type_checker::do_section_node(m19::section_node * const node, int lvl) {
  //
}

void m19::type_checker::do_section_end_node(m19::section_end_node * const node, int lvl) {
  //
}

void m19::type_checker::do_section_init_node(m19::section_init_node * const node, int lvl) {
  //
}

void m19::type_checker::do_continue_node(m19::continue_node * const node, int lvl) {
  // NOTHING TO TYPE CHECK
}

void m19::type_checker::do_return_node(m19::return_node * const node, int lvl) {
  // NOTHING TO TYPE CHECK
}

void m19::type_checker::do_stop_node(m19::stop_node * const node, int lvl) {
  // NOTHING TO TYPE CHECK
}