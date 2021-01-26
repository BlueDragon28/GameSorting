#include "MainWindow.h"
#include "CommonStruct.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QString>

int main(int argc, char** argv)
{
	// Initialize the resources of the program (images, etc).
	Q_INIT_RESOURCE(gamesorting);

	qRegisterMetaTypeStreamOperators<GameStarRating>("GameStarRating");

	// Creating an application object.
	QApplication app(argc, argv);
	app.setOrganizationName("Erwan28250");
	app.setApplicationName("GameSorting");
	app.setApplicationVersion(GAMESORTING_VERSION);
	
	// Setting the application icon.
	QIcon appIcon = QIcon(":/Images/GameSorting.ico");
	app.setWindowIcon(appIcon);

	// Parsing command line.
	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument("gameList", QCoreApplication::translate("main", "Loading a game list file. Only one file can be open."));

	QCommandLineOption noSettings("restoreSettings", QCoreApplication::translate("main", "Restore settings to default."));
	parser.addOption(noSettings);

	parser.process(app);
	
	QString gListFilePath = parser.positionalArguments().size() > 0 ?
		parser.positionalArguments().first() : QString();
	bool bNoSettings = false;
	if (parser.isSet(noSettings))
		bNoSettings = true;

	MainWindow window(gListFilePath, bNoSettings);
	window.show();

	// Running the app main loop.
	return app.exec();
}