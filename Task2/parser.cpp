#include "parser.h"

node_ptr parser::parse(const string& s) {
    lex = lexical_analyzer(s + '$');
    return parse_descr_list();
};

node_ptr parser::parse_descr_list() {
    node_ptr node = new Node("DESCR_LIST");
    while (lex.get_cur_token() != END) {
        switch (lex.get_cur_token()) {
            case NAME:
                node->children.push_back(parse_descr());
                break;
            default:
                throw parser_exception("Wrong first of description", lex.get_cur_pos());
        }
    }
    if (node->children.empty()) {
        throw parser_exception("Empty descriptions list");
    }
    return node;
}

node_ptr parser::parse_descr() {
    node_ptr node = new Node("DESCR");
    node->children.push_back(parse_name(true));
    node->children.push_back(parse_var_list());
    node->children.push_back(new Node(";"));
    lex.next_pos();
    return node;
}

node_ptr parser::parse_var_list() {
    node_ptr node = new Node("VAR_LIST");
    while (lex.get_cur_token() != SEMICOLON) {
        switch(lex.get_cur_token()) {
            case STAR:
            case NAME:
                node->children.push_back(parse_var());
                break;
            default:
                throw parser_exception("Wrong first of variables list", lex.get_cur_pos());
        }
        if (lex.get_cur_token() != SEMICOLON) {
            node->children.push_back(new Node(","));
            lex.next_pos();
        }
    }
    if (node->children.empty()) {
        throw parser_exception("Empty variables list", lex.get_cur_pos());
    }
    return node;
}

node_ptr parser::parse_var() {
    node_ptr node = new Node("VAR");
    while (lex.get_cur_token() != COMMA && lex.get_cur_token() != SEMICOLON) {
        switch(lex.get_cur_token()) {
            case STAR:
                node->children.push_back(new Node("*"));
                lex.next_pos();
                node->children.push_back(parse_var());
                break;
            case NAME:
                node->children.push_back(parse_name(false));
                break;
            default:
                throw parser_exception("Wrong first of variable", lex.get_cur_pos());
        }
    }
    if (node->children.empty()) {
        throw parser_exception("Empty variable", lex.get_cur_pos());
    }
    return node;
}

node_ptr parser::parse_name(bool is_type) {
    string name;
    while (lex.cur_is_letter()) {
        name += lex.get_cur_char();
        lex.next_pos();
    }
    if (name.empty()) {
        throw parser_exception("Empty name", lex.get_cur_pos());
    }
    if (is_type) {
        if (lex.get_cur_token() != STAR && lex.get_cur_token() != NAME) {
            throw parser_exception("Wrong follow of type name", lex.get_cur_pos());
        }
        name += ' ';
    } else {
        if (lex.get_cur_token() != COMMA && lex.get_cur_token() != SEMICOLON) {
            throw parser_exception("Wrong follow of variable name", lex.get_cur_pos());
        }
    }
    return new Node(name);
}