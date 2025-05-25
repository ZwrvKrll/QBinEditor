#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>

class QTabWidget;
class QWidget;
class QLineEdit;
class QPushButton;
class QTableWidget;
class QPlainTextEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void searchFiles();
    void onTableItemDoubleClicked(int row, int column);
    void openBinaryFile();
    void saveBinaryFile();
    void performSearch();
    void clearSearchHighlights();
    void highlightAllMatches();

private:
    QWidget *searchTab;
    QLineEdit *directoryLineEdit;
    QLineEdit *patternLineEdit;
    QPushButton *searchButton;
    QTableWidget *resultTableWidget;

    QWidget *editorTab;
    QLineEdit *filePathLineEdit;
    QPushButton *openButton;
    QPlainTextEdit *hexEditor;
    QPlainTextEdit *asciiEditor;
    QPlainTextEdit *binEditor;
    QPushButton *saveButton;

    QTabWidget *tabWidget;
    QTabWidget *formatTabWidget;

    QLineEdit *searchLineEdit;
    QPushButton *findButton;
    QPushButton *prevButton;
    QPushButton *nextButton;
    QPushButton *highlightAllButton;
    QPushButton *clearSearchButton;

    QList<int> m_foundMatchOffsets;
    int m_currentMatchIndex;

    void goToNextMatch();
    void goToPreviousMatch();
    void highlightSingleMatch(QPlainTextEdit* editor, const QString &query, int offset);
    void highlightMatches(QPlainTextEdit *editor, const QString &query);
};

#endif // MAINWINDOW_H
