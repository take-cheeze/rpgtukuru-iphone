#include <QRect>


class KutoDebugMenuView // : UIViewController <UITableViewDelegate, UITableViewDataSource>
{
private:
/*
	UITableView*		tableView_;
	const char*			selectedSectionName_;
 */
	QRect				frame_;
public:
	static KutoDebugMenuView* initWithFrame(QRect rect);

	~KutoDebugMenuView();
};

/*
- (id)initWithFrame:(CGRect)frame;

@property (readonly, assign) UITableView* tableView;
@property (nonatomic, assign) const char* selectedSectionName;

@end

*/

