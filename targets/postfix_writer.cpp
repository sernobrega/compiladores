
#include <string>
#include <sstream>
#include "targets/type_checker.h"
#include "targets/postfix_writer.h"
#include "targets/frame_size_calculator.h"
#include "ast/all.h"  // all.h is automatically generated
#include "m19_parser.tab.h"

/*****************************           NOT USED           *****************************/
void m19::postfix_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  // EMPTY
}
void m19::postfix_writer::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}

/*****************************           SEQUENCE           *****************************/
void m19::postfix_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    node->node(i)->accept(this, lvl);
  }
}

void m19::postfix_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if(node->name() == "@") {
    _pf.LOCAL(_function->offset());
    return;
  }

  const std::string &id = node->name();
  std::shared_ptr<m19::symbol> symbol = _symtab.find(id);
  if (symbol->global()) {
    _pf.ADDR(symbol->name());
  }
  else {
    _pf.LOCAL(symbol->offset());
  }
}

void m19::postfix_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl);
  node->type()->name() == basic_type::TYPE_DOUBLE ? _pf.LDDOUBLE(): _pf.LDINT(); 
}

void m19::postfix_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->rvalue()->accept(this, lvl + 2);
  if (node->type()->name() == basic_type::TYPE_DOUBLE) {
    if (node->rvalue()->type()->name() == basic_type::TYPE_INT)
    _pf.I2D();
    _pf.DUP64();
  } else {
    _pf.DUP32();
  }

  node->lvalue()->accept(this, lvl);
  if (node->type()->name() == basic_type::TYPE_DOUBLE) {
    _pf.STDOUBLE();
  } else {
    _pf.STINT();
  }
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void m19::postfix_writer::do_evaluation_node(m19::evaluation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  basic_type *type = node->argument()->type();
  node->argument()->accept(this, lvl);
  _pf.TRASH(type->size());
}

void m19::postfix_writer::do_print_node(m19::print_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  basic_type *etype = node->argument()->type();
  node->argument()->accept(this, lvl); // expression to print
  if (etype->name() == basic_type::TYPE_INT) {
    os() << "        ;; int " << std::endl;
    _functions_to_declare.insert("printi");
    _pf.CALL("printi");
    _pf.TRASH(4); // trash int
  } else if (etype->name() == basic_type::TYPE_DOUBLE) {
    _functions_to_declare.insert("printd");
    _pf.CALL("printd");
    _pf.TRASH(8); // trash double
  } else if (etype->name() == basic_type::TYPE_STRING) {
    _functions_to_declare.insert("prints");
    _pf.CALL("prints");
    _pf.TRASH(4); // trash char pointer
  } else {
    std::cerr << "cannot print expression of unknown type" << std::endl;
    return;
  }

  if (node->newline()) {
    _functions_to_declare.insert("println");
    _pf.CALL("println");
  }
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
  ASSERT_SAFE_EXPRESSIONS;

  auto id = node->id();

  int offset = 0, typesize = node->type()->size();
  if (_inFunctionBody) {
    _offset -= typesize;
    offset = _offset;
  } else if (_inFunctionArgs) {
    offset = _offset;
    _offset += typesize;
  } else {
    offset = 0; // global variable
  }

  //Fetch symbol inserted by type checker
  std::shared_ptr<m19::symbol> symbol = new_symbol();
  if (symbol) {
    symbol->set_offset(offset);
    reset_new_symbol();
  }

  if (_inFunctionBody) {
    // if we are dealing with local variables, then no action is needed
    // unless an initializer exists
    if (node->expr()) {
      node->expr()->accept(this, lvl);
      if (node->type()->name() == basic_type::TYPE_INT || node->type()->name() == basic_type::TYPE_STRING
          || node->type()->name() == basic_type::TYPE_POINTER) {
        _pf.DUP32();
        _pf.LOCAL(symbol->offset());
        _pf.STINT();
      } else if (node->type()->name() == basic_type::TYPE_DOUBLE) {
        if(node->expr()->type()->name() == basic_type::TYPE_INT) {
          _pf.I2D();
        }
        _pf.DUP64();
        _pf.LOCAL(symbol->offset());
        _pf.STDOUBLE();
      } else {
        std::cerr << "cannot initialize" << std::endl;
      }
    }
    return;
  }

  if(_function)
    return;
  
  if (node->expr() == nullptr) {
    _pf.BSS();
    _pf.ALIGN();
    _pf.GLOBAL(id, _pf.OBJ());
    _pf.LABEL(id);
    _pf.SALLOC(typesize);
    return;
  }

  if (node->type()->name() == basic_type::TYPE_INT || node->type()->name() == basic_type::TYPE_DOUBLE
      || node->type()->name() == basic_type::TYPE_POINTER) {

    node->constant() ? _pf.RODATA() : _pf.DATA();
    _pf.ALIGN();
    _pf.GLOBAL(id, _pf.OBJ());
    _pf.LABEL(id);

    if (node->type()->name() == basic_type::TYPE_INT) {
      node->expr()->accept(this, lvl);
    } else if (node->type()->name() == basic_type::TYPE_POINTER) {
      node->expr()->accept(this, lvl);
    } else if (node->type()->name() == basic_type::TYPE_DOUBLE) {
      if (node->expr()->type()->name() == basic_type::TYPE_DOUBLE) {
        node->expr()->accept(this, lvl);
      } else if (node->expr()->type()->name() == basic_type::TYPE_INT) {
        cdk::integer_node *dclini = dynamic_cast<cdk::integer_node *>(node->expr());
        cdk::double_node ddi(dclini->lineno(), dclini->value());
        ddi.accept(this, lvl);
      } else {
        std::cerr << node->lineno() << ": '" << id << "' has bad initializer for real value\n";
        _errors = true;
      }
      
    }
  } else if (node->type()->name() == basic_type::TYPE_STRING) {
    //std::cout << "STRING" << std::endl;
    // if (node->constant()) {
      //std::cout << "CONSTANT" << std::endl;
      int litlbl;
      // HACK!!! string literal initializers must be emitted before the string identifier
      _pf.RODATA();
      _pf.ALIGN();
      _pf.LABEL(mklbl(litlbl = ++_lbl));
      _pf.SSTRING(dynamic_cast<cdk::string_node *>(node->expr())->value());
      _pf.ALIGN();
      _pf.LABEL(id);
      _pf.SADDR(mklbl(litlbl));
    // } else {
    //   std::cout << "NOT CONSTANT" << std::endl;
    //   _pf.DATA();
    //   _pf.ALIGN();
    //   _pf.LABEL(id);
    //   node->expr()->accept(this, lvl);
    // }
  } else {
    std::cerr << node->lineno() << ": '" << id << "' has unexpected initializer\n";
    _errors = true;
  }
}

void m19::postfix_writer::do_stack_alloc_node(m19::stack_alloc_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);
  _pf.INT(node->type()->subtype()->size());
  _pf.SHTL();
  _pf.ALLOC(); // allocate
  _pf.SP();// put base pointer in stack
}

void m19::postfix_writer::do_address_node(m19::address_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lval()->accept(this, lvl + 2);
}

void m19::postfix_writer::do_index_node(m19::index_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (node->expr()) {
    node->expr()->accept(this, lvl);
  } else {
    if (_function) {
      _pf.LOCV(-_function->type()->size());
    } else {
      std::cerr << "FATAL: " << node->lineno() << ": trying to use return value outside function" << std::endl;
    }
  }
  node->index()->accept(this, lvl);
  _pf.INT(3);
  _pf.SHTL();
  _pf.ADD(); // add pointer and index
}

/****************************************************************************************
 *****************************       ITERATION RELATED       ****************************
 *****************************        IF-ELSE RELATED       *****************************
 ****************************************************************************************/
void m19::postfix_writer::do_for_node(m19::for_node * const node, int lvl) {
  _forIni.push(++_lbl); // after init, before body
  _forStep.push(++_lbl);// after intruction
  _forEnd.push(++_lbl);// after for

  os() << "        ;; FOR initialize" << std::endl;
  node->init()->accept(this, lvl);

  os() << "        ;; FOR test" << std::endl;
  _pf.LABEL(mklbl(_forIni.top()));
  node->stop()->accept(this, lvl);
  _pf.JZ(mklbl(_forEnd.top()));
  os() << "        ;; FOR instruction" << std::endl;
  // execute instruction
  node->instruction()->accept(this, lvl + 2);

  os() << "        ;; FOR increment" << std::endl;
  _pf.LABEL(mklbl(_forStep.top()));
  node->step()->accept(this, lvl);
  os() << "        ;; FOR jump to test" << std::endl;
  _pf.JMP(mklbl(_forIni.top()));
  os() << "        ;; FOR end" << std::endl;
  _pf.LABEL(mklbl(_forEnd.top()));

  _forIni.pop();
  _forStep.pop();
  _forEnd.pop();
}

void m19::postfix_writer::do_if_node(m19::if_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1));
}

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

/****************************************************************************************
 *****************************       LOGICAL RELATED        *****************************
 ****************************************************************************************/
void m19::postfix_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() == basic_type::TYPE_INT && node->right()->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() == basic_type::TYPE_INT && node->right()->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.I2D();

  _pf.LT();
}
void m19::postfix_writer::do_le_node(cdk::le_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() == basic_type::TYPE_INT && node->right()->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() == basic_type::TYPE_INT && node->right()->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.I2D();

  _pf.LE();
}
void m19::postfix_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() == basic_type::TYPE_INT && node->right()->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() == basic_type::TYPE_INT && node->right()->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.I2D();

  _pf.GE();
}
void m19::postfix_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() == basic_type::TYPE_INT && node->right()->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() == basic_type::TYPE_INT && node->right()->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.I2D();

  _pf.GT();
}
void m19::postfix_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() == basic_type::TYPE_INT && node->right()->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() == basic_type::TYPE_INT && node->right()->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.I2D();

  _pf.NE();
}
void m19::postfix_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() == basic_type::TYPE_INT && node->right()->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() == basic_type::TYPE_INT && node->right()->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.I2D();

  _pf.EQ();
}

void m19::postfix_writer::do_not_node(cdk::not_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl + 2);
  _pf.INT(0);
  _pf.EQ();
  //_pf.NOT();
}
void m19::postfix_writer::do_and_node(cdk::and_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl = ++_lbl;
  node->left()->accept(this, lvl + 2);
  _pf.DUP32();
  _pf.JZ(mklbl(lbl));
  node->right()->accept(this, lvl + 2);
  _pf.AND();
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl));
}
void m19::postfix_writer::do_or_node(cdk::or_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl = ++_lbl;
  node->left()->accept(this, lvl + 2);
  _pf.DUP32();
  _pf.JNZ(mklbl(lbl));
  node->right()->accept(this, lvl + 2);
  _pf.OR();
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl));
}

/****************************************************************************************
 *****************************     ARITHMETIC RELATED       *****************************
 ****************************************************************************************/
void m19::postfix_writer::do_add_node(cdk::add_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);
  if (node->type()->name() == basic_type::TYPE_DOUBLE && node->left()->type()->name() == basic_type::TYPE_INT) {
    _pf.I2D();
  } else if (node->type()->name() == basic_type::TYPE_POINTER && node->left()->type()->name() == basic_type::TYPE_INT) {
    _pf.INT(3);
    _pf.SHTL();
  }

  node->right()->accept(this, lvl + 2);
  if (node->type()->name() == basic_type::TYPE_DOUBLE && node->right()->type()->name() == basic_type::TYPE_INT) {
    _pf.I2D();
  } else if (node->type()->name() == basic_type::TYPE_POINTER && node->right()->type()->name() == basic_type::TYPE_INT) {
    _pf.INT(3);
    _pf.SHTL();
  }

  node->type()->name() == basic_type::TYPE_DOUBLE ? _pf.DADD() : _pf.ADD();
}
void m19::postfix_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);
  if (node->type()->name() == basic_type::TYPE_DOUBLE && node->left()->type()->name() == basic_type::TYPE_INT)
  _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->type()->name() == basic_type::TYPE_DOUBLE && node->right()->type()->name() == basic_type::TYPE_INT) {
    _pf.I2D();
  } else if (node->type()->name() == basic_type::TYPE_POINTER && node->right()->type()->name() == basic_type::TYPE_INT) {
    _pf.INT(3);
    _pf.SHTL();
  }

  node->type()->name() == basic_type::TYPE_DOUBLE ? _pf.DSUB() : _pf.SUB();
}
void m19::postfix_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);
  if (node->type()->name() == basic_type::TYPE_DOUBLE && node->left()->type()->name() == basic_type::TYPE_INT)
  _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->type()->name() == basic_type::TYPE_DOUBLE && node->right()->type()->name() == basic_type::TYPE_INT)
  _pf.I2D();

  node->type()->name() == basic_type::TYPE_DOUBLE ? _pf.DMUL() : _pf.MUL();
}
void m19::postfix_writer::do_div_node(cdk::div_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);
  if (node->type()->name() == basic_type::TYPE_DOUBLE && node->left()->type()->name() == basic_type::TYPE_INT)
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->type()->name() == basic_type::TYPE_DOUBLE && node->right()->type()->name() == basic_type::TYPE_INT)
    _pf.I2D();

  node->type()->name() == basic_type::TYPE_DOUBLE ? _pf.DDIV() : _pf.DIV();
}
void m19::postfix_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  _pf.MOD();
}

void m19::postfix_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  _pf.NEG(); // 2-complement
}

void m19::postfix_writer::do_identity_node(m19::identity_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);
}

/****************************************************************************************
 *****************************        TYPES RELATED         *****************************
 ****************************************************************************************/
void m19::postfix_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  if (_inFunctionBody) {
    _pf.INT(node->value()); // integer literal is on the stack: push an integer
  } else {
    _pf.SINT(node->value()); // integer literal is on the DATA segment
  }
}

void m19::postfix_writer::do_double_node(cdk::double_node * const node, int lvl) {
  if (_inFunctionBody) {
    _pf.DOUBLE(node->value()); // load number to the stack
  } else {
    _pf.SDOUBLE(node->value());    // double is on the DATA segment
  }
}

void m19::postfix_writer::do_string_node(cdk::string_node * const node, int lvl) {
  int lbl1;

  /* generate the string */
  _pf.RODATA();
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl1 = ++_lbl));
  _pf.SSTRING(node->value());
  if(_function) {
    /* local variable */
    _pf.TEXT();
    _pf.ADDR(mklbl(lbl1));
  }
  else {
    /* global variable */
    _pf.TEXT(); 
    _pf.SADDR(mklbl(lbl1)); 
  }
}

/****************************************************************************************
 *****************************       FUNCTION RELATED       *****************************
 ****************************************************************************************/
void m19::postfix_writer::do_function_definition_node(m19::function_definition_node * const node, int lvl) {
  if (_inFunctionBody || _inFunctionArgs) {
    error(node->lineno(), "cannot define function in body or in arguments");
    return;
  }

  ASSERT_SAFE_EXPRESSIONS;

  bool _inMain = (node->id() == "m19");

  _function = new_symbol(); //gets symbol set by type checker
  _functions_to_declare.erase(_function->name());
  reset_new_symbol();

  _offset = 8; //FP IP
  _symtab.push();
  if(node->arguments()) {
    _inFunctionArgs = true;
    for(size_t ix = 0; ix < node->arguments()->size(); ix++) {
      cdk::basic_node * arg = node->arguments()->node(ix);
      if(arg == nullptr) break;
      arg->accept(this, 0);
    }
     _inFunctionArgs = false;
  }

  _pf.TEXT();
  _pf.ALIGN();
  if(_inMain) { //m19
    _pf.GLOBAL("_main", _pf.FUNC());
    _pf.LABEL("_main");
  } else if(node->scope() == tPUBLIC) {
    _pf.GLOBAL(_function->name(), _pf.FUNC());
    _pf.LABEL(_function->name());
  } else { //private function
    _pf.LABEL(_function->name());
  }
   
  frame_size_calculator lsc(_compiler, _symtab);
  node->accept(&lsc, lvl);
  _pf.ENTER(lsc.localsize());

  _inFunctionBody = true;

  _offset = -_function->type()->size(); //retval

  if(node->retval()) {
    node->retval()->accept(this, lvl);
    if(_function->type()->name() == basic_type::TYPE_INT || _function->type()->name() == basic_type::TYPE_POINTER || _function->type()->name() == basic_type::TYPE_STRING) {
      _pf.LOCAL(_offset);
      _pf.STINT();
    } else if(_function->type()->name() == basic_type::TYPE_DOUBLE) {
      _pf.I2D(); //Couldn't figure how to fetch retval type, this is unnecessary in some cases but harmless
      _pf.LOCAL(_offset);
      _pf.STDOUBLE();
    }
  }

  _function->set_offset(_offset);
    
  //sections
  _endSectionlbl = ++_lbl;
  _endBodylbl = ++_lbl;
  os() << "        ;; before body " << std::endl;
  if(node->init()) node->init()->accept(this, lvl + 4);
  if(node->section()) {
    for(size_t ix = 0; ix < node->section()->size(); ix++) {
      m19::section_node * sec = (m19::section_node *)node->section()->node(ix);
      if(sec == nullptr) break;
      sec->accept(this, lvl + 8);
    }
  }
  _pf.LABEL(mklbl(_endSectionlbl));
  if(node->end()) node->end()->accept(this, lvl + 4);
  os() << "        ;; after body " << std::endl;
  _pf.LABEL(mklbl(_endBodylbl));
  _inFunctionBody = false;

  if(node->init()) _symtab.pop();

  if(_function->type()->name() == basic_type::TYPE_INT || _function->type()->name() == basic_type::TYPE_POINTER || _function->type()->name() == basic_type::TYPE_STRING) {
    _pf.STFVAL32();
  } else if(_function->type()->name() == basic_type::TYPE_DOUBLE) {
    _pf.STFVAL64();
  }

  _pf.LEAVE();
  _pf.RET();

  
  _symtab.pop(); 

  _function = nullptr;
  //main function (m19) is being defined, functions to be declared are extern
  if(_inMain) 
    for(std::string s: _functions_to_declare)
      _pf.EXTERN(s);
}

void m19::postfix_writer::do_block_node(m19::block_node * const node, int lvl) {
  _symtab.push(); // for block-local vars
  if (node->declaration()) node->declaration()->accept(this, lvl + 2);
  if (node->instruction()) node->instruction()->accept(this, lvl + 2);
  if(!_InitSection) _symtab.pop();
}

void m19::postfix_writer::do_function_declaration_node(m19::function_declaration_node * const node, int lvl) {
  if (_inFunctionBody || _inFunctionArgs) {
    error(node->lineno(), "cannot declare function in body or in args");
    return;
  }

  ASSERT_SAFE_EXPRESSIONS;

  if (!new_symbol()) return;

  std::shared_ptr<m19::symbol> function = new_symbol();
  _functions_to_declare.insert(function->name());
  reset_new_symbol();
}

void m19::postfix_writer::do_function_call_node(m19::function_call_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  const std::string &id = node->id() == "@" ? _function->name() : node->id();
  std::shared_ptr<m19::symbol> symbol = _symtab.find(id);

  os() << "        ;; function call node " << std::endl;
  size_t argsSize = 0;
  if (node->arguments()) {
    for (int ax = node->arguments()->size(); ax > 0; ax--) {
      cdk::expression_node *arg = dynamic_cast<cdk::expression_node*>(node->arguments()->node(ax - 1));
      arg->accept(this, lvl + 2);
      if(arg->type()->name() == basic_type::TYPE_INT && symbol->args().at(ax-1)->name() == basic_type::TYPE_DOUBLE)
        _pf.I2D();
      //do_int2double(symbol->args().at(ax - 1), arg->type());
      argsSize += symbol->args().at(ax - 1)->size();
    }
  }
  _pf.CALL(id);
  if (argsSize != 0) {
    _pf.TRASH(argsSize);
  }

  basic_type *type = symbol->type();
  if (type->name() == basic_type::TYPE_INT || type->name() == basic_type::TYPE_POINTER || type->name() == basic_type::TYPE_STRING) {
    _pf.LDFVAL32();
  }
  else if (type->name() == basic_type::TYPE_DOUBLE) {
    _pf.LDFVAL64();
  }
  else if(type->name() == basic_type::TYPE_VOID) {
    //NOTHING
  } else {
     error(node->lineno(), "unexpected error in function call");
  }
}

/****************************************************************************************
 *****************************       SECTIONS RELATED       *****************************
 ****************************************************************************************/
void m19::postfix_writer::do_section_node(m19::section_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if(node->qualifier() == tINCLUSIVE) {
    os() << "        ;; section inclusive" << std::endl;
    int lbl2;
    node->expr() ? node->expr()->accept(this, lvl + 2) : _pf.INT(1);
    _pf.JZ(mklbl(lbl2 = ++_lbl));
    node->block()->accept(this, lvl + 2);
    _pf.LABEL(mklbl(lbl2));
  } else {
    os() << "        ;; section exclusive " << std::endl;
    int lbl1;
    node->expr() ? node->expr()->accept(this, lvl + 2) : _pf.INT(1);
    _pf.JZ(mklbl(lbl1 = ++_lbl));
    node->block()->accept(this, lvl + 2);
    _pf.JMP(mklbl(_endSectionlbl));
    _pf.LABEL(mklbl(lbl1));
    
  }
}

void m19::postfix_writer::do_section_end_node(m19::section_end_node * const node, int lvl) {
  _inFinalSection = true;
  node->block()->accept(this, lvl + 2);
  _inFinalSection = false;
}

void m19::postfix_writer::do_section_init_node(m19::section_init_node * const node, int lvl) {
  _InitSection = true;
  node->block()->accept(this, lvl + 2);
  _InitSection = false;
}

/****************************************************************************************
 *****************************    CONTINUE, RETURN, STOP    *****************************
 ****************************************************************************************/
void m19::postfix_writer::do_continue_node(m19::continue_node * const node, int lvl) {
   if (_forIni.size() != 0) {
    _pf.JMP(mklbl(_forStep.top())); // jump to next cycle
  } else
    error(node->lineno(), "'restart' outside 'for'");
}

void m19::postfix_writer::do_return_node(m19::return_node * const node, int lvl) {
  if(_inFinalSection) {
    _pf.JMP(mklbl(_endBodylbl));
  } else {
    _pf.JMP(mklbl(_endSectionlbl));
  }
}

void m19::postfix_writer::do_stop_node(m19::stop_node * const node, int lvl) {
  if (_forIni.size() != 0) {
    _pf.JMP(mklbl(_forEnd.top())); // jump to for end
  } else
    error(node->lineno(), "'break' outside 'for'");
}