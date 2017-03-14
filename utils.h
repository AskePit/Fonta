#ifndef UTILS_H
#define UTILS_H

#include "types.h"
#include <limits>
#include <QFont>

inline QFont& mut_font(const QFont& f) { return const_cast<QFont&>(f); }

int getDPI();
inline float pt2px(float pt) { return pt*getDPI()/72.; }
inline float inf() { return std::numeric_limits<float>::infinity(); }

int callQuestionDialog(CStringRef message);
void callInfoDialog(CStringRef message);

#endif // UTILS_H
