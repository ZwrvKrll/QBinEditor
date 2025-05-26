#include "mainwindow.h"
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QPlainTextEdit>
#include <QDir>
#include <QDirIterator>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QByteArray>
#include <QRegularExpression>
#include <QFileInfo>
#include <QKeyEvent>
#include <QTimer>
#include <QTextBlock>
#include <QFont>
#include <QColor>

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent),
m_currentMatchIndex(-1)
{
    resize(800, 600);
    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);

    searchTab = new QWidget(this);
    QVBoxLayout *searchLayout = new QVBoxLayout(searchTab);
    directoryLineEdit = new QLineEdit(searchTab);
    directoryLineEdit->setPlaceholderText("Enter directory path...");
    searchLayout->addWidget(directoryLineEdit);
    patternLineEdit = new QLineEdit(searchTab);
    patternLineEdit->setPlaceholderText("Enter file name or search pattern (e.g., part of the name or *.ext)");
    searchLayout->addWidget(patternLineEdit);
    searchButton = new QPushButton("Search", searchTab);
    searchLayout->addWidget(searchButton);
    resultTableWidget = new QTableWidget(searchTab);
    resultTableWidget->setColumnCount(2);
    resultTableWidget->setHorizontalHeaderLabels(QStringList() << "Filename" << "File Directory");
    resultTableWidget->horizontalHeader()->setStretchLastSection(true);
    resultTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    resultTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    searchLayout->addWidget(resultTableWidget);
    tabWidget->addTab(searchTab, "File Search");
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::searchFiles);
    connect(resultTableWidget, &QTableWidget::cellDoubleClicked, this, &MainWindow::onTableItemDoubleClicked);

    editorTab = new QWidget(this);
    QVBoxLayout *editorLayout = new QVBoxLayout(editorTab);
    QHBoxLayout *fileLayout = new QHBoxLayout();
    filePathLineEdit = new QLineEdit(editorTab);
    filePathLineEdit->setPlaceholderText("Enter binary file path...");
    fileLayout->addWidget(filePathLineEdit);
    openButton = new QPushButton("Open", editorTab);
    fileLayout->addWidget(openButton);
    editorLayout->addLayout(fileLayout);
    formatTabWidget = new QTabWidget(editorTab);
    hexEditor = new QPlainTextEdit(editorTab);
    hexEditor->setPlaceholderText("File content in hexadecimal format...");
    hexEditor->setLineWrapMode(QPlainTextEdit::NoWrap);
    hexEditor->setFont(QFont("Courier New"));
    hexEditor->installEventFilter(this);
    formatTabWidget->addTab(hexEditor, "Hex");
    asciiEditor = new QPlainTextEdit(editorTab);
    asciiEditor->setPlaceholderText("File content in ASCII format...");
    asciiEditor->setLineWrapMode(QPlainTextEdit::NoWrap);
    asciiEditor->setFont(QFont("Courier New"));
    formatTabWidget->addTab(asciiEditor, "ASCII");
    binEditor = new QPlainTextEdit(editorTab);
    binEditor->setPlaceholderText("File content in BIN format...");
    binEditor->setLineWrapMode(QPlainTextEdit::NoWrap);
    binEditor->setFont(QFont("Courier New"));
    formatTabWidget->addTab(binEditor, "Bin");
    editorLayout->addWidget(formatTabWidget);
    QHBoxLayout *internalSearchLayout = new QHBoxLayout();
    searchLineEdit = new QLineEdit(editorTab);
    searchLineEdit->setPlaceholderText("Enter search query...");
    searchLineEdit->installEventFilter(this);
    internalSearchLayout->addWidget(searchLineEdit);
    findButton = new QPushButton("Find", editorTab);
    internalSearchLayout->addWidget(findButton);
    prevButton = new QPushButton("↑", editorTab);
    internalSearchLayout->addWidget(prevButton);
    nextButton = new QPushButton("↓", editorTab);
    internalSearchLayout->addWidget(nextButton);
    highlightAllButton = new QPushButton("Highlight All", editorTab);
    internalSearchLayout->addWidget(highlightAllButton);
    clearSearchButton = new QPushButton("Clear Search", editorTab);
    internalSearchLayout->addWidget(clearSearchButton);
    editorLayout->addLayout(internalSearchLayout);
    saveButton = new QPushButton("Save", editorTab);
    editorLayout->addWidget(saveButton);
    tabWidget->addTab(editorTab, "Binary Editor");
    connect(openButton, &QPushButton::clicked, this, &MainWindow::openBinaryFile);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveBinaryFile);
    connect(findButton, &QPushButton::clicked, this, &MainWindow::performSearch);
    connect(highlightAllButton, &QPushButton::clicked, this, &MainWindow::highlightAllMatches);
    connect(clearSearchButton, &QPushButton::clicked, this, &MainWindow::clearSearchHighlights);
    connect(prevButton, &QPushButton::clicked, this, &MainWindow::goToPreviousMatch);
    connect(nextButton, &QPushButton::clicked, this, &MainWindow::goToNextMatch);
}

MainWindow::~MainWindow()
{
}

void MainWindow::searchFiles()
{
    QString directoryPath = directoryLineEdit->text().trimmed();
    QString pattern = patternLineEdit->text().trimmed();
    if (directoryPath.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter the directory path.");
        return;
    }
    if (pattern.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter the file name or search pattern.");
        return;
    }
    QDir dir(directoryPath);
    if (!dir.exists()) {
        QMessageBox::warning(this, "Directory Error", "The specified directory does not exist.");
        return;
    }
    if (!pattern.contains("*") && !pattern.contains("?"))
        pattern = "*" + pattern + "*";
    resultTableWidget->setRowCount(0);
    int rowCount = 0;
    QDirIterator it(directoryPath, QStringList() << pattern, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString filePath = it.next();
        QFileInfo fileInfo(filePath);
        QString fileName = fileInfo.fileName();
        QString folder = fileInfo.absolutePath();
        resultTableWidget->insertRow(rowCount);
        QTableWidgetItem *nameItem = new QTableWidgetItem(fileName);
        QTableWidgetItem *folderItem = new QTableWidgetItem(folder);
        nameItem->setData(Qt::UserRole, fileInfo.absoluteFilePath());
        resultTableWidget->setItem(rowCount, 0, nameItem);
        resultTableWidget->setItem(rowCount, 1, folderItem);
        rowCount++;
    }
    if (rowCount == 0) {
        resultTableWidget->insertRow(0);
        QTableWidgetItem *nameItem = new QTableWidgetItem("No files found");
        QTableWidgetItem *folderItem = new QTableWidgetItem("");
        resultTableWidget->setItem(0, 0, nameItem);
        resultTableWidget->setItem(0, 1, folderItem);
    }
}

void MainWindow::onTableItemDoubleClicked(int row, int)
{
    QTableWidgetItem *nameItem = resultTableWidget->item(row, 0);
    if (!nameItem)
        return;
    QVariant data = nameItem->data(Qt::UserRole);
    if (!data.isValid() || data.toString().isEmpty())
        return;
    QString filePath = data.toString();
    filePathLineEdit->setText(filePath);
    tabWidget->setCurrentWidget(editorTab);
    openBinaryFile();
}

void MainWindow::openBinaryFile()
{
    QString filePath = filePathLineEdit->text().trimmed();
    if (filePath.isEmpty()) {
        filePath = QFileDialog::getOpenFileName(this, "Open Binary File", "", "All Files (*.*)");
        if (filePath.isEmpty())
            return;
        filePathLineEdit->setText(filePath);
    }
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "File Error", "Unable to open file for reading.");
        return;
    }
    QByteArray data = file.readAll();
    file.close();
    QString hexString;
    QString asciiString;
    QString binString;
    const int bytesPerLine = 32;
    int dataSize = data.size();
    for (int i = 0; i < dataSize; ++i) {
        unsigned char byte = static_cast<unsigned char>(data.at(i));
        if (i > 0 && (i % bytesPerLine) == 0)
            hexString += "\n";
        hexString += QString("%1").arg(byte, 2, 16, QLatin1Char('0')).toUpper();
        if ((i % bytesPerLine) != (bytesPerLine - 1))
            hexString += " ";
        if (i > 0 && (i % bytesPerLine) == 0)
            asciiString += "\n";
        asciiString += (byte >= 32 && byte <= 126) ? static_cast<char>(byte) : '.';
        if (i > 0 && (i % 8) == 0)
            binString += "\n";
        binString += QString("%1").arg(byte, 8, 2, QLatin1Char('0')) + " ";
    }
    hexEditor->setPlainText(hexString.trimmed());
    asciiEditor->setPlainText(asciiString.trimmed());
    binEditor->setPlainText(binString.trimmed());
}

void MainWindow::saveBinaryFile()
{
    int activeTab = formatTabWidget->currentIndex();
    QByteArray newData;
    if (activeTab == 0) {
        QString hexText = hexEditor->toPlainText();
        QString hexWithoutSpaces = hexText.remove(QRegularExpression("[\\s\\n]"));
        if (hexWithoutSpaces.length() % 2 != 0) {
            QMessageBox::warning(this, "Format Error", "Incorrect hexadecimal string length.");
            return;
        }
        for (int i = 0; i < hexWithoutSpaces.length(); i += 2) {
            bool ok;
            char byte = static_cast<char>(hexWithoutSpaces.mid(i, 2).toUShort(&ok, 16));
            if (!ok) {
                QMessageBox::warning(this, "Conversion Error", "Error converting hexadecimal value to byte.");
                return;
            }
            newData.append(byte);
        }
    }
    else if (activeTab == 1) {
        QString asciiText = asciiEditor->toPlainText().remove("\n");
        newData = asciiText.toUtf8();
    }
    else if (activeTab == 2) {
        QString binText = binEditor->toPlainText().remove("\n");
        QStringList parts = binText.split(" ", Qt::SkipEmptyParts);
        for (const QString &part : parts) {
            bool ok;
            uint byte = part.toUInt(&ok, 2);
            if (!ok) {
                QMessageBox::warning(this, "Conversion Error", "Error converting binary value.");
                return;
            }
            newData.append(static_cast<char>(byte));
        }
    }
    QString filePath = filePathLineEdit->text().trimmed();
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, "File Error", "File path not specified.");
        return;
    }
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "File Error", "Unable to open file for writing.");
        return;
    }
    file.write(newData);
    file.close();
    QMessageBox::information(this, "Success", "File saved successfully.");
}

void MainWindow::highlightSingleMatch(QPlainTextEdit *editor, const QString &query, int offset)
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    QColor color = QColor("#999999");
    QTextCursor cursor(editor->document());
    cursor.setPosition(offset);
    cursor.setPosition(offset + query.length(), QTextCursor::KeepAnchor);
    QTextEdit::ExtraSelection selection;
    selection.cursor = cursor;
    selection.format.setBackground(color);
    extraSelections.append(selection);
    editor->setExtraSelections(extraSelections);
}

void MainWindow::highlightMatches(QPlainTextEdit *editor, const QString &query)
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    QColor color = QColor("#999999");
    QString text = editor->toPlainText();
    int offset = 0;
    while (true) {
        int index = text.indexOf(query, offset, Qt::CaseInsensitive);
        if (index == -1)
            break;
        QTextCursor cursor(editor->document());
        cursor.setPosition(index);
        cursor.setPosition(index + query.length(), QTextCursor::KeepAnchor);
        QTextEdit::ExtraSelection selection;
        selection.cursor = cursor;
        selection.format.setBackground(color);
        extraSelections.append(selection);
        offset = index + query.length();
    }
    editor->setExtraSelections(extraSelections);
}

void MainWindow::performSearch()
{
    QString query = searchLineEdit->text();
    if (query.isEmpty()){
        clearSearchHighlights();
        m_foundMatchOffsets.clear();
        m_currentMatchIndex = -1;
        return;
    }
    QPlainTextEdit *activeEditor = nullptr;
    int activeTab = formatTabWidget->currentIndex();
    if (activeTab == 0)
        activeEditor = hexEditor;
    else if (activeTab == 1)
        activeEditor = asciiEditor;
    else if (activeTab == 2)
        activeEditor = binEditor;
    if (!activeEditor)
        return;
    QString text = activeEditor->toPlainText();
    m_foundMatchOffsets.clear();
    int offset = 0;
    while (true) {
        int pos = text.indexOf(query, offset, Qt::CaseInsensitive);
        if (pos == -1)
            break;
        m_foundMatchOffsets.append(pos);
        offset = pos + query.length();
    }
    if (!m_foundMatchOffsets.isEmpty()){
        m_currentMatchIndex = 0;
        highlightSingleMatch(activeEditor, query, m_foundMatchOffsets[m_currentMatchIndex]);
        QTextCursor cursor(activeEditor->document());
        cursor.setPosition(m_foundMatchOffsets[m_currentMatchIndex]);
        activeEditor->setTextCursor(cursor);
    }
    else {
        clearSearchHighlights();
    }
}

void MainWindow::clearSearchHighlights()
{
    hexEditor->setExtraSelections(QList<QTextEdit::ExtraSelection>());
    asciiEditor->setExtraSelections(QList<QTextEdit::ExtraSelection>());
    binEditor->setExtraSelections(QList<QTextEdit::ExtraSelection>());
    searchLineEdit->clear();
    m_foundMatchOffsets.clear();
    m_currentMatchIndex = -1;
}

void MainWindow::highlightAllMatches()
{
    QString query = searchLineEdit->text();
    if (query.isEmpty()){
        clearSearchHighlights();
        return;
    }
    QPlainTextEdit *activeEditor = nullptr;
    int activeTab = formatTabWidget->currentIndex();
    if (activeTab == 0)
        activeEditor = hexEditor;
    else if (activeTab == 1)
        activeEditor = asciiEditor;
    else if (activeTab == 2)
        activeEditor = binEditor;
    if (activeEditor)
        highlightMatches(activeEditor, query);
}

void MainWindow::goToNextMatch()
{
    if (m_foundMatchOffsets.isEmpty())
        return;

    int activeTab = formatTabWidget->currentIndex();
    QPlainTextEdit* activeEditor = (activeTab == 0) ? hexEditor : ((activeTab == 1) ? asciiEditor : binEditor);
    if (!activeEditor)
        return;

    m_currentMatchIndex = (m_currentMatchIndex + 1) % m_foundMatchOffsets.size();
    QString query = searchLineEdit->text();
    highlightSingleMatch(activeEditor, query, m_foundMatchOffsets[m_currentMatchIndex]);
    QTextCursor cursor(activeEditor->document());
    cursor.setPosition(m_foundMatchOffsets[m_currentMatchIndex]);
    activeEditor->setTextCursor(cursor);
}

void MainWindow::goToPreviousMatch()
{
    if (m_foundMatchOffsets.isEmpty())
        return;
    int activeTab = formatTabWidget->currentIndex();
    QPlainTextEdit* activeEditor = (activeTab == 0) ? hexEditor : ((activeTab == 1) ? asciiEditor : binEditor);
    if (!activeEditor)
        return;
    m_currentMatchIndex = (m_currentMatchIndex - 1 + m_foundMatchOffsets.size()) % m_foundMatchOffsets.size();
    QString query = searchLineEdit->text();
    highlightSingleMatch(activeEditor, query, m_foundMatchOffsets[m_currentMatchIndex]);
    QTextCursor cursor(activeEditor->document());
    cursor.setPosition(m_foundMatchOffsets[m_currentMatchIndex]);
    activeEditor->setTextCursor(cursor);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == searchLineEdit && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if(keyEvent->key() == Qt::Key_Up) {
            goToPreviousMatch();
            return true;
        } else if(keyEvent->key() == Qt::Key_Down) {
            goToNextMatch();
            return true;
        }
    }
    if (obj == hexEditor && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        QTextCursor cursor = hexEditor->textCursor();
        if (keyEvent->key() == Qt::Key_Backspace) {
            if (cursor.hasSelection()) {
                if (cursor.selectedText().contains(" "))
                    return true;
            } else if (cursor.position() > cursor.block().position()) {
                QTextCursor tmp = cursor;
                tmp.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
                if (tmp.selectedText() == " ")
                    return true;
            }
        }
        else if (keyEvent->key() == Qt::Key_Delete) {
            if (cursor.hasSelection()) {
                if (cursor.selectedText().contains(" "))
                    return true;
            } else {
                QTextCursor tmp = cursor;
                tmp.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
                if (tmp.selectedText() == " ")
                    return true;
            }
        }
        if (!keyEvent->text().isEmpty()) {
            QChar ch = keyEvent->text().at(0);
            if (ch.isDigit() || (ch.toUpper() >= 'A' && ch.toUpper() <= 'F')) {
                int col = cursor.position() - cursor.block().position();
                if ((col % 3) == 1) {
                    QTimer::singleShot(0, [this]() {
                        QTextCursor cur = hexEditor->textCursor();
                        cur.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, 1);
                        hexEditor->setTextCursor(cur);
                    });
                }
            }
        }
    }
    return QMainWindow::eventFilter(obj, event);
}
