#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QStatusBar>
#include <QtCore/QProcess>
#include <QtCore/QTimer>
#include <QtCore/QDir>
#include <QtGui/QFont>
#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtGui/QTextCursor>
#include <QtGui/QKeyEvent>
#include <QtCore/QStandardPaths>
#include <cmath>

class FlowchartWidget : public QWidget {
    Q_OBJECT

public:
    FlowchartWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setMinimumSize(600, 400);
        setStyleSheet("background-color: white; border: 1px solid #ccc;");
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // Draw flowchart for compiler process
        drawCompilerFlowchart(painter);
    }

private:
    void drawCompilerFlowchart(QPainter &painter) {
        // Colors
        QColor boxColor(135, 206, 235); // Sky blue
        QColor arrowColor(70, 130, 180); // Steel blue
        QColor textColor(25, 25, 112); // Midnight blue
        
        // Fonts
        QFont titleFont("Arial", 10, QFont::Bold);
        QFont textFont("Arial", 9);
        
        // Box dimensions
        int boxWidth = 120;
        int boxHeight = 60;
        int spacing = 80;
        int startX = 50;
        int startY = 20;
        
        // Helper function to draw rounded rectangle with text
        auto drawBox = [&](int x, int y, const QString &text, QColor color = QColor()) {
            if (!color.isValid()) {
                color = boxColor;
            }
            painter.setPen(QPen(color.darker(), 2));
            painter.setBrush(QBrush(color));
            painter.drawRoundedRect(x, y, boxWidth, boxHeight, 10, 10);
            
            painter.setPen(textColor);
            painter.setFont(textFont);
            painter.drawText(QRect(x, y, boxWidth, boxHeight), Qt::AlignCenter | Qt::TextWordWrap, text);
        };
        
        // Helper function to draw arrow
        auto drawArrow = [&](int x1, int y1, int x2, int y2) {
            painter.setPen(QPen(arrowColor, 2));
            painter.drawLine(x1, y1, x2, y2);
            
            // Draw arrowhead
            double angle = atan2(y2 - y1, x2 - x1);
            int arrowLength = 10;
            painter.drawLine(x2, y2, 
                           x2 - arrowLength * cos(angle - M_PI/6),
                           y2 - arrowLength * sin(angle - M_PI/6));
            painter.drawLine(x2, y2,
                           x2 - arrowLength * cos(angle + M_PI/6),
                           y2 - arrowLength * sin(angle + M_PI/6));
        };
        
        // Draw title
        painter.setPen(textColor);
        painter.setFont(titleFont);
        painter.drawText(10, 10, "NPAVC Compiler Process Flow");
        
        // Step 1: Source Code
        drawBox(startX, startY + 40, "Source Code\n(.npavc file)", QColor(255, 182, 193));
        
        // Step 2: Lexical Analysis
        drawBox(startX, startY + 40 + spacing, "Lexical Analysis\n(Tokenization)", boxColor);
        drawArrow(startX + boxWidth/2, startY + 40 + boxHeight, 
                 startX + boxWidth/2, startY + 40 + spacing);
        
        // Step 3: Syntax Analysis
        drawBox(startX, startY + 40 + spacing * 2, "Syntax Analysis\n(Parsing)", boxColor);
        drawArrow(startX + boxWidth/2, startY + 40 + spacing + boxHeight, 
                 startX + boxWidth/2, startY + 40 + spacing * 2);
        
        // Step 4: AST Generation
        drawBox(startX + boxWidth + 30, startY + 40 + spacing * 2, "AST Generation\n(Parse Tree)", boxColor);
        drawArrow(startX + boxWidth, startY + 40 + spacing * 2 + boxHeight/2, 
                 startX + boxWidth + 30, startY + 40 + spacing * 2 + boxHeight/2);
        
        // Step 5: Interpretation or Compilation
        drawBox(startX + boxWidth + 30, startY + 40 + spacing, "Evaluation\n(Interpreter)", QColor(144, 238, 144));
        drawArrow(startX + boxWidth + 30 + boxWidth/2, startY + 40 + spacing * 2, 
                 startX + boxWidth + 30 + boxWidth/2, startY + 40 + spacing + boxHeight);
        
        // Step 6: Code Generation (if compiling)
        drawBox(startX + (boxWidth + 30) * 2, startY + 40 + spacing * 2, "C++ Code\nGeneration", QColor(255, 165, 0));
        drawArrow(startX + boxWidth + 30 + boxWidth, startY + 40 + spacing * 2 + boxHeight/2, 
                 startX + (boxWidth + 30) * 2, startY + 40 + spacing * 2 + boxHeight/2);
        
        // Step 7: Binary Output
        drawBox(startX + (boxWidth + 30) * 2, startY + 40 + spacing, "Binary\nExecutable", QColor(255, 182, 193));
        drawArrow(startX + (boxWidth + 30) * 2 + boxWidth/2, startY + 40 + spacing * 2, 
                 startX + (boxWidth + 30) * 2 + boxWidth/2, startY + 40 + spacing + boxHeight);
        
        // Add legend
        painter.setPen(textColor);
        painter.setFont(textFont);
        painter.drawText(10, height() - 80, "Legend:");
        
        // Legend items
        painter.setBrush(QBrush(QColor(255, 182, 193)));
        painter.drawRect(10, height() - 60, 15, 15);
        painter.drawText(30, height() - 48, "Input/Output");
        
        painter.setBrush(QBrush(boxColor));
        painter.drawRect(10, height() - 40, 15, 15);
        painter.drawText(30, height() - 28, "Processing");
        
        painter.setBrush(QBrush(QColor(144, 238, 144)));
        painter.drawRect(10, height() - 20, 15, 15);
        painter.drawText(30, height() - 8, "Execution");
    }
};

class ShellEmulator : public QWidget {
    Q_OBJECT

public:
    ShellEmulator(QWidget *parent = nullptr) : QWidget(parent) {
        setupUI();
        setupProcess();
        
        // Set initial directory
        currentDir = QDir::currentPath();
        updatePrompt();
        
	installEventFilter(this);

        // Set focus to command input
        commandInput->setFocus();
        
        // Force a repaint to ensure text is visible
        commandInput->repaint();
        
        QTimer::singleShot(100, this, [this]() {
            commandInput->setFocus();
            commandInput->setCursorPosition(0);
        });
    }

protected:
    void showEvent(QShowEvent *event) override {
        QWidget::showEvent(event);
        // Ensure focus is set when the widget is shown
        QTimer::singleShot(50, this, [this]() {
            commandInput->setFocus();
            commandInput->setCursorPosition(0);
            commandInput->repaint();
        });
    }

    void keyPressEvent(QKeyEvent *event) override {
        if (!commandInput->hasFocus()) {
            commandInput->setFocus();
        }
        QWidget::keyPressEvent(event); 
    }

    void mousePressEvent(QMouseEvent *event) override {
        // Set focus to command input when clicking anywhere in the shell
        commandInput->setFocus();
        QWidget::mousePressEvent(event);
    }
    bool eventFilter(QObject *obj, QEvent *event) override {
	    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        
        // Handle history navigation first
        if (keyEvent->key() == Qt::Key_Up) {
            if (historyIndex > 0) {
                historyIndex--;
                if (historyIndex < commandHistory.size()) {
                    commandInput->setText(commandHistory[historyIndex]);
                }
            }
            return true; // Event handled
        } else if (keyEvent->key() == Qt::Key_Down) {
            if (historyIndex < commandHistory.size() - 1) {
                historyIndex++;
                commandInput->setText(commandHistory[historyIndex]);
            } else if (historyIndex == commandHistory.size() - 1) {
                historyIndex++;
                commandInput->clear();
            }
            return true; // Event handled
        }
        
        // For all other keys, ensure focus is on command input
        if (!commandInput->hasFocus()) {
            commandInput->setFocus();
        }
        
        // Let the command input handle the key event
        if (obj != commandInput) {
            QApplication::sendEvent(commandInput, event);
            return true; // Event handled
        }
    }
    
    return QWidget::eventFilter(obj, event);
}

private slots:
    void executeCommand() {
        QString command = commandInput->text().trimmed();
        if (command.isEmpty()) return;
        
        // Add command to history
        commandHistory.append(command);
        historyIndex = commandHistory.size();
        
        // Display command in output
        outputArea->append(prompt + command);
        
        // Handle built-in commands
        if (command == "clear") {
            outputArea->clear();
            updatePrompt();
        } else if (command.startsWith("cd ")) {
            QString path = command.mid(3).trimmed();
            if (path.isEmpty()) path = QDir::homePath();
            
            QDir dir(path);
            if (dir.exists()) {
                currentDir = dir.absolutePath();
                QDir::setCurrent(currentDir);
                updatePrompt();
            } else {
                outputArea->append("cd: " + path + ": No such file or directory");
            }
        } else if (command == "pwd") {
            outputArea->append(currentDir);
        } else if (command == "ls" || command == "dir") {
            QDir dir(currentDir);
            QStringList entries = dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
            for (const QString &entry : entries) {
                QFileInfo info(dir.absoluteFilePath(entry));
                QString line = info.isDir() ? "[DIR]  " : "[FILE] ";
                line += entry;
                outputArea->append(line);
            }
        } else if (command == "help") {
            showHelp();
        } else {
            // Execute external command
            executeExternalCommand(command);
        }
        
        commandInput->clear();
        commandInput->setFocus(); // Maintain focus after execution
    }
    
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitStatus == QProcess::CrashExit) {
            outputArea->append("Process crashed");
        } else {
            outputArea->append("Process finished with exit code: " + QString::number(exitCode));
        }
        updatePrompt();
        commandInput->setFocus(); // Restore focus after process finishes
    }
    
    void processError(QProcess::ProcessError error) {
        QString errorString;
        switch (error) {
            case QProcess::FailedToStart:
                errorString = "Failed to start process";
                break;
            case QProcess::Crashed:
                errorString = "Process crashed";
                break;
            case QProcess::Timedout:
                errorString = "Process timed out";
                break;
            case QProcess::ReadError:
                errorString = "Read error";
                break;
            case QProcess::WriteError:
                errorString = "Write error";
                break;
            default:
                errorString = "Unknown error";
        }
        outputArea->append("Error: " + errorString);
        updatePrompt();
        commandInput->setFocus(); // Restore focus after error
    }
    
    void readProcessOutput() {
        QByteArray data = process->readAllStandardOutput();
        outputArea->append(QString::fromLocal8Bit(data));
    }
    
    void readProcessError() {
        QByteArray data = process->readAllStandardError();
        outputArea->append(QString::fromLocal8Bit(data));
    }

private:
    void setupUI() {
        setLayout(new QVBoxLayout);
    
    // Set focus policy to accept focus
    setFocusPolicy(Qt::StrongFocus);
    
    // Output area
    outputArea = new QTextEdit;
    outputArea->setReadOnly(true);
    outputArea->setFont(QFont("Consolas", 10));
    outputArea->setStyleSheet("background-color: #1e1e1e; color: #ffffff; border: 1px solid #555;");
    outputArea->setFocusPolicy(Qt::NoFocus); // Don't steal focus from command input
    
    // Input area
    QHBoxLayout *inputLayout = new QHBoxLayout;
    
    promptLabel = new QLabel("$");
    promptLabel->setFont(QFont("Consolas", 10));
    promptLabel->setStyleSheet("color: #00ff00; font-weight: bold;");
    
    commandInput = new QLineEdit;
    commandInput->setFont(QFont("Consolas", 10));
    commandInput->setStyleSheet(
        "QLineEdit {"
        "    background-color: #2d2d2d;"
        "    color: #ffffff;"
        "    border: 1px solid #555;"
        "    padding: 2px;"
        "    selection-background-color: #0078d4;"
        "    selection-color: #ffffff;"
        "}"
        "QLineEdit:focus {"
        "    border: 2px solid #0078d4;"
        "    background-color: #3d3d3d;"
        "}"
    );
    commandInput->setFocusPolicy(Qt::StrongFocus);
    
    // IMPORTANT: Accept focus for the whole widget
    commandInput->setFocus();
    
    executeButton = new QPushButton("Execute");
    executeButton->setStyleSheet("background-color: #0078d4; color: white; border: none; padding: 5px 10px;");
    executeButton->setFocusPolicy(Qt::NoFocus); // Don't steal focus from command input
    
    inputLayout->addWidget(promptLabel);
    inputLayout->addWidget(commandInput);
    inputLayout->addWidget(executeButton);
    
    layout()->addWidget(outputArea);
    layout()->addItem(inputLayout);
    
    // Connect signals
    connect(commandInput, &QLineEdit::returnPressed, this, &ShellEmulator::executeCommand);
    connect(executeButton, &QPushButton::clicked, this, &ShellEmulator::executeCommand);
    
    // Initial welcome message
    outputArea->append("NPAVC Compiler Shell Emulator");
    outputArea->append("Type 'help' for available commands");
    outputArea->append("Type 'npavc <filename>' to compile/interpret NPAVC files");
    outputArea->append("Use Up/Down arrows for command history");
    outputArea->append("");
    }
    
    void setupProcess() {
        process = new QProcess(this);
        connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &ShellEmulator::processFinished);
        connect(process, &QProcess::errorOccurred, this, &ShellEmulator::processError);
        connect(process, &QProcess::readyReadStandardOutput, this, &ShellEmulator::readProcessOutput);
        connect(process, &QProcess::readyReadStandardError, this, &ShellEmulator::readProcessError);
    }
    
    void executeExternalCommand(const QString &command) {
        QStringList args = command.split(' ', Qt::SkipEmptyParts);
        if (args.isEmpty()) return;
        
        QString program = args.takeFirst();
        
        process->setWorkingDirectory(currentDir);
        process->start(program, args);
        
        if (!process->waitForStarted()) {
            outputArea->append("Command not found: " + program);
            updatePrompt();
        }
    }
    
    void updatePrompt() {
        QDir dir(currentDir);
        prompt = QString("[%1] $ ").arg(dir.dirName());
        promptLabel->setText(prompt);
        
        // Scroll to bottom
        QTextCursor cursor = outputArea->textCursor();
        cursor.movePosition(QTextCursor::End);
        outputArea->setTextCursor(cursor);
    }
    
    void showHelp() {
        outputArea->append("Available commands:");
        outputArea->append("  npavc <file>        - Interpret NPAVC source file");
        outputArea->append("  npavc <file> -c     - Compile NPAVC to executable");
        outputArea->append("  npavc <file> -c -o <name> - Compile with custom output name");
        outputArea->append("  ls, dir             - List directory contents");
        outputArea->append("  cd <path>           - Change directory");
        outputArea->append("  pwd                 - Print current directory");
        outputArea->append("  clear               - Clear screen");
        outputArea->append("  help                - Show this help");
        outputArea->append("");
        outputArea->append("Keyboard shortcuts:");
        outputArea->append("  Up/Down arrows      - Navigate command history");
        outputArea->append("  Enter               - Execute command");
        outputArea->append("");
        outputArea->append("NPAVC Language Features:");
        outputArea->append("  - C-like syntax with void main() entry point");
        outputArea->append("  - Integer variables and arithmetic");
        outputArea->append("  - String literals and print() function");
        outputArea->append("  - Control flow: if/else, while loops");
        outputArea->append("  - Comments: // and /* */");
        outputArea->append("");
    }
    
    QTextEdit *outputArea;
    QLineEdit *commandInput;
    QPushButton *executeButton;
    QLabel *promptLabel;
    QProcess *process;
    QString currentDir;
    QString prompt;
    QStringList commandHistory;
    int historyIndex = 0;
};

class CompilerExplanation : public QWidget {
    Q_OBJECT

public:
    CompilerExplanation(QWidget *parent = nullptr) : QWidget(parent) {
        setupUI();
    }

private:
    void setupUI() {
        setLayout(new QVBoxLayout);
        
        QScrollArea *scrollArea = new QScrollArea;
        QWidget *contentWidget = new QWidget;
        QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
        
        // Title
        QLabel *titleLabel = new QLabel("NPAVC Compiler Architecture");
        titleLabel->setFont(QFont("Arial", 16, QFont::Bold));
        titleLabel->setStyleSheet("color: #2c3e50; margin: 10px;");
        contentLayout->addWidget(titleLabel);
        
        // Overview
        addSection(contentLayout, "Overview", 
                   "NPAVC (Not Particularly Advanced Virtual Computer) is a simple programming language "
                   "compiler that demonstrates the fundamental concepts of language implementation. "
                   "It supports both interpretation and compilation to C++.");
        
        // Lexical Analysis
        addSection(contentLayout, "1. Lexical Analysis (Tokenization)", 
                   "The lexer converts the source code into a stream of tokens. Each token represents "
                   "a meaningful unit like keywords, identifiers, operators, or literals.\n\n"
                   "Key components:\n"
                   "• Character-by-character scanning\n"
                   "• Token classification (keywords, identifiers, operators)\n"
                   "• String and number literal parsing\n"
                   "• Comment handling (// and /* */)\n"
                   "• Error reporting with line/column information");
        
        // Syntax Analysis
        addSection(contentLayout, "2. Syntax Analysis (Parsing)", 
                   "The parser takes the token stream and builds an Abstract Syntax Tree (AST) "
                   "according to the language grammar.\n\n"
                   "Parser features:\n"
                   "• Recursive descent parsing\n"
                   "• Operator precedence handling\n"
                   "• Expression parsing (arithmetic, comparison)\n"
                   "• Statement parsing (assignments, control flow)\n"
                   "• Error recovery and reporting");
        
        // AST Generation
        addSection(contentLayout, "3. AST (Abstract Syntax Tree) Generation", 
                   "The AST represents the hierarchical structure of the program, abstracting away "
                   "concrete syntax details.\n\n"
                   "AST Node types:\n"
                   "• Program and function nodes\n"
                   "• Expression nodes (arithmetic, comparison)\n"
                   "• Statement nodes (assignments, control flow)\n"
                   "• Literal nodes (numbers, strings)\n"
                   "• Variable and function call nodes");
        
        // Evaluation
        addSection(contentLayout, "4. Evaluation (Interpretation)", 
                   "The evaluator traverses the AST and executes the program directly.\n\n"
                   "Evaluation features:\n"
                   "• Tree-walking interpreter\n"
                   "• Variable symbol table management\n"
                   "• Runtime type checking\n"
                   "• Built-in function handling (print, compile)\n"
                   "• Control flow execution");
        
        // Code Generation
        addSection(contentLayout, "5. Code Generation (Compilation)", 
                   "When compiling, the system generates equivalent C++ code from the AST.\n\n"
                   "Code generation process:\n"
                   "• AST traversal for code emission\n"
                   "• C++ syntax generation\n"
                   "• Type mapping (int, string)\n"
                   "• Function call translation\n"
                   "• Output to .cpp file");
        
        // Language Features
        addSection(contentLayout, "Language Features", 
                   "NPAVC supports a subset of C-like features:\n\n"
                   "• Data types: int, string literals\n"
                   "• Operators: +, -, *, /, ==, !=, <, >, <=, >=\n"
                   "• Control flow: if/else, while loops\n"
                   "• Functions: print() for output\n"
                   "• Comments: // single-line, /* multi-line */\n"
                   "• Variables: declaration and assignment");
        
        // Usage Examples
        addSection(contentLayout, "Usage Examples", 
                   "Example NPAVC program:\n\n"
                   "void main() {\n"
                   "    int x = 10;\n"
                   "    int y = 20;\n"
                   "    print(\"Sum: \");\n"
                   "    print(x + y);\n"
                   "    \n"
                   "    if (x < y) {\n"
                   "        print(\"x is less than y\");\n"
                   "    }\n"
                   "}\n\n"
                   "Commands:\n"
                   "• npavc program.npavc - Interpret\n"
                   "• npavc program.npavc -c - Compile to executable\n"
                   "• npavc program.npavc -c -o myprogram - Custom output name");
        
        scrollArea->setWidget(contentWidget);
        scrollArea->setWidgetResizable(true);
        layout()->addWidget(scrollArea);
    }
    
    void addSection(QVBoxLayout *layout, const QString &title, const QString &content) {
        QGroupBox *groupBox = new QGroupBox(title);
        groupBox->setStyleSheet("QGroupBox { font-weight: bold; font-size: 12pt; color: #34495e; }");
        
        QVBoxLayout *sectionLayout = new QVBoxLayout(groupBox);
        
        QLabel *contentLabel = new QLabel(content);
        contentLabel->setWordWrap(true);
        contentLabel->setStyleSheet("font-size: 10pt; color: #2c3e50; line-height: 1.4;");
        contentLabel->setFont(QFont("Arial", 9));
        
        sectionLayout->addWidget(contentLabel);
        layout->addWidget(groupBox);
    }
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setWindowTitle("NPAVC Compiler - Interactive Development Environment");
        setMinimumSize(1000, 700);
        resize(1200, 800);
    }

private slots:
    void onTabChanged(int index) {
        // When switching to shell tab, ensure it gets focus
        if (index == 0) { // Shell tab
            QTimer::singleShot(50, this, [this]() {
                if (shellEmulator) {
                    shellEmulator->setFocus();
                }
            });
        }
    }

private:
    void setupUI() {
        QWidget *centralWidget = new QWidget;
        setCentralWidget(centralWidget);
        
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        
        // Title
        QLabel *titleLabel = new QLabel("NPAVC Compiler IDE");
        titleLabel->setFont(QFont("Arial", 18, QFont::Bold));
        titleLabel->setStyleSheet("color: #2c3e50; margin: 10px; text-align: center;");
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);
        
        // Create tab widget
        QTabWidget *tabWidget = new QTabWidget;
        connect(tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
        
        // Shell tab
        shellEmulator = new ShellEmulator;
        tabWidget->addTab(shellEmulator, "Shell Emulator");
        
        // Explanation tab
        CompilerExplanation *explanation = new CompilerExplanation;
        tabWidget->addTab(explanation, "How It Works");
        
        // Flowchart tab
        FlowchartWidget *flowchart = new FlowchartWidget;
        QScrollArea *flowchartScroll = new QScrollArea;
        flowchartScroll->setWidget(flowchart);
        flowchartScroll->setWidgetResizable(true);
        tabWidget->addTab(flowchartScroll, "Process Flow");
        
        mainLayout->addWidget(tabWidget);
        
        // Status bar
        statusBar()->showMessage("Ready - Use the Shell tab to run NPAVC commands");
    }
    
    ShellEmulator *shellEmulator;
};

#include "npavGui.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
