#include "MusicHeader.h"

/* setter & getter */
std::string MusicHeader::getValues(int valueIdx) {
	return values[valueIdx];
}
void MusicHeader::setValues(int valueIdx, std::string val) {
	values[valueIdx] = val;
}