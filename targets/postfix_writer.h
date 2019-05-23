#ifndef __M19_SEMANTICS_POSTFIX_WRITER_H__
#define __M19_SEMANTICS_POSTFIX_WRITER_H__

#include <set>
#include <string>
#include <iostream>
#include <stack>
#include <cdk/symbol_table.h>
#include <cdk/emitters/basic_postfix_emitter.h>
#include "targets/basic_ast_visitor.h"
#include "targets/symbol.h"

namespace m19 {

  //!
  //! Traverse syntax tree and generate the corresponding assembly code.
  //!
  class postfix_writer: public basic_ast_visitor {
    cdk::symbol_table<m19::symbol> &_symtab;
    cdk::basic_postfix_emitter &_pf;
    int _lbl;
    bool _inForInit;
    bool _errors, _inFunction, _inFunctionName, _inFunctionArgs, _inFunctionBody;
    bool _inMain, _inFinalSection, _InitSection;
    std::stack<int> _forIni, _forStep, _forEnd; // for break/repeat
    std::set<std::string> _functions_to_declare;
    std::shared_ptr<m19::symbol> _function;
    int _endBodylbl, _endSectionlbl;


    int _offset;
  
  public:
    postfix_writer(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<m19::symbol> &symtab,
                   cdk::basic_postfix_emitter &pf) :
        basic_ast_visitor(compiler), _symtab(symtab), _pf(pf), _lbl(0), _errors(false), _inFunction(false), 
        _inFunctionName(false), _inFunctionArgs(false), _inFunctionBody(false), _inMain(false), _inFinalSection() {
    }

  public:
    ~postfix_writer() {
      os().flush();
    }

  private:
    /** Method used to generate sequential labels. */
    inline std::string mklbl(int lbl) {
      std::ostringstream oss;
      if (lbl < 0)
        oss << ".L" << -lbl;
      else
        oss << "_L" << lbl;
      return oss.str();
    }

    void error(int lineno, std::string s) {
      std::cerr << "error: " << lineno << ": " << s << std::endl;
    }

    void do_int2double(basic_type * left, basic_type * right) {
      if(left->name() == basic_type::TYPE_DOUBLE && right->name() == basic_type::TYPE_INT) {
          _pf.I2D();
        printf("hey\n");
      }
      
    }

  public:
  // do not edit these lines
#define __IN_VISITOR_HEADER__
#include "ast/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
  // do not edit these lines: end

  };

} // m19

#endif
