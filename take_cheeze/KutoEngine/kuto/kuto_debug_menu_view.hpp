#include <QRect>

namespace kuto
{

class DebugMenuView // : UIViewController <UITableViewDelegate, UITableViewDataSource>
{
private:
/*
	UITableView*		tableView_;
	const char*			selectedSectionName_;
 */
	QRect				frame_;
public:
	static DebugMenuView* initWithFrame(QRect rect);

	~DebugMenuView();
};

/*
- (id)initWithFrame:(CGRect)frame;

@property (readonly, assign) UITableView* tableView;
@property (nonatomic, assign) const char* selectedSectionName;

@end

*/

};
