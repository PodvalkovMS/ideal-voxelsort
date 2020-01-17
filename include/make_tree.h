#pragma once
#include "stdafx.h"

	class Tot_tree {
		
		typedef KDTreeVectorOfVectorsAdaptor< std::vector<std::vector<double> >, double >  my_kd_tree_t_n;
	public:
	Tot_tree(const std::vector<std::vector<double> >&  all, size_t dim, int max_leaf = 10);
	

		void find_near(
		const std::vector<double>&  numbers_iscom,
		std::vector<size_t>&   ret_indexes,
		std::vector<double>& out_dists_sqr,
		std::size_t n);
	

	private:
		std::unique_ptr<my_kd_tree_t_n>  mat_index_pointer;
	};
	
		
