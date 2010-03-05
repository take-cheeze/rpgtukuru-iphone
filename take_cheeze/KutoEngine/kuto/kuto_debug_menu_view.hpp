#include <QRect>
#include <QtOpenGL/QGLWidget>

namespace kuto
{

class DebugMenuView : public QGLWidget
{
private:
/*
	UITableView*		tableView_;
 */
	const char*			selectedSectionName_;
	QRect				frame_;
public:
	// static DebugMenuView* initWithFrame(QRect rect);
	DebugMenuView(QWidget* parent);
	const char*& selectedSection() { return selectedSectionName_; }

	~DebugMenuView();
protected:
/*
	virtual void initializeGL();
	virtual void paintGL();
	virtual void resizeGL(int width, int height);
 */
};

/*
- (id)initWithFrame:(CGRect)frame;

@property (readonly, assign) UITableView* tableView;
@property (nonatomic, assign) const char* selectedSectionName;

@end

*/

};
