#include <vector>
#include <utility>

using namespace std;

class Vertex {
	private:
		char label;
		bool mark;

	public:
		Vertex(char label) {
			this->label = label;
			mark = false;
		}

		char getLabel() {
			return label;
		}
		
		bool isMarked() {
			return mark;
		}

		void setMark() {
			mark = true;
		}

		void clearMark() {
			mark = false;
		}
};
