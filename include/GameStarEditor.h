#ifndef GAMESORTING_GAMESTAREDITOR
#define GAMESORTING_GAMESTAREDITOR

#include <QWidget>
#include <QSize>

#include "GameStarRating.h"

class GameStarEditor : public QWidget
{
	Q_OBJECT
public:
	GameStarEditor(QWidget* parent = nullptr);

	QSize sizeHint() const override;
	void setGameStarRating(const GameStarRating& gStarRating);
	GameStarRating gameStarRating();

protected:
	void paintEvent(QPaintEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

signals:
	void editFinished(GameStarEditor* editor);

private:
	int getStarNumberFromCursorPosition(int x) const;

	GameStarRating m_gStarRating;
};

#endif // GAMESORTING_GAMESTAREDITOR