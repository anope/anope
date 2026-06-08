// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#pragma once

#include <iterator>
#include <utility>

namespace Anope
{
	template <typename Iterator>
	class iterator_range;

	/** Returns a range containing all elements equivalent to \p value.
	 * @param collection The collection to search within.
	 * @param value The value to search for.
	 */
	template <typename Collection, typename Value>
	auto equal_range(const Collection& collection, const Value& value)
	{
		return iterator_range(collection.equal_range(value));
	}

	/** Returns a range representing a reverse iterator for the specified colleciton.
	 * @param collection The collection to create a reverse iterator for.
	 */
	template <typename Collection>
	auto reverse_range(const Collection& collection)
	{
		return iterator_range(collection.rbegin(), collection.rend());
	}
}

/** Represents a range of iterators. */
template <typename Iterator>
class Anope::iterator_range final
{
private:
	/** An iterator which points to the start of the range. */
	const Iterator begini;

	/* An iterator which points to one past the end of the range. */
	const Iterator endi;

public:
	/** Initialises a new iterator range with the specified iterators.
	 * @param begin An iterator which points to the start of the range.
	 * @param end An iterator which points to one past the end of the range.
	 */
	explicit iterator_range(Iterator begin, Iterator end)
		: begini(begin)
		, endi(end)
	{
	}

	/** Initialises a new iterator range from a pair of iterators.
	 * @param range A pair of iterators in the format [first, last).
	 */
	explicit iterator_range(std::pair<Iterator, Iterator> range)
		: begini(range.first)
		, endi(range.second)
	{
	}

	/** Determines whether the iterator range is empty. */
	bool empty() const { return begini == endi; }

	/** Retrieves an iterator which points to the start of the range. */
	const Iterator& begin() const { return begini; }

	/** Retrieves an iterator which points to one past the end of the range. */
	const Iterator& end() const { return endi; }

	/** Retrieves the number of hops within the iterator range. */
	typename std::iterator_traits<Iterator>::difference_type count() const { return std::distance(begini, endi); }
};
