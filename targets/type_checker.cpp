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

/*****************************             BLOCK            *****************************/
void m19::type_checker::do_block_node(m19::block_node * const node, int lvl) {
  // EMPTY
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
      int nodet = 0, exprt = 0;
      basic_type * nodetype = node->type();
      for(; nodetype->name() == basic_type::TYPE_POINTER; nodet++, nodetype = nodetype->_subtype);
      basic_type * exprtype = node->expr()->type();
      for(; exprtype->name() == basic_type::TYPE_POINTER; exprt++, exprtype = exprtype->_subtype);

      bool compatible = (nodet == exprt) && (exprtype == 0 || (exprtype != 0 && nodetype->name() == exprtype->name()));
      if (!compatible) throw std::string("wrong type for return expression (pointer expected).");
    } else {
      throw std::string("unknown type for expr.");
    }
  }

  const std::string &id = node->id();
  std::shared_ptr<m19::symbol> symbol = std::make_shared < m19::symbol > (node->constant(), // is it a constant?
  node->scope(), // scope: public, "private" (i.e., none)
  node->type(), // type (type id + type size)
  id, // identifier
  (bool)node->expr(), // initialized?
  false); // is it a function?
  std::shared_ptr<m19::symbol> previous = _symtab.find_local(id);
  if (!previous) {
    _symtab.insert(id, symbol);
    _parent->set_new_symbol(symbol);  // advise parent that a symbol has been inserted
  } else {
    throw std::string("variable '" + id + "' redeclared");
  }
}

void m19::type_checker::do_variable_node(cdk::variable_node * const node, int lvl) {
  ASSERT_UNSPEC;

  if(node->name() == "@") {
    node->type(_function->type());
    return;
  }
    
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
    if (node->rvalue()->type()->name() == basic_type::TYPE_POINTER) {
      int lt = 0, rt = 0;
      basic_type * ltype = node->lvalue()->type();
      for(; ltype->name() == basic_type::TYPE_POINTER; lt++, ltype = ltype->_subtype);

      basic_type * rtype = node->rvalue()->type();
      for(; rtype->name() == basic_type::TYPE_POINTER; rt++, rtype = rtype->_subtype);

      bool compatible = ((lt == rt - 1) && (rtype->name() != basic_type::TYPE_INT)) || ((lt == rt) && (rt == 0 ));
      if (!compatible) throw std::string("wrong assignment to pointer");

      if(lt == rt-1 && !_nullptr) {
        throw std::string("wrong assignment to pointer");
      }

      basic_type * pointertype = new basic_type(4, basic_type::TYPE_POINTER);
      basic_type * subtypeholder = new basic_type(0, basic_type::TYPE_UNSPEC);
      pointertype->_subtype = subtypeholder;
      for(; lt > 0; lt--, subtypeholder = pointertype->_subtype) {
        subtypeholder->_subtype = new basic_type(4, basic_type::TYPE_POINTER);
      }
      subtypeholder->_subtype = ltype;
      node->type(pointertype);
    } else if (node->rvalue()->type()->name() == basic_type::TYPE_INT) {
      if(_nullptr) 
        node->type(new basic_type(4, basic_type::TYPE_POINTER));
      else
        throw std::string("wrong assignment to pointer");
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

  _nullptr = false;
}

void m19::type_checker::do_evaluation_node(m19::evaluation_node * const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}

void m19::type_checker::do_print_node(m19::print_node * const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
  if (node->argument()->type()->name() == basic_type::TYPE_VOID)
    throw std::string("wrong type in print argument");
}

void m19::type_checker::do_read_node(m19::read_node * const node, int lvl) {
  node->type(new basic_type(0, basic_type::TYPE_UNSPEC));
}

void m19::type_checker::do_stack_alloc_node(m19::stack_alloc_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (node->argument()->type()->name() != basic_type::TYPE_INT) throw std::string(
      "integer expression expected in allocation expression");
//FIXME: check the following two lines
  auto mytype = new basic_type(4, basic_type::TYPE_POINTER);
  mytype->_subtype = new basic_type(8, basic_type::TYPE_DOUBLE);
  node->type(mytype);
}

void m19::type_checker::do_address_node(m19::address_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->lval()->accept(this, lvl + 2);
  if (node->lval()->type()->name() == basic_type::TYPE_DOUBLE) {
    node->type(new basic_type(4, basic_type::TYPE_POINTER));
  } else {
    throw std::string("wrong type in unary logical expression");
  }
}

void m19::type_checker::do_index_node(m19::index_node * const node, int lvl) {
  ASSERT_UNSPEC;
  if (node->expr()) {
    node->expr()->accept(this, lvl + 2);
    if (node->expr()->type()->name() != basic_type::TYPE_POINTER) throw std::string(
        "pointer expression expected in index left-value");
  } else {
    if (_function->type()->name() != basic_type::TYPE_POINTER) throw std::string(
        "return pointer expression expected in index left-value");
  }
  node->index()->accept(this, lvl + 2);
  if (node->index()->type()->name() != basic_type::TYPE_INT) throw std::string("integer expression expected in left-value index");
  node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
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
  // EMPTY
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

void m19::type_checker::do_unaryIDExpression(cdk::unary_expression_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl);
  if (node->argument()->type()->name() == basic_type::TYPE_INT || node->argument()->type()->name() == basic_type::TYPE_DOUBLE)
    node->type(node->argument()->type());
  else
    throw std::string("integer or vector expressions expected");
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
  else if ((node->left()->type()->name() == basic_type::TYPE_POINTER && node->right()->type()->name() == basic_type::TYPE_INT)
      || (node->left()->type()->name() == basic_type::TYPE_INT && node->right()->type()->name() == basic_type::TYPE_POINTER)) {
    int lt = 0, rt = 0;
    basic_type * ltype = node->left()->type();
    for(; ltype->name() == basic_type::TYPE_POINTER; lt++, ltype = ltype->_subtype);

    basic_type * rtype = node->right()->type();
    for(; rtype->name() == basic_type::TYPE_POINTER; rt++, rtype = rtype->_subtype);

    basic_type * pointertype = new basic_type(4, basic_type::TYPE_POINTER);
    basic_type * subtypeholder = new basic_type(0, basic_type::TYPE_UNSPEC);
    pointertype->_subtype = subtypeholder;
    for(; lt > 0; lt--, subtypeholder = pointertype->_subtype) {
      subtypeholder->_subtype = new basic_type(4, basic_type::TYPE_POINTER);
    }
    subtypeholder->_subtype = ltype;
    node->type(pointertype);
  }
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
  if(node->value() == 0) {
    _nullptr = true;
  }
}

void m19::type_checker::do_double_node(cdk::double_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
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
 
  if (node->arguments()) {
    std::vector<basic_type*> symargs;
    for (size_t ix = 0; ix < node->arguments()->size(); ix++) {
      m19::variable_declaration_node *arg = dynamic_cast<m19::variable_declaration_node*>(node->arguments()->node(ix));
      basic_type * new_type = new basic_type(arg->type()->size(), arg->type()->name());
      symargs.push_back(new_type);
    }
    function->set_args(symargs);
  }
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
  if(previous->scope() != node->scope() || previous->type()->name() != node->type()->name()) {
    throw std::string("Redefinition of function " + function->name() + " is invalid. Function declared with the same name but incompatible.");
  }

  if(node->arguments()) {
    if(function->args().size() != previous->args().size())
    throw std::string("Redefinition of function " + function->name() + " is invalid. Function declared with the same name but incompatible args.");

    size_t ix = 0;
    for (ix = 0; ix < function->args().size() || ix < previous->args().size(); ix++) {
      if(function->args().at(ix)->name() != previous->args().at(ix)->name())
        throw std::string("Redefinition of function " + function->name() + " is invalid. Function declared with the same name but incompatible args.");
    }

    if(ix < function->args().size() || ix < previous->args().size())
      throw std::string("Redefinition of function " + function->name() + " is invalid. Function declared with the same name but incompatible args.");
  }

  _symtab.replace(function->name(), function);
  _parent->set_new_symbol(function);
}

void m19::type_checker::do_function_call_node(m19::function_call_node * const node, int lvl) {
  ASSERT_UNSPEC;
  
  const std::string &id = node->id() == "@" ? _function->name() : node->id();
  std::shared_ptr<m19::symbol> symbol = _symtab.find(id);

  if (symbol == nullptr) throw std::string("symbol '" + id + "' is undeclared.");

  if (!symbol->isFunction()) throw std::string("symbol '" + id + "' is not a function.");

  if(node->arguments()->size() != symbol->args().size())
        throw std::string("conflicting calling for '" + id + "'");

  if (node->arguments()) {
    for (size_t ix = 0; ix < node->arguments()->size(); ix++) {
      cdk::expression_node *arg = dynamic_cast<cdk::expression_node*>(node->arguments()->node(ix));
      arg->accept(this, lvl + 2);

      if(symbol->args().at(ix)->name() != arg->type()->name() && !(symbol->args().at(ix)->name() == basic_type::TYPE_DOUBLE && arg->type()->name() == basic_type::TYPE_INT))
        throw std::string("conflicting declaration for '" + id + "'");
    }
  }

  node->type(symbol->type());
}

void m19::type_checker::do_function_declaration_node(m19::function_declaration_node * const node, int lvl) {
  std::string id;

  if (node->id() == "m19")
    id = "_main";
  else if (node->id() == "_main")
    id = "._main";
  else
    id = node->id();

  std::shared_ptr<m19::symbol> function = std::make_shared < m19::symbol
      > (false, node->qualifier(), node->type(), id, false, true, true);

  std::shared_ptr<m19::symbol> previous = _symtab.find(function->name());
  if (previous) {
      if(!previous->args().empty()) {
        if(function->args().size() != previous->args().size())
        throw std::string("conflicting declaration for '" + function->name() + "'");

        size_t ix = 0;
        for (ix = 0; ix < function->args().size() || ix < previous->args().size(); ix++) {
          if(function->args().at(ix)->name() != previous->args().at(ix)->name())
            throw std::string("conflicting declaration for '" + function->name() + "'");
        }

        if(ix < function->args().size() || ix < previous->args().size())
          throw std::string("conflicting declaration for '" + function->name() + "'");
      }
  } else {
    if (node->arguments()) {
      std::vector<basic_type*> symargs;
      for (size_t ix = 0; ix < node->arguments()->size(); ix++) {
        m19::variable_declaration_node *arg = dynamic_cast<m19::variable_declaration_node*>(node->arguments()->node(ix));
        basic_type * new_type = new basic_type(arg->type()->size(), arg->type()->name());
        symargs.push_back(new_type);
      }
      function->set_args(symargs);
    }
    _symtab.insert(function->name(), function);
    _parent->set_new_symbol(function);
  }
}

/****************************************************************************************
 *****************************       SECTIONS RELATED       *****************************
 ****************************************************************************************/
void m19::type_checker::do_section_node(m19::section_node * const node, int lvl) {
  if(node->expr())  {
    node->expr()->accept(this, lvl + 2);
    if (node->expr()->type()->name() != basic_type::TYPE_INT) throw std::string("wrong type for expr (integer expected).");
  }
}

void m19::type_checker::do_section_end_node(m19::section_end_node * const node, int lvl) {
  // NOTHING TO TYPE CHECK
}

void m19::type_checker::do_section_init_node(m19::section_init_node * const node, int lvl) {
  // NOTHING TO TYPE CHECK
}

/****************************************************************************************
 *****************************    CONTINUE, RETURN, STOP    *****************************
 ****************************************************************************************/
void m19::type_checker::do_continue_node(m19::continue_node * const node, int lvl) {
  // NOTHING TO TYPE CHECK
}

void m19::type_checker::do_return_node(m19::return_node * const node, int lvl) {
  // NOTHING TO TYPE CHECK
}

void m19::type_checker::do_stop_node(m19::stop_node * const node, int lvl) {
  // NOTHING TO TYPE CHECK
}