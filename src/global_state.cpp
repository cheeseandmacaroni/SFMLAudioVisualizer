#include <string>
#include <vector>

class GlobalState
{
public:
	enum state { playing, pause };
	state curState = pause;
	std::string visualizerName = "basic";
	int currentSongIndex = 0;
	bool allListsFolded = true;
	const std::vector<std::string>& getPaths() { return paths; }
	const std::vector<std::string>& getNames() { return names; }
	void init(const std::vector<std::string> &paths, const std::vector<std::string> &names)
	{
		this->names = names;
		this->paths = paths;
	}
private:
	std::vector<std::string> paths;
	std::vector<std::string> names;
};