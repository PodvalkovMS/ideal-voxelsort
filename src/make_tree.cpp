#include "make_tree.h"



	Tot_tree::Tot_tree(const std::vector<std::vector<double> > & all, std::size_t d, int max_leaf)
	{
	
		mat_index_pointer = std::make_unique<my_kd_tree_t_n>(d, all, max_leaf);

	}

	


	

	void Tot_tree::find_near( const std::vector<double>& numbers_iscom ,std::vector<std::size_t> &  ret_indexes,
		std::vector<double>& out_dists_sqr, std::size_t n) {

		nanoflann::KNNResultSet<double> resultSet(n);

		resultSet.init(&ret_indexes[0], &out_dists_sqr[0]);
		mat_index_pointer->index->findNeighbors(resultSet, &numbers_iscom[0], nanoflann::SearchParams());
	}
	

	



