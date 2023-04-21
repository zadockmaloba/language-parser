#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

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
        if (current_token.type() != Token::TokenType::STRING_LITERAL) {
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
        if (current_token.type() != Token::TokenType::STRING_LITERAL) {
          current_token.setType(Token::TokenType::PUNCTUATOR);
          current_token.data().append(1, v);
          end_token(current_token, list);
        } else
          current_token.data().append(1, v);
        break;
      case '$':
        if (current_token.type() != Token::TokenType::STRING_LITERAL) {
          current_token.setType(Token::TokenType::POINTER_OPERATOR);
          current_token.data().append(1, v);
          end_token(current_token, list);
        } else {
          current_token.data().append(1, v);
        }
        break;
      case '.':
        if (current_token.type() != Token::TokenType::STRING_LITERAL) {
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
        if (current_token.type() != Token::TokenType::STRING_LITERAL) {
          current_token.setType(Token::TokenType::LOGIC_OPERATOR);
          current_token.data().append(1, v);
        } else
          current_token.data().append(1, v);
        break;
      case '+':
      case '-':
      case '*':
      case '/':
      case '^':
      case '=':
        if (current_token.type() != Token::TokenType::STRING_LITERAL) {
          current_token.setType(Token::TokenType::ARITHMETIC_OPERATOR);
          current_token.data().append(1, v);
        } else
          current_token.data().append(1, v);
        break;
      case '\r':
      case '\n':
      case '\t':
        end_token(current_token, list);
        break;
      case ' ':
        if (current_token.type() == Token::TokenType::STRING_LITERAL)
          current_token.data().append(1, v);
        else
          end_token(current_token, list);
        break;
      case '\"':
        if (current_token.type() != Token::TokenType::STRING_LITERAL) {
          end_token(current_token, list);
          current_token.setType(Token::TokenType::STRING_LITERAL);
        } else if (current_token.type() == Token::TokenType::STRING_LITERAL) {
          end_token(current_token, list);
        }
        break;
      default:
        if (current_token.type() == Token::TokenType::STRING_LITERAL)
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

class Lexer {
public:
  Lexer() = default;
  ~Lexer() {}
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
    fprintf(stderr, "Could not open the speciied file: %s \n", "sample.nsl");

  while (std::getline(_file, line)) {
    data.append(line + "\n");
  }
  _file.close();

  fprintf(stdout, "FILE_DATA: \n %s \n", data.c_str());

  auto const tkns = Tokenizer::evaluate(data);

  for (auto const &v : tkns)
    std::cout << Token::TokenNames.at(v.type()) << " : " << v.const_data()
              << std::endl;
}