#include <fstream>
#include <iostream>
#include <map>
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
  !(x->const_data() == y && x->type() == Token::TokenType::PUNCTUATOR)

#define MAP_HAS(x, y) (x.find(y) != x.end())

#define DEFAULT_NODE_CONSTRUCTOR(x)                                            \
  x() { setPreferredType(this->type()); }

namespace ServerLang {

// Forward:
class ASTNode;

// Typedefs:
using node_list = std::vector<ASTNode>;

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
  VOID,
  ARRAY,
  CLASS,
  STRUCT,
  JSON,
  ROUTE,
  SCOPE,
  OBJECT,
  PRIMITIVE,
};

class ASTNode {

public: // virtual methods
  virtual ServerLang::Type type() const { return Type::UNDEFINED; };
  virtual const char *type_string() const { return "Undefined"; };

public:
  void setPreferredType(const Type newType) { m_preferredType = newType; }
  Type preferredType() const { return m_preferredType; }

private:
  Type m_preferredType = type();
};

class Scope : public ASTNode {
public:
  DEFAULT_NODE_CONSTRUCTOR(Scope)
  ServerLang::Type type() const override { return Type::SCOPE; }
  const char *type_string() const override { return "Scope"; }

private:
  node_list m_children;
};

class Object : public Scope {
public:
  DEFAULT_NODE_CONSTRUCTOR(Object)
  ServerLang::Type type() const override { return Type::OBJECT; }
  const char *type_string() const override { return "Object"; }
};

class Primitive : public ASTNode {
public:
  DEFAULT_NODE_CONSTRUCTOR(Primitive)
  ServerLang::Type type() const override { return Type::PRIMITIVE; }
  const char *type_string() const override { return "Primitive"; }
};

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
    {"Void", Type::VOID},       {"Array", Type::ARRAY},
    {"Class", Type::CLASS},     {"Struct", Type::STRUCT},
    {"Json", Type::JSON},       {"Route", Type::ROUTE},
};

namespace InternalTypes {

class I16 : public Primitive {
public:
  DEFAULT_NODE_CONSTRUCTOR(I16)
  ServerLang::Type type() const override { return Type::I16; }
  const char *type_string() const override { return "Integer_16"; }
};

class I32 : public Primitive {
public:
  DEFAULT_NODE_CONSTRUCTOR(I32)
  ServerLang::Type type() const override { return Type::I32; }
  const char *type_string() const override { return "Integer_32"; }
};

class I64 : public Primitive {
public:
  DEFAULT_NODE_CONSTRUCTOR(I64)
  ServerLang::Type type() const override { return Type::I64; }
  const char *type_string() const override { return "Integer_64"; }
};

class U8 : public Primitive {
public:
  DEFAULT_NODE_CONSTRUCTOR(U8)
  ServerLang::Type type() const override { return Type::U8; }
  const char *type_string() const override { return "Unsigned_8"; }
};

class U16 : public Primitive {
public:
  DEFAULT_NODE_CONSTRUCTOR(U16)
  ServerLang::Type type() const override { return Type::U16; }
  const char *type_string() const override { return "Unsigned_16"; }
};

class F32 : public Primitive {
public:
  DEFAULT_NODE_CONSTRUCTOR(F32)
  ServerLang::Type type() const override { return Type::F32; }
  const char *type_string() const override { return "Float_32"; }
};

class F64 : public Primitive {
public:
  DEFAULT_NODE_CONSTRUCTOR(F64)
  ServerLang::Type type() const override { return Type::F64; }
  const char *type_string() const override { return "Float_64"; }
};

class Bool : public Primitive {
public:
  DEFAULT_NODE_CONSTRUCTOR(Bool)
  ServerLang::Type type() const override { return Type::BOOL; }
  const char *type_string() const override { return "Boolean"; }
};

class Complex : public Primitive {
public:
  DEFAULT_NODE_CONSTRUCTOR(Complex)
  ServerLang::Type type() const override { return Type::COMPLEX; }
  const char *type_string() const override { return "Complex"; }
};

class String : public Primitive {
public:
  DEFAULT_NODE_CONSTRUCTOR(String)
  ServerLang::Type type() const override { return Type::STRING; }
  const char *type_string() const override { return "String"; }
};

class Void : public Primitive {
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

static const ASTNode get_type_instance(const Type _t) {
  switch (_t) {
  case ServerLang::Type::I16:
    return InternalTypes::I16{};
  case ServerLang::Type::I32:
    return InternalTypes::I32{};
  case ServerLang::Type::I64:
    return InternalTypes::I64{};
  case ServerLang::Type::U8:
    return InternalTypes::U8{};
  case ServerLang::Type::U16:
    return InternalTypes::U16{};
  case ServerLang::Type::F32:
    return InternalTypes::F32{};
  case ServerLang::Type::F64:
    return InternalTypes::F64{};
  case ServerLang::Type::BOOL:
    return InternalTypes::Bool{};
  case ServerLang::Type::COMPLEX:
    return InternalTypes::Complex{};
  case ServerLang::Type::STRING:
    return InternalTypes::String{};
  case ServerLang::Type::VOID:
    return InternalTypes::Void{};
  case ServerLang::Type::ARRAY:
    return CompoundTypes::Array{};
  case ServerLang::Type::CLASS:
    return CompoundTypes::Class{};
  case ServerLang::Type::STRUCT:
    return CompoundTypes::Struct{};
  case ServerLang::Type::JSON:
    return CompoundTypes::Json{};
  case ServerLang::Type::ROUTE:
    return CompoundTypes::Route{};
  default:
    return {};
    break;
  }
}

static const ASTNode get_type_instance(const char *_t_string) {
  auto _code = ServerLang::type_map.find(_t_string)->second;
  return get_type_instance(_code);
}

} // namespace ServerLang

class Token {
public:
  enum class TokenType {
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
      case ':':
      case '@':
        if (__NOT_STRING_OR_COMMENT__) {
          end_token(current_token, list);
          current_token.setType(Token::TokenType::PUNCTUATOR);
          current_token.data().append(1, v);
          end_token(current_token, list);
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
};

class SyntaxAnalyzer {
  enum class State {
    NO_OP,
    VARIABLE_DECL,
    CONST_DECL,
    FUNCTION_DECL,
    EXPRESSION
  };

  using node = ServerLang::ASTNode;
  using object = ServerLang::Object;
  using scope = ServerLang::Scope;

public:
  SyntaxAnalyzer() = default;
  ~SyntaxAnalyzer() {}

public:
  const ServerLang::node_list analyze(const Tokenizer::token_list &tokens) {
    ServerLang::node_list ret;
    auto itr = tokens.cbegin();
    while (itr != tokens.cend()) {
      switch (m_state) {
      case State::NO_OP:
        // std::cout << "[NO OP]" << std::endl;
        check_for_next_possible(itr);
        itr++;
        break;
      case State::CONST_DECL:
        std::cout << "[CONST DECL]" << std::endl;
        ret.push_back(check_for_const_decl(itr));
        break;
      case State::VARIABLE_DECL:
        std::cout << "[VAR DECL]" << std::endl;
        ret.push_back(check_for_variable_decl(itr));
        break;
      case State::FUNCTION_DECL:
        std::cout << "[FUNCTION DECL]" << std::endl;
        ret.push_back(check_for_fn_decl(itr));
        break;
      case State::EXPRESSION:
        std::cout << "[EXPRESSION]" << std::endl;
        ret.push_back(check_for_expression(itr));
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
  void check_for_next_possible(Tokenizer::token_list::const_iterator it) {
    switch (it->type()) {
    case Token::TokenType::COMMENT:
      m_state = State::NO_OP;
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
      it++;
      break;
    default:
      m_state = State::NO_OP;
      break;
    }
  }
  node check_for_expression(Tokenizer::token_list::const_iterator it) {
    while (NOT_DELIMETER(it, ";")) {
      // TODO
      it++;
    }
    m_state = State::NO_OP;
    return {};
  }
  scope check_for_compound_stmnt(Tokenizer::token_list::const_iterator it) {
    while (NOT_DELIMETER(it, "}")) {
      // TODO
      it++;
    }
    m_state = State::NO_OP;
    return {};
  }
  node check_for_variable_decl(Tokenizer::token_list::const_iterator it) {
    auto _id = it->const_data();
    node _var;
    std::string_view _val;
    it++;

    if (it->const_data() == ":" && it->type() == Token::TokenType::PUNCTUATOR) {
      it++;
      if (MAP_HAS(ServerLang::type_map, it->const_data().c_str()) &&
          it->type() == Token::TokenType::IDENTIFIER) {
        auto _type_string = it->const_data();
        _var = ServerLang::get_type_instance(_type_string.c_str());
        std::cout << "Variable pref_type: "
                  << static_cast<int>(_var.preferredType()) << std::endl;
      } else if (it->type() == Token::TokenType::IDENTIFIER)
        fprintf(stderr,
                "<IMPL_DECL> of type: %s\nCurrently only internal types can be "
                "implicitly declared \n",
                it->const_data().c_str());
    } else {
      fprintf(stderr, "Expected Identifier after ':' \n");
    }
    m_state = State::NO_OP;
    return _var;
  }
  node check_for_const_decl(Tokenizer::token_list::const_iterator it) {
    while (NOT_DELIMETER(it, ";")) {
      // TODO
      it++;
    }
    m_state = State::NO_OP;
    return {};
  }
  node check_for_fn_call(Tokenizer::token_list::const_iterator it) {
    while (NOT_DELIMETER(it, ";")) {
      // TODO
      it++;
    }
    m_state = State::NO_OP;
    return {};
  }
  node check_for_fn_decl(Tokenizer::token_list::const_iterator it) {
    while (NOT_DELIMETER(it, ";")) {
      // TODO
      it++;
    }
    m_state = State::NO_OP;
    return {};
  }
};

class Parser {
public:
  Parser() = default;
  ~Parser() {}
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
  _st.analyze(tkns);
}