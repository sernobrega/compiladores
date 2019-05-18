#include <string>
#include "targets/frame_size_calculator.h"
#include "targets/symbol.h"
#include "ast/all.h"

m19::frame_size_calculator::~frame_size_calculator() {
  os().flush();
}

//---------------------------------------------------------------------------

void m19::frame_size_calculator::do_for_node(m19::for_node * const node, int lvl) {
  // EMPTY
}


void m19::frame_size_calculator::do_if_node(m19::if_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_if_else_node(m19::if_else_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_variable_declaration_node(m19::variable_declaration_node * const node, int lvl) {
  //
}

void m19::frame_size_calculator::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_section_node(m19::section_node * const node, int lvl) {
  //
}

void m19::frame_size_calculator::do_section_end_node(m19::section_end_node * const node, int lvl) {
  //
}

void m19::frame_size_calculator::do_section_init_node(m19::section_init_node * const node, int lvl) {
  //
}


void m19::frame_size_calculator::do_function_definition_node(m19::function_definition_node * const node, int lvl) {
    _localsize += node->type()->size(); // save space for the function's return type
    if(node->init()) node->init()->accept(this, lvl + 2);
    if(node->section()) {
        for(size_t ix = 0; ix < node->section()->size(); ix++) {
        m19::section_node * sec = (m19::section_node *)node->section()->node(ix);
        if(sec == nullptr) break;
        sec->accept(this, lvl + 4);
        }
    }
    if(node->end()) node->end()->accept(this, lvl + 2);

}