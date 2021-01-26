#ifndef GAMESORTING_GAMESTARRATING
#define GAMESORTING_GAMESTARRATING

#include <QPolygonF>
#include <QRect>
#include <QPalette>
#include <QDataStream>

class GameStarRating
{
public:
	enum class EditMode { Editable, ReadOnly };

	GameStarRating(int starCount = 1, int maxStarCount = 5, QWidget* parent = nullptr);
	GameStarRating(const GameStarRating& gameStarRating);

	void paint(QPainter* painter, const QRect& rect, const QPalette& palette, EditMode editMode) const;
	QSize sizeHint() const;
	int starCount() const;
	int maxStarCount() const;
	void setStarCount(int starCount);
	void setMaxStarCount(int maxStarCount);
	void initStarPolygon();

	bool operator<(const GameStarRating& other) const;
	bool operator>(const GameStarRating& other) const;
	bool operator<=(const GameStarRating& other) const;
	bool operator>=(const GameStarRating& other) const;

private:
	int m_starCount, m_maxStarCount, m_paintScaleFactor;
	QPolygonF m_starPolygon;
};

Q_DECLARE_METATYPE(GameStarRating)

QDataStream& operator<<(QDataStream& out, const GameStarRating& star);
QDataStream& operator>>(QDataStream& in, GameStarRating& star);

#endif // GAMESORTING_GAMESTARRATING