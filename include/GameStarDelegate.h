#ifndef GAMESORTING_GAMESTARDELEGATE
#define GAMESORTING_GAMESTARDELEGATE

#include <QStyledItemDelegate>

class GameStarEditor;

class GameStarDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	GameStarDelegate(QWidget* parent = nullptr);

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void setEditorData(QWidget* editor, const QModelIndex& parent) const override;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

private slots:
	void commitAndCloseEditor(GameStarEditor* editor);
};

#endif // GAMESORTING_GAMESTARDELEGATE