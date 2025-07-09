#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <stdexcept>
#include <fstream>

// Token types for our simple language
enum TokenType {
    VOID, MAIN, LPAREN, RPAREN, LBRACE, RBRACE,
    NUMBER, PLUS, MINUS, MULTIPLY, DIVIDE, SEMICOLON,
    IDENTIFIER, COMMA,
    EOF_TOKEN, UNKNOWN
};

// Token structure
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token(TokenType t, const std::string& v, int l, int c) 
        : type(t), value(v), line(l), column(c) {}
};

// Lexer class - converts source code into tokens
class Lexer {
private:
    std::string source;
    size_t pos;
    int line;
    int column;
    
    char currentChar() {
        if (pos >= source.length()) return '\0';
        return source[pos];
    }
    
    void advance() {
        if (pos < source.length()) {
            if (source[pos] == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
            pos++;
        }
    }
    
    void skipWhitespace() {
        while (isspace(currentChar())) {
            advance();
        }
    }
    
    Token makeNumber() {
        std::string num;
        int startCol = column;
        
        while (isdigit(currentChar())) {
            num += currentChar();
            advance();
        }
        
        return Token(NUMBER, num, line, startCol);
    }
    
    Token makeIdentifier() {
        std::string id;
        int startCol = column;
        
        while (isalnum(currentChar()) || currentChar() == '_') {
            id += currentChar();
            advance();
        }
        
        // Check for keywords
        if (id == "void") return Token(VOID, id, line, startCol);
        if (id == "main") return Token(MAIN, id, line, startCol);
        
        // Everything else is an identifier (function names, variable names, etc.)
        return Token(IDENTIFIER, id, line, startCol);
    }
    
public:
    Lexer(const std::string& src) : source(src), pos(0), line(1), column(1) {}
    
    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        
        while (currentChar() != '\0') {
            skipWhitespace();
            
            if (currentChar() == '\0') break;
            
            char ch = currentChar();
            int startCol = column;
            
            if (isdigit(ch)) {
                tokens.push_back(makeNumber());
            } else if (isalpha(ch) || ch == '_') {
                tokens.push_back(makeIdentifier());
            } else {
                switch (ch) {
                    case '(': tokens.push_back(Token(LPAREN, "(", line, startCol)); advance(); break;
                    case ')': tokens.push_back(Token(RPAREN, ")", line, startCol)); advance(); break;
                    case '{': tokens.push_back(Token(LBRACE, "{", line, startCol)); advance(); break;
                    case '}': tokens.push_back(Token(RBRACE, "}", line, startCol)); advance(); break;
                    case '+': tokens.push_back(Token(PLUS, "+", line, startCol)); advance(); break;
                    case '-': tokens.push_back(Token(MINUS, "-", line, startCol)); advance(); break;
                    case '*': tokens.push_back(Token(MULTIPLY, "*", line, startCol)); advance(); break;
                    case '/': tokens.push_back(Token(DIVIDE, "/", line, startCol)); advance(); break;
                    case ';': tokens.push_back(Token(SEMICOLON, ";", line, startCol)); advance(); break;
                    case ',': tokens.push_back(Token(COMMA, ",", line, startCol)); advance(); break;
                    default:
                        std::cout << "Warning: Unknown character '" << ch << "' at line " 
                                  << line << ", column " << column << std::endl;
                        advance();
                        break;
                }
            }
        }
        
        tokens.push_back(Token(EOF_TOKEN, "", line, column));
        return tokens;
    }
};

// AST Node types
enum NodeType {
    PROGRAM_NODE, MAIN_FUNCTION_NODE, ARITHMETIC_NODE, NUMBER_NODE, FUNCTION_CALL_NODE
};

// Base AST Node
struct ASTNode {
    NodeType type;
    std::vector<ASTNode*> children;
    std::string value;
    
    ASTNode(NodeType t, const std::string& v = "") : type(t), value(v) {}
    
    ~ASTNode() {
        for (auto child : children) {
            delete child;
        }
    }
};

// Parser class - converts tokens into Abstract Syntax Tree
class Parser {
private:
    std::vector<Token> tokens;
    size_t pos;
    
    Token& currentToken() {
        if (pos >= tokens.size()) {
            static Token eofToken(EOF_TOKEN, "", 0, 0);
            return eofToken;
        }
        return tokens[pos];
    }
    
    void advance() {
        if (pos < tokens.size()) pos++;
    }
    
    void expect(TokenType type) {
        if (currentToken().type != type) {
            throw std::runtime_error("Expected token type " + std::to_string(type) + 
                                     " but got " + std::to_string(currentToken().type));
        }
        advance();
    }
    
    // Parse arithmetic expression with proper precedence
    ASTNode* parseExpression() {
        return parseAddition();
    }
    
    ASTNode* parseAddition() {
        ASTNode* left = parseMultiplication();
        
        while (currentToken().type == PLUS || currentToken().type == MINUS) {
            Token op = currentToken();
            advance();
            ASTNode* right = parseMultiplication();
            
            ASTNode* node = new ASTNode(ARITHMETIC_NODE, op.value);
            node->children.push_back(left);
            node->children.push_back(right);
            left = node;
        }
        
        return left;
    }
    
    ASTNode* parseMultiplication() {
        ASTNode* left = parsePrimary();
        
        while (currentToken().type == MULTIPLY || currentToken().type == DIVIDE) {
            Token op = currentToken();
            advance();
            ASTNode* right = parsePrimary();
            
            ASTNode* node = new ASTNode(ARITHMETIC_NODE, op.value);
            node->children.push_back(left);
            node->children.push_back(right);
            left = node;
        }
        
        return left;
    }
    
    ASTNode* parsePrimary() {
        if (currentToken().type == NUMBER) {
            ASTNode* node = new ASTNode(NUMBER_NODE, currentToken().value);
            advance();
            return node;
        } else if (currentToken().type == IDENTIFIER) {
            // Check if this is a function call
            std::string funcName = currentToken().value;
            advance();
            
            if (currentToken().type == LPAREN) {
                // This is a function call
                advance(); // consume '('
                
                ASTNode* funcCall = new ASTNode(FUNCTION_CALL_NODE, funcName);
                
                // Parse arguments
                if (currentToken().type != RPAREN) {
                    do {
                        ASTNode* arg = parseExpression();
                        funcCall->children.push_back(arg);
                        
                        if (currentToken().type == COMMA) {
                            advance(); // consume ','
                        } else {
                            break;
                        }
                    } while (currentToken().type != RPAREN);
                }
                
                expect(RPAREN); // consume ')'
                return funcCall;
            } else {
                // This would be a variable reference (not implemented yet)
                throw std::runtime_error("Variables not implemented yet: " + funcName);
            }
        } else if (currentToken().type == LPAREN) {
            advance(); // consume '('
            ASTNode* node = parseExpression();
            expect(RPAREN); // consume ')'
            return node;
        } else {
            throw std::runtime_error("Expected number, identifier, or '(' at line " + 
                                     std::to_string(currentToken().line));
        }
    }
    
public:
    Parser(const std::vector<Token>& toks) : tokens(toks), pos(0) {}
    
    ASTNode* parse() {
        ASTNode* program = new ASTNode(PROGRAM_NODE);
        
        // Parse: void main() { ... }
        expect(VOID);
        expect(MAIN);
        expect(LPAREN);
        expect(RPAREN);
        expect(LBRACE);
        
        ASTNode* mainFunc = new ASTNode(MAIN_FUNCTION_NODE);
        
        // Parse arithmetic expressions and function calls until we hit '}'
        while (currentToken().type != RBRACE && currentToken().type != EOF_TOKEN) {
            if (currentToken().type == NUMBER || currentToken().type == LPAREN || 
                currentToken().type == IDENTIFIER) {
                ASTNode* expr = parseExpression();
                mainFunc->children.push_back(expr);
                
                // Optional semicolon
                if (currentToken().type == SEMICOLON) {
                    advance();
                }
            } else {
                advance(); // Skip unknown tokens
            }
        }
        
        expect(RBRACE);
        program->children.push_back(mainFunc);
        
        return program;
    }
};

// Evaluator class - executes the AST
class Evaluator {
public:
    int evaluate(ASTNode* node) {
        switch (node->type) {
            case PROGRAM_NODE: {
                // Execute main function
                if (!node->children.empty()) {
                    return evaluate(node->children[0]);
                }
                return 0;
            }
                
            case MAIN_FUNCTION_NODE: {
                // Execute all expressions in main and return the last result
                int result = 0;
                for (auto child : node->children) {
                    result = evaluate(child);
                    // Don't print results for function calls (they handle their own output)
                    if (child->type != FUNCTION_CALL_NODE) {
                        std::cout << "Result: " << result << std::endl;
                    }
                }
                return result;
            }
                
            case ARITHMETIC_NODE: {
                if (node->children.size() != 2) {
                    throw std::runtime_error("Arithmetic node must have exactly 2 children");
                }
                
                int left = evaluate(node->children[0]);
                int right = evaluate(node->children[1]);
                
                if (node->value == "+") return left + right;
                if (node->value == "-") return left - right;
                if (node->value == "*") return left * right;
                if (node->value == "/") {
                    if (right == 0) throw std::runtime_error("Division by zero");
                    return left / right;
                }
                throw std::runtime_error("Unknown arithmetic operator: " + node->value);
            }
                
            case NUMBER_NODE: {
                return std::stoi(node->value);
            }
            
            case FUNCTION_CALL_NODE: {
                // Handle built-in functions
                if (node->value == "print") {
                    if (node->children.size() != 1) {
                        throw std::runtime_error("print() function expects exactly 1 argument");
                    }
                    int value = evaluate(node->children[0]);
                    std::cout << value;
                    return value;
                } else {
                    throw std::runtime_error("Unknown function: " + node->value);
                }
            }
                
            default: {
                throw std::runtime_error("Unknown node type");
            }
        }
    }
};

// Main compiler function
int main(int argc, char* argv[]) {
    // Check command line arguments
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file>" << std::endl;
        std::cerr << "Example: " << argv[0] << " main.x" << std::endl;
        return 1;
    }
    
    std::string filename = argv[1];
    std::string sourceCode;
    
    // Read the source file
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'" << std::endl;
        return 1;
    }
    
    // Read entire file content
    std::string line;
    while (std::getline(file, line)) {
        sourceCode += line + "\n";
    }
    file.close();
    
    if (sourceCode.empty()) {
        std::cerr << "Error: File '" << filename << "' is empty" << std::endl;
        return 1;
    }
    
    try {
        std::cout << "Compiling file: " << filename << std::endl;
        std::cout << "Compiling..." << std::endl;
        
        // Lexical analysis
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();
        
        // Parsing
        Parser parser(tokens);
        ASTNode* ast = parser.parse();
        
        // Evaluation
        Evaluator evaluator;
        int finalResult = evaluator.evaluate(ast);
        
        // Cleanup
        delete ast;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
