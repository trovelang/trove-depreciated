#pragma once

#include <types.h>

namespace trove {
    class SourcePosition {
	public:
		SourcePosition() {}
		SourcePosition(u32 index_start, u32 index_end, u32 line_start, u32 line_end)
			: index_start(index_start), index_end(index_end), line_start(line_start), line_end(line_end) {}

		SourcePosition merge(SourcePosition other) {
			SourcePosition merged;
			merged.index_start = index_start < other.index_start ? index_start : other.index_start;
			merged.index_end = index_end > other.index_end ? index_end : other.index_end;
			merged.line_start = line_start < other.line_start ? line_start : other.line_start;
			merged.line_end = line_end > other.line_end ? line_end : other.line_end;
			return merged;
		}

	private:
		u32 index_start = 0;
		u32 index_end = 0;
		u32 line_start = 0;
		u32 line_end = 0;
	};
}