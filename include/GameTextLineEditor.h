#ifndef GAMESORTING_GAMETEXTLINEEDITOR
#define GAMESORTING_GAMETEXTLINEEDITOR

#include <QLineEdit>

class QTabBar;

class GameTextLineEditor : public QLineEdit
{
	Q_OBJECT
public:
	explicit GameTextLineEditor(int tabIndex, QTabBar* tabBar, QWidget* parent = nullptr);

signals:
	void editingCancelled();

public slots:
	void resizeToTab();

protected:
	void keyReleaseEvent(QKeyEvent* event) override;

private:
	void updateTabBarName();

	int m_tabIndex;
	QTabBar* m_tabBar;
};

#endif // GAMESORTING_GAMETEXTLINEEDITOR