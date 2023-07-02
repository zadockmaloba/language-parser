#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#define __NOT_STRING_OR_COMMENT__                                              \
  current_token.type() !=                                                      \
      Token::TokenType::STRING_LITERAL &&current_token.type() !=               \
      Token::TokenType::COMMENT

#define __NUMERIC_OR_WHITESPACE__                                              \
  current_token.type() == Token::TokenType::NUMERIC_LITERAL ||                 \
      current_token.type() == Token::TokenType::WHITE_SPACE

#define NOT_DELIMETER(x, y)                                                    \
  !(x->type() == Token::TokenType::PUNCTUATOR && x->const_data() == y)

#define MAP_HAS(x, y) (x.find(y) != x.end())

#define DEFAULT_NODE_CONSTRUCTOR(x)                                            \
  x() { setPreferredType(this->type()); }

#define DEBUG_ITERATOR(x)                                                      \
  std::cout << "{{ITERATOR}}: " << x->const_data() << "\n";

#define PRINT_ITERATOR_ARRAY(x)                                                \
  std::cout << ">>> ";                                                         \
  for (auto &v : x) {                                                          \
    std::cout << v.const_data();                                               \
  }                                                                            \
  std::cout << std::endl;

#define MAKE_UNIQUE_NODE_PTR(x) std::make_unique<ServerLang::ASTNode>(x);

#define GET_SET(x, _type, _virt)                                               \
  _virt _type x() const { return m_##x; }                                      \
  _virt void set##x(_type newValue) { m_##x = newValue; }

namespace ServerLang {

// Forward:
class ASTNode;

// Typedefs:
using node_ptr = std::unique_ptr<ASTNode>;
using node_list = std::vector<node_ptr>;

enum class Type {
  UNDEFINED,
  I16,
  I32,
  I64,
  U8,
  U16,
  F32,
  F64,
  BOOL,
  COMPLEX,
  STRING,
  VARIANT,
  VOID,
  ARRAY,
  CLASS,
  STRUCT,
  JSON,
  ROUTE,
  SCOPE,
  OBJECT,
  FUNCTION,
  PRIMITIVE,
  EXPRESSION,
  ARITHMETICEXPRESSION,
  LOGICALEXPRESSION,
  ASSIGNMENTEXPRESSION,
  CALLEXPRESSION,
  ACCESSEXPRESSION,
};

enum class Operators {
  ADD,
  SUB,
  MUL,
  DIV,
  INCR,
  DCR,
  AND,
  OR,
  XOR,
  NAND,
  NOR,
  ASGN,
  CALL,
  ACC
};

class ASTNode {

public: // virtual methods
  ASTNode() : m_id("__NO_ID__") {}
  virtual ~ASTNode() {}
  virtual ServerLang::Type type() const { return Type::UNDEFINED; };
  virtual const char *type_string() const { return "Undefined"; };

public:
  Type preferredType() const { return m_preferredType; }
  void setPreferredType(const Type newType) { m_preferredType = newType; }

  const char *id() const { return m_id.c_str(); }
  void setId(const char *newId) { m_id = newId; }

  GET_SET(parent, ASTNode *, virtual)

  node_list &children() { return m_children; }
  const node_list &children_const() const { return m_children; }

private:
  ASTNode *m_parent;
  Type m_preferredType = type();
  std::string m_id;
  node_list m_children;
};

class Scope : public ASTNode {
public:
  DEFAULT_NODE_CONSTRUCTOR(Scope)
  ServerLang::Type type() const override { return Type::SCOPE; }
  const char *type_string() const override { return "Scope"; }

public:
  // node_list &children() { return m_children; }
  // const node_list &children_const() const { return m_children; }

public:
  virtual bool executable() const { return m_executable; };
  virtual void setExecutable(bool newVal) { m_executable = newVal; };

private:
  // node_list m_children;
  bool m_executable = false;
};

class Object : public Scope {
public:
  DEFAULT_NODE_CONSTRUCTOR(Object)
  ServerLang::Type type() const override { return Type::OBJECT; }
  const char *type_string() const override { return "Object"; }
};

template <typename T> class Function : public Scope {
public:
  DEFAULT_NODE_CONSTRUCTOR(Function)
  ServerLang::Type type() const override { return Type::FUNCTION; }
  const char *type_string() const override { return "Function"; }

public:
  T value() const { return m_value; }
  void setValue(const T &newValue) { m_value = newValue; }
  Type return_t() const { return m_return_t; }
  void setReturn_t(const Type newValue) { m_return_t = newValue; }

private:
  T m_value;
  Type m_return_t = Type::VOID;
  node_list m_parameters;
};

template <typename T> class Primitive : public ASTNode {
public:
  DEFAULT_NODE_CONSTRUCTOR(Primitive)
  ServerLang::Type type() const override { return Type::PRIMITIVE; }
  const char *type_string() const override { return "Primitive"; }

  T value() const { return m_value; }
  void setValue(const T &newValue) { m_value = newValue; }

private:
  T m_value;
};

class Expression : public ASTNode {
public:
  DEFAULT_NODE_CONSTRUCTOR(Expression)
  ServerLang::Type type() const override { return Type::EXPRESSION; }
  const char *type_string() const override { return "Expression"; }

public: // virtual methods
  GET_SET(opr, Operators, virtual)
  GET_SET(lhs, ASTNode *, virtual)
  GET_SET(rhs, ASTNode *, virtual)
  virtual node_ptr exec() = 0;

private:
  ASTNode *m_lhs, *m_rhs;
  Operators m_opr;
};

namespace Expressions {
class AccessExpression : public Expression {
public:
  AccessExpression(ASTNode *_lhs, ASTNode *_rhs, const Operators _op) {
    this->setlhs(_lhs);
    this->setrhs(_rhs);
    this->setopr(_op);
  };
  ServerLang::Type type() const override { return Type::ACCESSEXPRESSION; }
  const char *type_string() const override { return "AccessExpression"; }
  node_ptr exec() override { return {}; }
};
class ArithmeticExpression : public Expression {
public:
  ArithmeticExpression(ASTNode *_lhs, ASTNode *_rhs, const Operators _op) {
    this->setlhs(_lhs);
    this->setrhs(_rhs);
    this->setopr(_op);
  };
  ServerLang::Type type() const override { return Type::ARITHMETICEXPRESSION; }
  const char *type_string() const override { return "ArithmeticExpression"; }
  node_ptr exec() override { return {}; }
};
class AssignmentExpression : public Expression {
public:
  AssignmentExpression(ASTNode *_lhs, ASTNode *_rhs, const Operators _op) {
    this->setlhs(_lhs);
    this->setrhs(_rhs);
    this->setopr(_op);
  };
  ServerLang::Type type() const override { return Type::ASSIGNMENTEXPRESSION; }
  const char *type_string() const override { return "AssignmentExpression"; }
  node_ptr exec() override { return {}; }
};
class CallExpression : public Expression {
public:
  CallExpression(ASTNode *_lhs, ASTNode *_rhs, const Operators _op) {
    this->setlhs(_lhs);
    this->setrhs(_rhs);
    this->setopr(_op);
  };
  ServerLang::Type type() const override { return Type::CALLEXPRESSION; }
  const char *type_string() const override { return "CallExpression"; }
  node_ptr exec() override { return {}; }
};
class LogicalExpression : public Expression {
public:
  LogicalExpression(ASTNode *_lhs, ASTNode *_rhs, const Operators _op) {
    this->setlhs(_lhs);
    this->setrhs(_rhs);
    this->setopr(_op);
  };
  ServerLang::Type type() const override { return Type::LOGICALEXPRESSION; }
  const char *type_string() const override { return "LogicalExpression"; }
  node_ptr exec() override { return {}; }
};
} // namespace Expressions

struct cmp_str {
  bool operator()(char const *a, char const *b) const {
    auto const comp_res = std::strcmp(a, b);
    return comp_res < 0;
  }
};

const std::map<const char *, bool, cmp_str> keywords = {
    {"var", 1}, {"const", 1}, {"def", 1}, {"return", 1},
    {"for", 1}, {"while", 1}, {"if", 1},
};

const std::map<const char *, Type, cmp_str> type_map = {
    {"I16", Type::I16},         {"I32", Type::I32},
    {"I64", Type::I64},         {"U8", Type::U8},
    {"U16", Type::U16},         {"F32", Type::F32},
    {"F64", Type::F64},         {"Bool", Type::BOOL},
    {"Complex", Type::COMPLEX}, {"String", Type::STRING},
    {"Variant", Type::VARIANT}, {"Void", Type::VOID},
    {"Array", Type::ARRAY},     {"Class", Type::CLASS},
    {"Struct", Type::STRUCT},   {"Json", Type::JSON},
    {"Route", Type::ROUTE},
};

namespace InternalTypes {

class I16 : public Primitive<uint16_t> {
public:
  DEFAULT_NODE_CONSTRUCTOR(I16)
  ServerLang::Type type() const override { return Type::I16; }
  const char *type_string() const override { return "Integer_16"; }
};

class I32 : public Primitive<int32_t> {
public:
  DEFAULT_NODE_CONSTRUCTOR(I32)
  ServerLang::Type type() const override { return Type::I32; }
  const char *type_string() const override { return "Integer_32"; }
};

class I64 : public Primitive<int64_t> {
public:
  DEFAULT_NODE_CONSTRUCTOR(I64)
  ServerLang::Type type() const override { return Type::I64; }
  const char *type_string() const override { return "Integer_64"; }
};

class U8 : public Primitive<u_int8_t> {
public:
  DEFAULT_NODE_CONSTRUCTOR(U8)
  ServerLang::Type type() const override { return Type::U8; }
  const char *type_string() const override { return "Unsigned_8"; }
};

class U16 : public Primitive<u_int16_t> {
public:
  DEFAULT_NODE_CONSTRUCTOR(U16)
  ServerLang::Type type() const override { return Type::U16; }
  const char *type_string() const override { return "Unsigned_16"; }
};

class F32 : public Primitive<float> {
public:
  DEFAULT_NODE_CONSTRUCTOR(F32)
  ServerLang::Type type() const override { return Type::F32; }
  const char *type_string() const override { return "Float_32"; }
};

class F64 : public Primitive<double> {
public:
  DEFAULT_NODE_CONSTRUCTOR(F64)
  ServerLang::Type type() const override { return Type::F64; }
  const char *type_string() const override { return "Float_64"; }
};

class Bool : public Primitive<bool> {
public:
  DEFAULT_NODE_CONSTRUCTOR(Bool)
  ServerLang::Type type() const override { return Type::BOOL; }
  const char *type_string() const override { return "Boolean"; }
};

class Complex : public Primitive<size_t> {
public:
  DEFAULT_NODE_CONSTRUCTOR(Complex)
  ServerLang::Type type() const override { return Type::COMPLEX; }
  const char *type_string() const override { return "Complex"; }
};

class String : public Primitive<std::string> {
public:
  DEFAULT_NODE_CONSTRUCTOR(String)
  ServerLang::Type type() const override { return Type::STRING; }
  const char *type_string() const override { return "String"; }
};

class Variant : public Primitive<std::string> {
public:
  DEFAULT_NODE_CONSTRUCTOR(Variant)
  ServerLang::Type type() const override { return Type::VARIANT; }
  const char *type_string() const override { return "Variant"; }
};

class Void : public Primitive<bool> {
public:
  DEFAULT_NODE_CONSTRUCTOR(Void)
  ServerLang::Type type() const override { return Type::VOID; }
  const char *type_string() const override { return "Void"; }
};

} // namespace InternalTypes

namespace CompoundTypes {

class Array : public Object {
public:
  DEFAULT_NODE_CONSTRUCTOR(Array)
  ServerLang::Type type() const override { return Type::ARRAY; }
  const char *type_string() const override { return "Array"; }
};

class Class : public Object {
public:
  DEFAULT_NODE_CONSTRUCTOR(Class)
  ServerLang::Type type() const override { return Type::CLASS; }
  const char *type_string() const override { return "Class"; }
};

class Struct : public Object {
public:
  DEFAULT_NODE_CONSTRUCTOR(Struct)
  ServerLang::Type type() const override { return Type::STRUCT; }
  const char *type_string() const override { return "Struct"; }
};

class Json : public Object {
public:
  DEFAULT_NODE_CONSTRUCTOR(Json)
  ServerLang::Type type() const override { return Type::JSON; }
  const char *type_string() const override { return "Json"; }
};

class Route : public Object {
public:
  DEFAULT_NODE_CONSTRUCTOR(Route)
  ServerLang::Type type() const override { return Type::ROUTE; }
  const char *type_string() const override { return "Route"; }
};

} // namespace CompoundTypes

static ASTNode *get_type_instance(const Type _t) {
  switch (_t) {
  case ServerLang::Type::I16:
    return new InternalTypes::I16{};
  case ServerLang::Type::I32:
    return new InternalTypes::I32{};
  case ServerLang::Type::I64:
    return new InternalTypes::I64{};
  case ServerLang::Type::U8:
    return new InternalTypes::U8{};
  case ServerLang::Type::U16:
    return new InternalTypes::U16{};
  case ServerLang::Type::F32:
    return new InternalTypes::F32{};
  case ServerLang::Type::F64:
    return new InternalTypes::F64{};
  case ServerLang::Type::BOOL:
    return new InternalTypes::Bool{};
  case ServerLang::Type::COMPLEX:
    return new InternalTypes::Complex{};
  case ServerLang::Type::STRING:
    return new InternalTypes::String{};
  case ServerLang::Type::VARIANT:
    return new InternalTypes::Variant{};
  case ServerLang::Type::VOID:
    return new InternalTypes::Void{};
  case ServerLang::Type::ARRAY:
    return new CompoundTypes::Array{};
  case ServerLang::Type::CLASS:
    return new CompoundTypes::Class{};
  case ServerLang::Type::STRUCT:
    return new CompoundTypes::Struct{};
  case ServerLang::Type::JSON:
    return new CompoundTypes::Json{};
  case ServerLang::Type::ROUTE:
    return new CompoundTypes::Route{};
  default:
    return {};
    break;
  }
}

static ASTNode *get_type_instance(const char *_t_string) {
  auto _code = ServerLang::type_map.find(_t_string)->second;
  return get_type_instance(_code);
}

} // namespace ServerLang

class Token {
public:
  enum class TokenType {
    GARBAGE_TYPE,
    WHITE_SPACE,
    COMMENT,
    KEYWORD,
    IDENTIFIER,
    NUMERIC_LITERAL,
    STRING_LITERAL,
    ACCESS_OPERATOR,
    ARITHMETIC_OPERATOR,
    LOGIC_OPERATOR,
    POINTER_OPERATOR,
    PUNCTUATOR
  };
  const static std::map<const TokenType, const char *> TokenNames;

public:
  Token() = default;
  ~Token() {}

public: // Get, Set
  TokenType type() const { return m_type; }
  void setType(const TokenType newType) { m_type = newType; }

  const std::string &const_data() const { return m_data; }
  std::string &data() { return m_data; }
  void setData(const std::string &newData) { m_data = newData; }

private:
  TokenType m_type = TokenType::WHITE_SPACE;
  std::string m_data;
};

const std::map<const Token::TokenType, const char *> Token::TokenNames = {
    {Token::TokenType::WHITE_SPACE, "WhiteSpace"},
    {Token::TokenType::COMMENT, "Comment"},
    {Token::TokenType::KEYWORD, "KeyWord"},
    {Token::TokenType::IDENTIFIER, "Identifier"},
    {Token::TokenType::NUMERIC_LITERAL, "NumericLiteral"},
    {Token::TokenType::STRING_LITERAL, "StringLiteral"},
    {Token::TokenType::ACCESS_OPERATOR, "AccessOperator"},
    {Token::TokenType::ARITHMETIC_OPERATOR, "ArithmeticOperator"},
    {Token::TokenType::LOGIC_OPERATOR, "LogicOperator"},
    {Token::TokenType::POINTER_OPERATOR, "PointerOperator"},
    {Token::TokenType::PUNCTUATOR, "Punctuator"},
};

class Tokenizer {
public: // typedefs
  using token_list = std::vector<Token>;

public:
  Tokenizer() = default;
  ~Tokenizer() {}

public: // static methods
  static const token_list evaluate(const std::string_view &source) {
    token_list list;
    Token current_token;
    for (auto const &v : source) {
      switch (v) {
      case 48 ... 57: // 0-9
        if (current_token.type() == Token::TokenType::WHITE_SPACE) {
          current_token.setType(Token::TokenType::NUMERIC_LITERAL);
          current_token.data().append(1, v);
        } else
          current_token.data().append(1, v);
        break;

      case '_':
      case 65 ... 90:  // A-Z
      case 97 ... 122: // a-z
        if (__NOT_STRING_OR_COMMENT__) {
          // if (current_token.type() != Token::TokenType::IDENTIFIER) {
          //   end_token(current_token, list);
          //   current_token.setType(Token::TokenType::IDENTIFIER);
          // }
          current_token.setType(Token::TokenType::IDENTIFIER);
          current_token.data().append(1, v);
        } else
          current_token.data().append(1, v);
        break;
      case '{':
      case '}':
      case '(':
      case ')':
      case ';':
      case ',':
      case '@':
        if (__NOT_STRING_OR_COMMENT__) {
          end_token(current_token, list);
          current_token.setType(Token::TokenType::PUNCTUATOR);
          current_token.data().append(1, v);
          end_token(current_token, list);
        } else
          current_token.data().append(1, v);
        break;
      case ':':
        if (current_token.type() == Token::TokenType::PUNCTUATOR &&
            current_token.const_data() == ":") {
          current_token.setType(Token::TokenType::ACCESS_OPERATOR);
          current_token.data().append(1, v);
          end_token(current_token, list);
        } else if (__NOT_STRING_OR_COMMENT__) {
          end_token(current_token, list);
          current_token.setType(Token::TokenType::PUNCTUATOR);
          current_token.data().append(1, v);
          // end_token(current_token, list);
        } else
          current_token.data().append(1, v);
        break;
      case '$':
        if (__NOT_STRING_OR_COMMENT__) {
          current_token.setType(Token::TokenType::POINTER_OPERATOR);
          current_token.data().append(1, v);
          end_token(current_token, list);
        } else {
          current_token.data().append(1, v);
        }
        break;
      case '.':
        if (current_token.type() == Token::TokenType::NUMERIC_LITERAL)
          current_token.data().append(1, v);
        else if (__NOT_STRING_OR_COMMENT__) {
          current_token.setType(Token::TokenType::ACCESS_OPERATOR);
          current_token.data().append(1, v);
          end_token(current_token, list);
        } else {
          current_token.data().append(1, v);
        }
        break;
      case '!':
      case '&':
      case '|':
      case '>':
      case '<':
        if (__NOT_STRING_OR_COMMENT__) {
          current_token.setType(Token::TokenType::LOGIC_OPERATOR);
          current_token.data().append(1, v);
        } else
          current_token.data().append(1, v);
        break;
      case '*':
        if (__NOT_STRING_OR_COMMENT__ && current_token.const_data() == "/") {
          current_token.setType(Token::TokenType::IDENTIFIER);
          current_token.data().append(1, v);
        } else if (__NOT_STRING_OR_COMMENT__) {
          current_token.setType(Token::TokenType::ARITHMETIC_OPERATOR);
          current_token.data().append(1, v);
        } else
          current_token.data().append(1, v);
        break;
      case '/':
        if (current_token.const_data() == "/")
          current_token.setType(Token::TokenType::COMMENT);
      case '+':
      case '-':
      case '^':
      case '=':
        if (__NOT_STRING_OR_COMMENT__) {
          current_token.setType(Token::TokenType::ARITHMETIC_OPERATOR);
          current_token.data().append(1, v);
        } else
          current_token.data().append(1, v);
        break;
      case '\r':
      case '\n':
        end_token(current_token, list);
        break;
      case '\t':
      case ' ':
        if (current_token.type() == Token::TokenType::STRING_LITERAL ||
            current_token.type() == Token::TokenType::COMMENT)
          current_token.data().append(1, v);
        else
          end_token(current_token, list);
        break;
      case '\"':
        if (__NOT_STRING_OR_COMMENT__) {
          end_token(current_token, list);
          current_token.setType(Token::TokenType::STRING_LITERAL);
        } else if (current_token.type() == Token::TokenType::STRING_LITERAL) {
          end_token(current_token, list);
        }
        break;
      default:
        if (current_token.type() == Token::TokenType::STRING_LITERAL ||
            current_token.type() == Token::TokenType::COMMENT)
          current_token.data().append(1, v);

        break;
      }
    }
    return list;
  }

  static void end_token(Token &_token, token_list &_list) {
    if (_token.type() != Token::TokenType::WHITE_SPACE)
      _list.push_back(_token);
    _token.setType(Token::TokenType::WHITE_SPACE);
    _token.setData("");
  }

  static token_list get_span(token_list::const_iterator &it, const char *delim,
                             Token::TokenType type) {
    token_list ret;
    while (!(it->const_data() == delim && it->type() == type)) {
      ret.push_back(*it);
      it++;
    }
    return ret;
  }

  static const Token peek(token_list::const_iterator it) { return *it++; }
};

class SyntaxAnalyzer {
  enum class State {
    NO_OP,
    VARIABLE_DECL,
    CONST_DECL,
    FUNCTION_DECL,
    EXPRESSION
  };

  using node = ServerLang::node_ptr;
  using node_list = ServerLang::node_list;
  using token_list = Tokenizer::token_list;
  using object = ServerLang::Object;
  using scope = ServerLang::Scope;

public:
  SyntaxAnalyzer() = default;
  ~SyntaxAnalyzer() {}

public:
  const ServerLang::node_list analyze(const Tokenizer::token_list &tokens) {
    ServerLang::node_list ret;
    // State _i_state{State::NO_OP};
    auto itr = tokens.cbegin();
    while (itr != tokens.cend()) {
      switch (m_state) {
      case State::NO_OP:
        check_for_next_possible(itr);
        // itr++;
        break;
      case State::CONST_DECL:
        std::cout << "[CONST DECL]::begin => " << itr->const_data()
                  << std::endl;
        itr++;
        ret.push_back(check_for_const_decl(itr));
        // itr++;
        break;
      case State::VARIABLE_DECL:
        std::cout << "[VAR DECL]::begin => " << itr->const_data() << std::endl;
        itr++;
        ret.push_back(check_for_variable_decl(itr));
        // itr++;
        break;
      case State::FUNCTION_DECL:
        std::cout << "[FUNCTION DECL]::begin => " << itr->const_data()
                  << std::endl;
        itr++;
        ret.push_back(check_for_fn_decl(itr));
        // itr++;
        break;
      case State::EXPRESSION:
        std::cout << "[EXPRESSION]::begin => " << itr->const_data()
                  << std::endl;
        // itr++;
        ret.push_back(check_for_expression(itr));
        itr++;
        break;
      default:
        break;
      }
    }
    return ret;
  }

private:
  State m_state = {State::NO_OP};

private: // helpers
         // TODO: Prevent infinite loop on syntax error
  void move_to_next_end(Tokenizer::token_list::const_iterator &it,
                        const std::string_view &delim = ";") {
    while (NOT_DELIMETER(it, delim)) {
      DEBUG_ITERATOR(it)
      it++;
    }
    it++;
  }

  void check_for_next_possible(Tokenizer::token_list::const_iterator &it) {
    switch (it->type()) {
    case Token::TokenType::COMMENT:
      m_state = State::NO_OP;
      it++;
      break;
    case Token::TokenType::IDENTIFIER:
      if (ServerLang::keywords.find(it->const_data().c_str()) ==
          ServerLang::keywords.end()) {
        m_state = State::EXPRESSION;
      } else if (it->const_data() == "const") {
        m_state = State::CONST_DECL;
      } else if (it->const_data() == "var") {
        m_state = State::VARIABLE_DECL;
      } else if (it->const_data() == "def") {
        m_state = State::FUNCTION_DECL;
      }
      // it++;
      break;

    default:
      m_state = State::NO_OP;
      move_to_next_end(it);
      break;
    }
  }
  void check_for_library_imports(Tokenizer::token_list::const_iterator &it) {
    // TODO
  }
  void check_for_script_imports(Tokenizer::token_list::const_iterator &it) {
    // TODO
  }
  node check_for_expression(Tokenizer::token_list::const_iterator &it) {
    auto const _tmp =
        Tokenizer::get_span(it, ";", Token::TokenType::PUNCTUATOR);

    node _ret;

    for (auto &j : _tmp) {
      if (j.const_data() == "(" && j.type() == Token::TokenType::PUNCTUATOR) {
        auto _exp = new ServerLang::Expressions::CallExpression{
            0, 0, ServerLang::Operators::CALL};

        _ret.reset(_exp);
        break;
      }

      else if (j.const_data() == "=" &&
               j.type() == Token::TokenType::PUNCTUATOR) {
        auto _exp = new ServerLang::Expressions::AssignmentExpression{
            0, 0, ServerLang::Operators::ASGN};

        _ret.reset(_exp);
        break;
      }
    }

    // PRINT_ITERATOR_ARRAY(_tmp);
    m_state = State::NO_OP;
    return _ret;
  }

  token_list
  get_compound_stmnt_nodes(Tokenizer::token_list::const_iterator &it) {
    token_list ret;
    int brace_count = 1;
    while (brace_count != 0) {
      std::cout << "Cmpnd_Sttmnt::Before: ";
      DEBUG_ITERATOR(it)
      if (it->const_data() == "{" && it->type() == Token::TokenType::PUNCTUATOR)
        brace_count += 1;
      else if (it->const_data() == "}" &&
               it->type() == Token::TokenType::PUNCTUATOR)
        brace_count -= 1;

      if (brace_count > 0) { // FIXME: Find a better way to do this
        ret.push_back(*it);
        it++;
      }
      std::cout << "Cmpnd_Sttmnt::After: ";
      DEBUG_ITERATOR(it)
    }
    return ret;
  }

  node check_for_compound_stmnt(Tokenizer::token_list::const_iterator &it) {
    node _ret; //= MAKE_UNIQUE_NODE_PTR(ServerLang::Scope{});
    auto _tmp = new ServerLang::Scope;
    _ret.reset(_tmp);
    m_state = State::NO_OP;
    auto _eval = analyze(get_compound_stmnt_nodes(it));
    for (int i = 0; i < _eval.size(); ++i)
      // FIXME: CUrrently not appending child nodes
      _tmp->children().emplace_back(std::move(_eval.at(i)));
    //_tmp->setChildren(_eval);
    m_state = State::NO_OP;
    it++;
    return _ret;
  }

  node check_for_parameter_list(Tokenizer::token_list::const_iterator &it) {
    while (NOT_DELIMETER(it, ")")) {
      std::cout << "Param_List::Before: ";
      DEBUG_ITERATOR(it)
      if (it++; it->const_data() == "(" &&
                it->type() == Token::TokenType::PUNCTUATOR) {
        it++;
        check_for_parameter_list(it);
      }
      std::cout << "Param_List::After: ";
      DEBUG_ITERATOR(it)
    }
    it++;
    return {};
  }

  node check_for_variable_decl(Tokenizer::token_list::const_iterator &it) {
    auto _id = it->const_data();
    node _var;
    std::string_view _val;

    if (it++;
        it->const_data() == ":" && it->type() == Token::TokenType::PUNCTUATOR) {
      if (it++; MAP_HAS(ServerLang::type_map, it->const_data().c_str()) &&
                it->type() == Token::TokenType::IDENTIFIER) {
        auto _type_string = it->const_data();
        auto _temp = ServerLang::get_type_instance(_type_string.c_str());
        _temp->setId(_id.c_str());
        _var.reset(_temp);
        std::cout << "Variable pref_type: "
                  << static_cast<int>(_var->preferredType()) << std::endl;
        it++;
      } else if (it->type() == Token::TokenType::IDENTIFIER) {
        fprintf(stderr,
                "<IMPL_DECL> of type: %s\nCurrently only internal types can be "
                "implicitly declared \n",
                it->const_data().c_str());
        auto _temp = ServerLang::get_type_instance("Variant");
        _temp->setId(_id.c_str());
        _var.reset(_temp);
        std::cout << "Variable pref_type: "
                  << static_cast<int>(_var->preferredType()) << std::endl;
        it++;
      } else {
        fprintf(stderr, "Expected Identifier after ':' \n");
        m_state = State::NO_OP;
      }
    } else {
      auto _temp = ServerLang::get_type_instance("Variant");
      _temp->setId(_id.c_str());
      _var.reset(_temp);
      std::cout << "Variable pref_type: "
                << static_cast<int>(_var->preferredType()) << std::endl;
    }
    DEBUG_ITERATOR(it)
    if (it->const_data() == "=" &&
        it->type() == Token::TokenType::ARITHMETIC_OPERATOR) {
      if (it++; it->const_data() == "{" &&
                it->type() == Token::TokenType::PUNCTUATOR) {
        std::cout << "Var_Decl::Before: ";
        DEBUG_ITERATOR(it)
        it++;
        std::cout << "Var_Decl::After: ";
        DEBUG_ITERATOR(it)
        check_for_compound_stmnt(it);
      } else {
        auto _tmp = Tokenizer::get_span(it, ";", Token::TokenType::PUNCTUATOR);
        // PRINT_ITERATOR_ARRAY(_tmp);
        it++;
      }
    }

    m_state = State::NO_OP;
    return _var;
  }

  node check_for_const_decl(Tokenizer::token_list::const_iterator &it) {
    std::string _id;
    node _var;
    std::string_view _val;

    // FIXME: This is hacky
    if (it->const_data() == "@" && it->type() == Token::TokenType::PUNCTUATOR) {
      it++;
      _id = it->const_data();
    } else
      _id = it->const_data();

    if (it++;
        it->const_data() == ":" && it->type() == Token::TokenType::PUNCTUATOR) {
      if (it++; MAP_HAS(ServerLang::type_map, it->const_data().c_str()) &&
                it->type() == Token::TokenType::IDENTIFIER) {
        auto _type_string = it->const_data();
        auto _temp = ServerLang::get_type_instance(_type_string.c_str());
        _temp->setId(_id.c_str());
        _var.reset(_temp);
        std::cout << "Variable pref_type: "
                  << static_cast<int>(_var->preferredType()) << std::endl;
        it++;
      } else if (it->type() == Token::TokenType::IDENTIFIER) {
        fprintf(stderr,
                "<IMPL_DECL> of type: %s\nCurrently only internal types can be "
                "implicitly declared \n",
                it->const_data().c_str());
        auto _temp = ServerLang::get_type_instance("Variant");
        _temp->setId(_id.c_str());
        _var.reset(_temp);
        std::cout << "Variable pref_type: "
                  << static_cast<int>(_var->preferredType()) << std::endl;
        it++;
      } else {
        fprintf(stderr, "Expected Identifier after ':' \n");
        m_state = State::NO_OP;
      }
    } else {
      auto _temp = ServerLang::get_type_instance("Variant");
      _temp->setId(_id.c_str());
      _var.reset(_temp);
      std::cout << "Variable pref_type: "
                << static_cast<int>(_var->preferredType()) << std::endl;
    }
    DEBUG_ITERATOR(it)
    if (it->const_data() == "=" &&
        it->type() == Token::TokenType::ARITHMETIC_OPERATOR) {
      if (it++; it->const_data() == "{" &&
                it->type() == Token::TokenType::PUNCTUATOR) {
        std::cout << "Var_Decl::Before: ";
        DEBUG_ITERATOR(it)
        it++;
        std::cout << "Var_Decl::After: ";
        DEBUG_ITERATOR(it)
        _var->children().emplace_back(std::move(check_for_compound_stmnt(it)));
      } else {
        auto _tmp = Tokenizer::get_span(it, ";", Token::TokenType::PUNCTUATOR);
        // PRINT_ITERATOR_ARRAY(_tmp);
        it++;
      }
    }

    m_state = State::NO_OP;
    return _var;
  }

  node check_for_fn_call(Tokenizer::token_list::const_iterator &it) {
    while (NOT_DELIMETER(it, ";")) {
      // TODO
      it++;
    }
    m_state = State::NO_OP;
    return {};
  }
  node check_for_fn_decl(Tokenizer::token_list::const_iterator &it) {
    std::cout << "FN_NAME: ";
    DEBUG_ITERATOR(it)
    auto const _id = it->const_data();
    auto _fn = new ServerLang::Function<ServerLang::node_ptr>();
    _fn->setId(_id.c_str());

    if (it++;
        it->const_data() == "(" && it->type() == Token::TokenType::PUNCTUATOR) {
      it++;
      check_for_parameter_list(it);
    } else {
      fprintf(stderr, "Expected '(' after identifer %s\n", _id.c_str());
      move_to_next_end(it, "}");
    }

    if (it->const_data() == ":" && it->type() == Token::TokenType::PUNCTUATOR) {
      it++;
      DEBUG_ITERATOR(it)
      _fn->setReturn_t(
          ServerLang::type_map.find(it->const_data().c_str())->second);
    } else {
      fprintf(stderr, "Expected ':' after parameter list.\n");
      move_to_next_end(it, "}");
    }

    if (it++;
        it->const_data() == "{" && it->type() == Token::TokenType::PUNCTUATOR) {
      it++;
      check_for_compound_stmnt(it);
    } else {
      fprintf(stderr, "Expected '{' after function type.\n");
      move_to_next_end(it, "}");
    }

    m_state = State::NO_OP;
    node _ret;
    _ret.reset(_fn);
    return _ret;
  }

public: // Static members
  static void print_tree(const ServerLang::node_list &_l,
                         const int offset = 0) {
    for (auto const &v : _l) {
      if (v != nullptr) {
        auto str = std::string(offset, '.');
        fprintf(stdout, "%s| %s : %s\n", str.c_str(), v->id(),
                v->type_string());
        if (v->children_const().size()) {
          print_tree(v->children_const(), offset + 3);
        }
      } else
        std::cout << "[_NULL_OBJECT_]" << std::endl;
    }
  }
};

class Parser {
public:
  Parser() = default;
  ~Parser() {}
};

class Runtime {
public:
  Runtime() = default;
  ~Runtime() {}
};

int main(int argc, char **argv) {

  auto _file = std::fstream();
  std::string data, line;
  _file.open("sample.nsl", std::ios::in);
  if (!_file.is_open())
    fprintf(stderr, "Could not open the specified file: %s \n", "sample.nsl");

  while (std::getline(_file, line)) {
    data.append(line + "\n");
  }
  _file.close();

  fprintf(stdout, "FILE_DATA: \n %s \n", data.c_str());

  auto const tkns = Tokenizer::evaluate(data);

  for (auto const &v : tkns)
    std::cout << Token::TokenNames.at(v.type()) << " : " << v.const_data()
              << std::endl;

  SyntaxAnalyzer _st;
  auto const nodes = _st.analyze(tkns);

  SyntaxAnalyzer::print_tree(nodes);
  return 0;
}