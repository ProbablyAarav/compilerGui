#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <map>

// Token types for our language
enum TokenType {
    VOID, MAIN, LPAREN, RPAREN, LBRACE, RBRACE,
    NUMBER, PLUS, MINUS, MULTIPLY, DIVIDE, SEMICOLON,
    IDENTIFIER, COMMA, STRING, ASSIGN, INT,
    IF, ELSE, WHILE, FOR, RETURN, LBRACKET, RBRACKET,
    EQUAL, NOT_EQUAL, LESS_THAN, GREATER_THAN, LESS_THAN_OR_EQUAL, GREATER_THAN_OR_EQUAL,
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
    
    char peekChar() {
        if (pos + 1 >= source.length()) return '\0';
        return source[pos + 1];
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
    void skipComment() {
	    if (currentChar() == '/' && peekChar() == '/') {
		    while (currentChar() != '\n' && currentChar() != '\0') {
			    advance();
		    }
	    }
	    else if (currentChar() == '/' && peekChar() == '*') {
		    advance();
		    advance();
		    while (currentChar() != '\0') {
			    if (currentChar() == '*' && peekChar() == '/') {
				    advance();
				    advance();
				    break;
			    }
			    advance();
		    }
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
    
    Token makeString() {
        std::string str;
        int startCol = column;
        
        advance(); // Skip opening quote
        
        while (currentChar() != '"' && currentChar() != '\0') {
            if (currentChar() == '\\') {
                advance(); // Skip backslash
                char escaped = currentChar();
                switch (escaped) {
                    case 'n': str += '\n'; break;
                    case 't': str += '\t'; break;
                    case 'r': str += '\r'; break;
                    case '\\': str += '\\'; break;
                    case '"': str += '"'; break;
                    default: 
                        str += '\\';
                        str += escaped;
                        break;
                }
                advance();
            } else {
                str += currentChar();
                advance();
            }
        }
        
        if (currentChar() == '"') {
            advance(); // Skip closing quote
        } else {
            throw std::runtime_error("Unterminated string literal at line " + std::to_string(line));
        }
        
        return Token(STRING, str, line, startCol);
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
        if (id == "int") return Token(INT, id, line, startCol);
        if (id == "if") return Token(IF, id, line, startCol);
        if (id == "else") return Token(ELSE, id, line, startCol);
        if (id == "while") return Token(WHILE, id, line, startCol);
        if (id == "for") return Token(FOR, id, line, startCol);
        if (id == "return") return Token(RETURN, id, line, startCol);
        
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
            } else if (ch == '"') {
                tokens.push_back(makeString());
            } else if (isalpha(ch) || ch == '_') {
                tokens.push_back(makeIdentifier());
            } else {
                switch (ch) {
                    case '(': tokens.push_back(Token(LPAREN, "(", line, startCol)); advance(); break;
                    case ')': tokens.push_back(Token(RPAREN, ")", line, startCol)); advance(); break;
                    case '{': tokens.push_back(Token(LBRACE, "{", line, startCol)); advance(); break;
                    case '}': tokens.push_back(Token(RBRACE, "}", line, startCol)); advance(); break;
                    case '[': tokens.push_back(Token(LBRACKET, "[", line, startCol)); advance(); break;
                    case ']': tokens.push_back(Token(RBRACKET, "]", line, startCol)); advance(); break;
                    case '+': tokens.push_back(Token(PLUS, "+", line, startCol)); advance(); break;
                    case '-': tokens.push_back(Token(MINUS, "-", line, startCol)); advance(); break;
                    case '*': tokens.push_back(Token(MULTIPLY, "*", line, startCol)); advance(); break;
                    case '/': 
			      if (peekChar() == '/' || peekChar() == '*') {
				      skipComment();
				      break;
			      }
			      else {
			      	tokens.push_back(Token(DIVIDE, "/", line, startCol)); advance(); break;
			      }
                    case ';': tokens.push_back(Token(SEMICOLON, ";", line, startCol)); advance(); break;
                    case ',': tokens.push_back(Token(COMMA, ",", line, startCol)); advance(); break;
                    case '=':
                        if (peekChar() == '=') {
                            tokens.push_back(Token(EQUAL, "==", line, startCol));
                            advance(); advance();
                        } else {
                            tokens.push_back(Token(ASSIGN, "=", line, startCol));
                            advance();
                        }
                        break;
                    case '!':
                        if (peekChar() == '=') {
                            tokens.push_back(Token(NOT_EQUAL, "!=", line, startCol));
                            advance(); advance();
                        } else {
                            std::cout << "Warning: Unknown character '" << ch << "' at line " 
                                      << line << ", column " << column << std::endl;
                            advance();
                        }
                        break;
                    case '<': 
			if (peekChar() == '=') {
				tokens.push_back(Token(LESS_THAN_OR_EQUAL, "<=", line, startCol));
				advance(); advance();
			}
			else {
				tokens.push_back(Token(LESS_THAN, "<", line, startCol));
				advance();
			}
			break;
                    case '>': 
			      if (peekChar() == '=') {
				      tokens.push_back(Token(GREATER_THAN_OR_EQUAL, ">=", line, startCol));
				      advance(); advance();
			      }
			      else {
				      tokens.push_back(Token(GREATER_THAN, ">", line, startCol));
				      advance();
			      }
			      break;
			      
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
    PROGRAM_NODE, MAIN_FUNCTION_NODE, ARITHMETIC_NODE, NUMBER_NODE, 
    FUNCTION_CALL_NODE, STRING_NODE, VARIABLE_NODE, ASSIGNMENT_NODE,
    IF_NODE, WHILE_NODE, COMPARISON_NODE, BLOCK_NODE, RETURN_NODE,
    FUNCTION_DEF_NODE, VARIABLE_DECL_NODE
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

// Parser class
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
    
    ASTNode* parseExpression() {
        return parseComparison();
    }
    
    ASTNode* parseComparison() {
        ASTNode* left = parseAddition();
        
        while (currentToken().type == EQUAL || currentToken().type == NOT_EQUAL ||
               currentToken().type == LESS_THAN || currentToken().type == GREATER_THAN ||
	       currentToken().type == LESS_THAN_OR_EQUAL || currentToken().type == GREATER_THAN_OR_EQUAL) {
            Token op = currentToken();
            advance();
            ASTNode* right = parseAddition();
            
            ASTNode* node = new ASTNode(COMPARISON_NODE, op.value);
            node->children.push_back(left);
            node->children.push_back(right);
            left = node;
        }
        
        return left;
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
        } else if (currentToken().type == STRING) {
            ASTNode* node = new ASTNode(STRING_NODE, currentToken().value);
            advance();
            return node;
        } else if (currentToken().type == IDENTIFIER) {
            std::string name = currentToken().value;
            advance();
            
            if (currentToken().type == LPAREN) {
                // Function call
                advance(); // consume '('
                
                ASTNode* funcCall = new ASTNode(FUNCTION_CALL_NODE, name);
                
                if (currentToken().type != RPAREN) {
                    do {
                        ASTNode* arg = parseExpression();
                        funcCall->children.push_back(arg);
                        
                        if (currentToken().type == COMMA) {
                            advance();
                        } else {
                            break;
                        }
                    } while (currentToken().type != RPAREN);
                }
                
                expect(RPAREN);
                return funcCall;
            } else {
                // Variable reference
                return new ASTNode(VARIABLE_NODE, name);
            }
        } else if (currentToken().type == LPAREN) {
            advance(); // consume '('
            ASTNode* node = parseExpression();
            expect(RPAREN);
            return node;
        } else {
            throw std::runtime_error("Expected number, string, identifier, or '(' at line " + 
                                     std::to_string(currentToken().line));
        }
    }
    
    ASTNode* parseStatement() {
        if (currentToken().type == INT) {
            // Variable declaration
            advance(); // consume 'int'
            std::string varName = currentToken().value;
            expect(IDENTIFIER);
            
            ASTNode* varDecl = new ASTNode(VARIABLE_DECL_NODE, varName);
            
            if (currentToken().type == ASSIGN) {
                advance(); // consume '='
                ASTNode* value = parseExpression();
                varDecl->children.push_back(value);
            }
            
            expect(SEMICOLON);
            return varDecl;
        } else if (currentToken().type == IDENTIFIER) {
            // Assignment or expression
            std::string name = currentToken().value;
            advance();
            
            if (currentToken().type == ASSIGN) {
                advance(); // consume '='
                ASTNode* value = parseExpression();
                expect(SEMICOLON);
                
                ASTNode* assignment = new ASTNode(ASSIGNMENT_NODE, name);
                assignment->children.push_back(value);
                return assignment;
            } else {
                // Put the identifier back for expression parsing
                pos--;
                ASTNode* expr = parseExpression();
                expect(SEMICOLON);
                return expr;
            }
        } else if (currentToken().type == IF) {
            advance(); // consume 'if'
            expect(LPAREN);
            ASTNode* condition = parseExpression();
            expect(RPAREN);
            
            ASTNode* thenStmt = parseStatement();
            
            ASTNode* ifNode = new ASTNode(IF_NODE);
            ifNode->children.push_back(condition);
            ifNode->children.push_back(thenStmt);
            
            if (currentToken().type == ELSE) {
                advance(); // consume 'else'
                ASTNode* elseStmt = parseStatement();
                ifNode->children.push_back(elseStmt);
            }
            
            return ifNode;
        } else if (currentToken().type == WHILE) {
            advance(); // consume 'while'
            expect(LPAREN);
            ASTNode* condition = parseExpression();
            expect(RPAREN);
            ASTNode* body = parseStatement();
            
            ASTNode* whileNode = new ASTNode(WHILE_NODE);
            whileNode->children.push_back(condition);
            whileNode->children.push_back(body);
            return whileNode;
        } else if (currentToken().type == LBRACE) {
            // Block
            advance(); // consume '{'
            ASTNode* block = new ASTNode(BLOCK_NODE);
            
            while (currentToken().type != RBRACE && currentToken().type != EOF_TOKEN) {
                ASTNode* stmt = parseStatement();
                block->children.push_back(stmt);
            }
            
            expect(RBRACE);
            return block;
        } else if (currentToken().type == RETURN) {
            advance(); // consume 'return'
            ASTNode* returnNode = new ASTNode(RETURN_NODE);
            
            if (currentToken().type != SEMICOLON) {
                ASTNode* value = parseExpression();
                returnNode->children.push_back(value);
            }
            
            expect(SEMICOLON);
            return returnNode;
        } else {
            // Expression statement
            ASTNode* expr = parseExpression();
            expect(SEMICOLON);
            return expr;
        }
    }
    
public:
    Parser(const std::vector<Token>& toks) : tokens(toks), pos(0) {}
    
    ASTNode* parse() {
        ASTNode* program = new ASTNode(PROGRAM_NODE);
        
	if (currentToken().type != VOID) {
            throw std::runtime_error("Error: couldn't find main function. Make sure to define it as void main() {");
        }

        // Parse: void main() { ... }
        expect(VOID);
        expect(MAIN);
        expect(LPAREN);
        expect(RPAREN);
        expect(LBRACE);
        
        ASTNode* mainFunc = new ASTNode(MAIN_FUNCTION_NODE);
        
        while (currentToken().type != RBRACE && currentToken().type != EOF_TOKEN) {
            ASTNode* stmt = parseStatement();
            mainFunc->children.push_back(stmt);
        }
        
        expect(RBRACE);
        program->children.push_back(mainFunc);
        
        return program;
    }
};

// Value type for evaluator
struct Value {
    enum Type { INT, STRING } type;
    int intValue = 0;
    std::string stringValue;
    bool uninitWarn = false;
    Value(int i) : type(INT), intValue(i) {}
    Value(const std::string& s) : type(STRING), stringValue(s) {}

    Value(ASTNode* n) {
        switch (n->type) {
            case NUMBER_NODE:
                type = INT;
                intValue = std::stoi(n->value);  // convert from string to int
                break;
            case STRING_NODE:
                type = STRING;
                stringValue = n->value;
                break;
            default:
                throw std::runtime_error("ASTNode type not convertible to Value");
        }
    }
    Value() {
	    type=INT;
	    intValue = 0;
	    // std::cerr << "Warning: variable '" << node->value << "' declared without init (default to 0)\n";
    }
};


// Evaluator class
class Evaluator {
private:
    std::map<std::string, Value> variables;
    
public:
    Value evaluate(ASTNode* node) {
        switch (node->type) {
            case PROGRAM_NODE: {
                if (!node->children.empty()) {
                    return evaluate(node->children[0]);
                }
                return Value(0);
            }
                
            case MAIN_FUNCTION_NODE: {
                Value result(0);
                for (auto child : node->children) {
                    result = evaluate(child);
                }
                return result;
            }
            
            case BLOCK_NODE: {
                Value result(0);
                for (auto child : node->children) {
                    result = evaluate(child);
                }
                return result;
            }
            
            case VARIABLE_DECL_NODE: {
                if (!node->children.empty()) {
                    Value value = evaluate(node->children[0]);
                    variables[node->value] = value;
                    return value;
                } else {
		    std::cerr << "Warning: variable '" << node->value <<"' declared without initialization (defaulting to 0)\n";
		    variables[node->value] = Value(0);
                    return Value(0);
                }
            }
            
            case ASSIGNMENT_NODE: {
                Value value = evaluate(node->children[0]);
                variables[node->value] = value;
                return value;
            }
            
            case VARIABLE_NODE: {
                if (variables.find(node->value) == variables.end()) {
                    throw std::runtime_error("Undefined variable: " + node->value);
                }
                return variables[node->value];
            }
            
            case IF_NODE: {
                Value condition = evaluate(node->children[0]);
                if (condition.type != Value::INT) {
                    throw std::runtime_error("If condition must be integer");
                }
                
                if (condition.intValue != 0) {
                    return evaluate(node->children[1]);
                } else if (node->children.size() > 2) {
                    return evaluate(node->children[2]);
                }
                return Value(0);
            }
            
            case WHILE_NODE: {
                Value result(0);
                while (true) {
                    Value condition = evaluate(node->children[0]);
                    if (condition.type != Value::INT || condition.intValue == 0) {
                        break;
                    }
                    result = evaluate(node->children[1]);
                }
                return result;
            }
            
            case COMPARISON_NODE: {
                Value left = evaluate(node->children[0]);
                Value right = evaluate(node->children[1]);
                
                if (left.type != Value::INT || right.type != Value::INT) {
                    throw std::runtime_error("Comparison operations only supported on integers");
                }
                
                if (node->value == "==") return Value(left.intValue == right.intValue ? 1 : 0);
                if (node->value == "!=") return Value(left.intValue != right.intValue ? 1 : 0);
                if (node->value == "<") return Value(left.intValue < right.intValue ? 1 : 0);
                if (node->value == ">") return Value(left.intValue > right.intValue ? 1 : 0);
		if (node->value == "<=") return Value(left.intValue <= right.intValue ? 1 : 0);
		if (node->value == ">=") return Value(left.intValue >= right.intValue ? 1 : 0);
                
                throw std::runtime_error("Unknown comparison operator: " + node->value);
            }
                
            case ARITHMETIC_NODE: {
                Value left = evaluate(node->children[0]);
                Value right = evaluate(node->children[1]);
                
                if (left.type != Value::INT || right.type != Value::INT) {
                    throw std::runtime_error("Arithmetic operations only supported on numbers");
                }
                
                if (node->value == "+") return Value(left.intValue + right.intValue);
                if (node->value == "-") return Value(left.intValue - right.intValue);
                if (node->value == "*") return Value(left.intValue * right.intValue);
                if (node->value == "/") {
                    if (right.intValue == 0) throw std::runtime_error("Division by zero");
                    return Value(left.intValue / right.intValue);
                }
                throw std::runtime_error("Unknown arithmetic operator: " + node->value);
            }
                
            case NUMBER_NODE: {
                return Value(std::stoi(node->value));
            }
            
            case STRING_NODE: {
                return Value(node->value);
            }
            
            case FUNCTION_CALL_NODE: {
                if (node->value == "print") {
                    if (node->children.size() != 1) {
                        throw std::runtime_error("print() function expects exactly 1 argument");
                    }
                    Value value = evaluate(node->children[0]);
                    if (value.type == Value::INT) {
                        std::cout << value.intValue;
                    } else {
                        std::cout << value.stringValue;
                    }
                    return value;
                } else if (node->value == "compile") {
                    if (node->children.size() != 1) {
                        throw std::runtime_error("compile() function expects exactly 1 argument");
                    }
                    Value filename = evaluate(node->children[0]);
                    if (filename.type != Value::STRING) {
                        throw std::runtime_error("compile() function expects string argument");
                    }
                    
                    // Read source file
                    std::ifstream file(filename.stringValue);
                    if (!file.is_open()) {
                        throw std::runtime_error("Could not open file: " + filename.stringValue);
                    }
                    
                    std::string sourceCode;
                    std::string line;
                    while (std::getline(file, line)) {
                        sourceCode += line + "\n";
                    }
                    file.close();
                    
                    // Compile to C++
                    Lexer lexer(sourceCode);
                    std::vector<Token> tokens = lexer.tokenize();
                    Parser parser(tokens);
                    ASTNode* ast = parser.parse();
                    
                    // Generate C++ code
                    std::string cppCode = generateCppCode(ast);
                    
                    // Write to output file
                    std::string outputName = filename.stringValue;
                    size_t dotPos = outputName.find_last_of('.');
                    if (dotPos != std::string::npos) {
                        outputName = outputName.substr(0, dotPos);
                    }
                    outputName += ".cpp";
                    
                    std::ofstream outFile(outputName);
                    outFile << cppCode;
                    outFile.close();
                    
                    std::cout << "Compiled " << filename.stringValue << " to " << outputName << std::endl;
                    
                    delete ast;
                    return Value(0);
                } else {
                    throw std::runtime_error("Unknown function: " + node->value);
                }
            }
            
            case RETURN_NODE: {
                if (!node->children.empty()) {
                    return evaluate(node->children[0]);
                }
                return Value(0);
            }
                
            default: {
                throw std::runtime_error("Unknown node type");
            }
        }
    }
    
private:
    std::string generateCppCode(ASTNode* node) {
        std::stringstream cpp;
        
        cpp << "#include <iostream>\n";
        cpp << "#include <string>\n\n";
        cpp << "int main() {\n";
        
        if (node->type == PROGRAM_NODE && !node->children.empty()) {
            ASTNode* mainFunc = node->children[0];
            if (mainFunc->type == MAIN_FUNCTION_NODE) {
                for (auto child : mainFunc->children) {
                    cpp << "    " << generateStatementCode(child) << "\n";
                }
            }
        }
        
        cpp << "    return 0;\n";
        cpp << "}\n";
        
        return cpp.str();
    }
    
    std::string generateStatementCode(ASTNode* node) {
        switch (node->type) {
            case VARIABLE_DECL_NODE: {
                std::string code = "int " + node->value;
                if (!node->children.empty()) {
                    code += " = " + generateExpressionCode(node->children[0]);
                }
                return code + ";";
            }
            
            case ASSIGNMENT_NODE: {
                return node->value + " = " + generateExpressionCode(node->children[0]) + ";";
            }
            
            case FUNCTION_CALL_NODE: {
                if (node->value == "print") {
                    return "std::cout << " + generateExpressionCode(node->children[0]) + ";";
                }
                return node->value + "();";
            }
            
            default: {
                return generateExpressionCode(node) + ";";
            }
        }
    }
    
    std::string generateExpressionCode(ASTNode* node) {
        switch (node->type) {
            case NUMBER_NODE: {
                return node->value;
            }
            
            case STRING_NODE: {
                return "\"" + node->value + "\"";
            }
            
            case VARIABLE_NODE: {
                return node->value;
            }
            
            case ARITHMETIC_NODE: {
                return "(" + generateExpressionCode(node->children[0]) + " " + 
                       node->value + " " + generateExpressionCode(node->children[1]) + ")";
            }
            
            case FUNCTION_CALL_NODE: {
                if (node->value == "print") {
                    return "std::cout << " + generateExpressionCode(node->children[0]);
                }
                return node->value + "()";
            }
            
            default: {
                return "0";
            }
        }
    }
};

// Main compiler function
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file>" << std::endl;
        return 1;
    }
    
    std::string filename = argv[1];
    std::string sourceCode;
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'" << std::endl;
        return 1;
    }
    
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
        
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();
        
        Parser parser(tokens);
        ASTNode* ast = parser.parse();
        
        Evaluator evaluator;
        Value result = evaluator.evaluate(ast);
        
        delete ast;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
