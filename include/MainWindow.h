#ifndef GAMESORTING_MAINWINDOW
#define GAMESORTING_MAINWINDOW

#include <QMainWindow>
#include <QString>
#include "GameListModel.h"
#include "CommonStruct.h"

class GameTabAndList;
class QTableView;
class AboutDialog;

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget* parent = nullptr);
	MainWindow(const QString& filePath, bool noSettings = false, QWidget* parent = nullptr);
	~MainWindow();

#ifndef NDEBUG
	void addGame();
#endif // NDEBUG

public slots:
	void newGameList();
	void openGameList();
	bool saveGameList();
	bool saveAsGameList();
	void filteringGameList();

protected:
	virtual void closeEvent(QCloseEvent* e);

private slots:
	void aboutApp();

private:
	void createMenu();
	void createCentralWidget();
	void readSettings(bool readSettings = true, bool loadLastSavedFile = true);
	void writeSettings();
	bool saveFile(const QString& filePath) const;
	const TabAndListData openFile(const QString& filePath, bool& result) const;
	const TabAndListData openFileVersion002And003(QDataStream& dataStream, bool v003, bool& result) const;
	void appendGame();
	void removeGame();
	bool maybeSave();
	void setFileNameIntoWindowTitle();

	GameTabAndList* m_gTabAndList;
	QString m_gFilePath, m_lastOpenedDir;
	AboutDialog* m_aboutDialog;
};

#endif // GAMESORTING_MAINWINDOW