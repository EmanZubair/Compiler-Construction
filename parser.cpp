#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <map>
#include <fstream>  // Include this header for file handling

using namespace std;

enum TokenType {
    T_INT, T_FLOAT, T_DOUBLE, T_STRING, T_BOOL, T_CHAR,
    T_ID, T_NUM, T_AGAR, T_ELSE, T_RETURN, 
    T_FOR, T_WHILE, T_SWITCH, T_CASE, T_DEFAULT, T_BREAK,
    T_ASSIGN, T_PLUS, T_MINUS, T_MUL, T_DIV, 
    T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE,  
    T_SEMICOLON, T_GT, T_EOF
};

struct Token {
    TokenType type;
    string value;
};

class Lexer {
private:
    string src;
    size_t pos;
    size_t line;  // To track the current line number
    size_t lineStart; // To track the start position of the current line

public:
    Lexer(const string &src) : src(src), pos(0), line(1), lineStart(0) {}

    vector<Token> tokenize() {
        vector<Token> tokens;
        while (pos < src.size()) {
            char current = src[pos];

            if (isspace(current)) {
                if (current == '\n') {
                    line++;
                    lineStart = pos + 1; // Update start position for the new line
                }
                pos++;
                continue;
            }
            if (isdigit(current)) {
                tokens.push_back(Token{T_NUM, consumeNumber()});
                continue;
            }
            if (isalpha(current)) {
                string word = consumeWord();
                if (word == "int") tokens.push_back(Token{T_INT, word});
                else if (word == "float") tokens.push_back(Token{T_FLOAT, word});
                else if (word == "double") tokens.push_back(Token{T_DOUBLE, word});
                else if (word == "string") tokens.push_back(Token{T_STRING, word});
                else if (word == "bool") tokens.push_back(Token{T_BOOL, word});
                else if (word == "char") tokens.push_back(Token{T_CHAR, word});
                else if (word == "Agar") tokens.push_back(Token{T_AGAR, word});  // Changed from "if" to "Agar"
                else if (word == "else") tokens.push_back(Token{T_ELSE, word});
                else if (word == "return") tokens.push_back(Token{T_RETURN, word});
                else if (word == "for") tokens.push_back(Token{T_FOR, word});
                else if (word == "while") tokens.push_back(Token{T_WHILE, word});
                else if (word == "switch") tokens.push_back(Token{T_SWITCH, word});
                else if (word == "case") tokens.push_back(Token{T_CASE, word});
                else if (word == "default") tokens.push_back(Token{T_DEFAULT, word});
                else if (word == "break") tokens.push_back(Token{T_BREAK, word});
                else tokens.push_back(Token{T_ID, word});
                continue;
            }

            switch (current) {
                case '=': tokens.push_back(Token{T_ASSIGN, "="}); break;
                case '+': tokens.push_back(Token{T_PLUS, "+"}); break;
                case '-': tokens.push_back(Token{T_MINUS, "-"}); break;
                case '*': tokens.push_back(Token{T_MUL, "*"}); break;
                case '/': tokens.push_back(Token{T_DIV, "/"}); break;
                case '(': tokens.push_back(Token{T_LPAREN, "("}); break;
                case ')': tokens.push_back(Token{T_RPAREN, ")"}); break;
                case '{': tokens.push_back(Token{T_LBRACE, "{"}); break;  
                case '}': tokens.push_back(Token{T_RBRACE, "}"}); break;  
                case ';': tokens.push_back(Token{T_SEMICOLON, ";"}); break;
                case '>': tokens.push_back(Token{T_GT, ">"}); break;
                default: 
                    cout << "Unexpected character: " << current 
                         << " at line " << line << endl; 
                    exit(1);
            }
            pos++;
        }
        tokens.push_back(Token{T_EOF, ""});
        return tokens;
    }

    string consumeNumber() {
        size_t start = pos;
        while (pos < src.size() && isdigit(src[pos])) pos++;
        return src.substr(start, pos - start);
    }

    string consumeWord() {
        size_t start = pos;
        while (pos < src.size() && isalnum(src[pos])) pos++;
        return src.substr(start, pos - start);
    }
};

class Parser {
public:
    Parser(const vector<Token> &tokens) : tokens(tokens), pos(0) {}

    void parseProgram() {
        while (tokens[pos].type != T_EOF) {
            parseStatement();
        }
        cout << "Parsing completed successfully! No Syntax Error" << endl;
    }

private:
    vector<Token> tokens;
    size_t pos;

    void parseStatement() {
        if (tokens[pos].type == T_INT || tokens[pos].type == T_FLOAT ||
            tokens[pos].type == T_DOUBLE || tokens[pos].type == T_STRING ||
            tokens[pos].type == T_BOOL || tokens[pos].type == T_CHAR) {
            parseDeclaration();
        } else if (tokens[pos].type == T_ID) {
            parseAssignment();
        } else if (tokens[pos].type == T_AGAR) {  // Changed from T_IF to T_AGAR
            parseAgarStatement();
        } else if (tokens[pos].type == T_RETURN) {
            parseReturnStatement();
        } else if (tokens[pos].type == T_LBRACE) {  
            parseBlock();
        } else {
            cout << "Syntax error: unexpected token " << tokens[pos].value 
                 << " at line " << getLineNumber() << endl;
            exit(1);
        }
    }

    void parseBlock() {
        expect(T_LBRACE);  
        while (tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF) {
            parseStatement();
        }
        expect(T_RBRACE);  
    }

    void parseDeclaration() {
        if (tokens[pos].type == T_INT || tokens[pos].type == T_FLOAT ||
            tokens[pos].type == T_DOUBLE || tokens[pos].type == T_STRING ||
            tokens[pos].type == T_BOOL || tokens[pos].type == T_CHAR) {
            expect(tokens[pos].type);  // Expect the type
            expect(T_ID);              // Expect an identifier
            expect(T_SEMICOLON);       // Expect a semicolon
        }
    }

    void parseAssignment() {
        expect(T_ID);
        expect(T_ASSIGN);
        parseExpression();
        expect(T_SEMICOLON);
    }

    void parseAgarStatement() {
        expect(T_AGAR);
        expect(T_LPAREN);
        parseExpression();
        expect(T_RPAREN);
        parseStatement();  
        if (tokens[pos].type == T_ELSE) {
            expect(T_ELSE);
            parseStatement();  
        }
    }

    void parseReturnStatement() {
        expect(T_RETURN);
        parseExpression();
        expect(T_SEMICOLON);
    }

    void parseExpression() {
        parseTerm();
        while (tokens[pos].type == T_PLUS || tokens[pos].type == T_MINUS) {
            pos++;
            parseTerm();
        }
        if (tokens[pos].type == T_GT) {
            pos++;
            parseExpression();  // After relational operator, parse the next expression
        }
    }

    void parseTerm() {
        parseFactor();
        while (tokens[pos].type == T_MUL || tokens[pos].type == T_DIV) {
            pos++;
            parseFactor();
        }
    }

    void parseFactor() {
        if (tokens[pos].type == T_NUM || tokens[pos].type == T_ID) {
            pos++;
        } else if (tokens[pos].type == T_LPAREN) {
            expect(T_LPAREN);
            parseExpression();
            expect(T_RPAREN);
        } else {
            cout << "Syntax error: unexpected token " << tokens[pos].value 
                 << " at line " << getLineNumber() << endl;
            exit(1);
        }
    }

    void expect(TokenType type) {
        if (tokens[pos].type == type) {
            pos++;
        } else {
            cout << "Syntax error: expected " << type 
                 << " but found " << tokens[pos].value 
                 << " at line " << getLineNumber() << endl;
            exit(1);
        }
    }

    size_t getLineNumber() {
        // Returns the line number for the current token (basic implementation)
        // In a real scenario, you may need to keep track of line numbers more precisely
        return 1; // Placeholder; update as necessary
    }
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }

    string filename = argv[1];
    ifstream file(filename);
    if (!file) {
        cout << "Error opening file: " << filename << endl;
        return 1;
    }

    string input((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    
    Lexer lexer(input);
    vector<Token> tokens = lexer.tokenize();
    
    Parser parser(tokens);
    parser.parseProgram();

    return 0;
}
