#ifndef GAMESORTING_LISTTABLEVIEW
#define GAMESORTING_LISTTABLEVIEW

#include "QTableView"

class ListTableView : public QTableView
{
	Q_OBJECT
public:
	ListTableView(QWidget* parent = nullptr);

protected:
	virtual void mousePressEvent(QMouseEvent* evt);
};

#endif // GAMESORTING_LISTTABLEVIEW