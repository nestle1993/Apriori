#include "apriori.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <stdio.h>
#include <stdlib.h>

namespace sequential {

	bool is_subset(std::set<int> set1, std::set<int> set2) {
		/* check if set1 is the subset of set2 */
		std::set<int>::iterator it = set1.begin();
		for (; it != set1.end(); it ++) {
			if (set2.find(*it) == set2.end()) return false;
		}
		return true;
	}

	std::set<int> merge_set(std::set<int> set1, std::set<int> set2) {
		/* merge set1 with set2 */
		std::set<int> ret;
		ret.insert(set1.begin(), set1.end());
		ret.insert(set2.begin(), set2.end());
		return ret;
	}

	void print_freqset_info(std::map<std::set<int>, float> freqset) {
		std::map<std::set<int>, float>::iterator map_it;
		std::set<int>::iterator it;
		int freq_k;

		if (freqset.empty()) return;
		freq_k = freqset.begin()->first.size();

		std::cout << "---- freqset of k=" << freq_k << " ----\n";
		for (map_it = freqset.begin(); map_it != freqset.end(); map_it ++) {
			std::cout << "< ";
			for (it = map_it->first.begin(); it != map_it->first.end(); it ++) {
				std::cout << *it << " ";
			}
			std::cout << ">, support=" << map_it->second << std::endl;
		}
		return;
	}

	void print_candidates_info(std::set<std::set<int> > candidates) {
		std::set<std::set<int> >::iterator out_it;
		std::set<int>::iterator it;
		int freq_k;

		if (candidates.empty()) return;
		freq_k = candidates.begin()->size();

		std::cout << "---- candidates of k=" << freq_k << " ----\n";
		for (out_it = candidates.begin(); out_it != candidates.end(); out_it ++) {
			std::cout << "< ";
			for (it = out_it->begin(); it != out_it->end(); it ++) {
				std::cout << *it << " ";
			}
			std::cout << ">" << std::endl;
		}
		return;
	}

	std::map<std::set<int>, float> prune_candidates(std::set<std::set<int> >candidates,
												  	std::vector<std::set<int> > transactions,
												  	float min_support,
												  	std::map<std::set<int>, float>& ret)
	{
		std::map<std::set<int>, float> freqset;
		std::map<std::set<int>, int> set_count;
		std::map<std::set<int>, int>::iterator map_it;
		std::set<std::set<int> >::iterator it;
		int t_size = transactions.size();

		for (it = candidates.begin(); it != candidates.end(); it ++) {
			std::set<int> cand = *it;
			for (int i = 0; i < t_size; i ++) {
				// if the candidate is the subset of transactions[i],
				// then update set_count by one.
				

				// ----------------- inline function -----------------
				// bool f = true;
				// for (std::set<int>::iterator z = cand.begin(); z != cand.end(); z ++) {
				// 	if (transactions[i].find(*z) == transactions[i].end()) {
				// 		f = false;
				// 		break;
				// 	}
				// }

				// if (f) {
				// 	if (set_count.find(*it) == set_count.end()) {
				// 		set_count[*it] = 1;
				// 	} else {
				// 		set_count[*it] += 1;
				// 	}
				// }

				// ----------------- call function -----------------
				if (is_subset(cand, transactions[i])) {
					if (set_count.find(*it) == set_count.end()) {
						set_count[*it] = 1;
					} else {
						set_count[*it] += 1;
					}
				}
			}
		}

		// calculate the support value and compare it with the min_support
		for (map_it = set_count.begin(); map_it != set_count.end(); map_it ++) {
			float support_val = float(map_it->second) / t_size;
			if (support_val >= min_support) {
				freqset[map_it->first] = support_val;
				ret[map_it->first] = support_val;
			}
		}
		return freqset;
	}

	std::set<std::set<int> > generate_candidates(std::map<std::set<int>, float> freqset)
	{
		/* Generate candidate sets from freqset.
		   Go over freqset and merge any two sets from it together.
		   If the size of the merged set equals current freq_k + 1,
		   then consider it as the candidate.
		*/
		std::set<std::set<int> > candidates;
		std::map<std::set<int>, float>::iterator it1, it2;
		std::set<int> merged_set;
		unsigned int k;

		if (freqset.empty()) return candidates;
		k = freqset.begin()->first.size();

		for (it1 = freqset.begin(); it1 != freqset.end(); it1 ++) {
			for (it2 = freqset.begin(); it2 != freqset.end(); it2 ++) {
				if (it1 == it2) continue;
				merged_set = merge_set(it1->first, it2->first);
				if (merged_set.size() == k + 1) {
					candidates.insert(merged_set);
				}
			}
		}
		return candidates;
	}

	std::map<std::set<int>, float> apriori(std::vector<std::set<int> > transactions, float min_support)
	{
		std::map<std::set<int>, float> ret;
		std::set<std::set<int> > candidates;
		std::map<std::set<int>, float> freqset;
		int k;

		// ---- generate candidate set with one element ----
		for (unsigned int i = 0; i < transactions.size(); i ++) {
			std::set<int>::iterator it = transactions[i].begin();
			for (; it != transactions[i].end(); it ++) {
				int tmp[1] = {*it};
				candidates.insert(std::set<int>(tmp, tmp + 1));
			}
		}

		// ---- prune 1-item candidates ----
		freqset = prune_candidates(candidates, transactions, min_support, ret);
		k = 1;

		// ---- iteratively performing ----
		while (!freqset.empty()) {
			printf("k=%d, freqset size=%d\n", k, int(freqset.size()));
			if (k >= 2) print_freqset_info(freqset);
			candidates = generate_candidates(freqset);
			// print_candidates_info(candidates);
			freqset = prune_candidates(candidates, transactions, min_support, ret);
			k += 1;
		}
		print_freqset_info(ret);
		return ret;
	}
}