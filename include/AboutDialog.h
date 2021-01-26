#ifndef GAMESORTING_ABOUTDIALOG
#define GAMESORTING_ABOUTDIALOG

#include <QDialog>

class AboutDialog : public QDialog
{
	Q_OBJECT
public:
	explicit AboutDialog(QWidget* parent = nullptr);

private:
	void setDialogParameters();
	void createAboutText();
};

#endif // GAMESORTING_ABOUTDIALOG