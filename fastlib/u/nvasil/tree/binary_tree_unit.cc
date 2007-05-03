/*
 * =====================================================================================
 *
 *       Filename:  binary_tree_unit.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/27/2007 10:20:40 AM EDT
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Nikolaos Vasiloglou (NV), nvasil@ieee.org
 *        Company:  Georgia Tech Fastlab-ESP Lab
 *
 * =====================================================================================
 */

#include <unistd.h>
#include <sys/mman.h>
#include <limits.h>
#include "fastlib/fastlib.h"
#include "u/nvasil/test/test.h"
#include "u/nvasil/dataset/binary_dataset.h"
#include "kd_pivoter1.h"
#include "tree_parameters_macro.h"
#include "binary_tree.h"

using namespace std;

template<typename TYPELIST, bool diagnostic>
class BinaryTreeTest {
 public:
  typedef typename TYPELIST::Precision_t   Precision_t;
	typedef typename TYPELIST::Allocator_t   Allocator_t;
  typedef typename TYPELIST::Metric_t      Metric_t;
	typedef typename TYPELIST::BoundingBox_t BoundingBox_t;
	typedef typename TYPELIST::NodeCachedStatistics_t NodeCachedStatistics_t;
	typedef typename TYPENAME::PointIdDiscriminator_t PointIdDiscriminator_t;
	typedef typename TYPELIST::Pivot_t Pivot_t;
	typedef Point<Precision_t, Allocator_t> Point_t; 
	typedef BinaryTree<TYPELIST, diagnostic> BinaryTree_t;
	
	void Init() {
    dimension_=2;
    num_of_points_=1000;
    data_file_="data";
		knns_=40;
    range_=0.2;
		result_file_="allnn";
    datase_.Init(data_file_, num_of_points_, dimension_);
    for(index_t i=0; i<num_of_points_; i++) {
		  for(index_t j=0; j<dimension_; j++) {
			  data_.At(i,j)=rand()/RAND_MAX;
			}
			data_.set_id(i,i);
		}		
    tree_.Init(data_);
	}
	void Destruct() {
	  tree_.Destruct();
	  data_.Destruct();
	  unlink(data_file_.c_str());
	  unlink(data_file_.append(".ind").c_str());	
		unlink(result_file_.c_str());
	}
	void BuildDepthFirst(){
    tree_.BuildDepstFirst();
	}
	void BuildBreadthFirst() {
	  tree_.BuildBreadthFirst();
	}
	void kNearestNeighbor() {
		tree_->BuildDepthFirst();
    vector<pair<Precision_t, Point_t> nearest_tree;
		pair<Precisiont_t, index_t> nearest_naive[num_of_points_];
	  for(index_t i=0; i<num_of_points_; i++) {
		  tree_.NearestNeighbor(data_At(i),
                            &nearest_tree,
                            knns_);
			Naive(data_-At(i), knns_, neares_naive);
			for(index_t j=0; j<knns_; j++) {
			  ASSERT_DOUBLE_APPROX(nearest_naive[j+1].first, 
			 	  	                 nearest_tree[j].first,
					                   numeric_limits<Precision_t>::epsilon());
			  TEST_ASSERT(nearest_tree[j].second.get_id()==
				            naive_tree[j+1].second)	;
			}
		}
	}
	void RangeNearestNeighbor() {
		tree_->BuildBreadthFirst();
	  vector<pair<Precision_t, Point_t> nearest_tree;
		pair<Precisiont_t, index_t> nearest_naive[num_of_points_];
	  for(index_t i=0; i<num_of_points_; i++) {
		  tree_.NearestNeighbor(data_At(i),
                            &nearest_tree,
                            range_);
			Naive(data_->At(i), neares_naive);
			for(index_t j=0; j<nearest_tree.size(); j++) {
			  ASSERT_DOUBLE_APPROX(nearest_naive[j+1].first, 
			 	  	                 nearest_tree[j].first,
					                   numeric_limits<Precision_t>::epsilon());
			  TEST_ASSERT(nearest_tree[j].second.get_id()==
				            nearest_naive[j+1].second)	;
			}
		}
	}
	void AllKNearestNeighbors() {
		tree_->BuildBreadthFirst();
	  tree_->InitAllKNearestNeighborOutput(result_file_, 
				                                 knns_);
		tree_->AllKNearestNeighbor(parent_, knns_);
    tree_->CloseAllKNearestNeighborOutput(knns_);
    struct stat info;
    FATAL(stat(data_file_.c_str(), &info)!=0, "Error %s file %s\n",
				  %strerror(errno), file_name.c_str());
		uint64 map_size = info.st_size-sizeof(int32);

    int fp=open(result, O_RDWR);
    Node_t::NNResult *res=mmap(NULL, 
				                       map_size, 
															 PROT_READ | PROT_WRITE, 
															 MAP_SHARED, fp,
			                         0);
		close(fp);
		std::sort(res, res+num_of_points_*knns);
		pair<Precisiont_t, index_t> nearest_naive[num_of_points_];
    for(index_t i=0; i<num_of_points_; i++) {
		  Naive(data_->At(res[i].point_id_), nearest_naive);
		 	for(index_t j=0; j<knns; j++) {
			 	ASSERT_DOUBLE_APPROX(nearest_naive[j+1].first, 
			 	  	                 res[i*knns+j].distance_,
					                   numeric_limits<Precision_t>::epsilon());
			  TEST_ASSERT(res[j].second.get_id()==
				            nearest_naive[j+1].second);
			}			
		}		
		munmap(res, map_size);
	}
	
	void AllRangeNearestNeighbors() {
	  tree_->BuildBreadthFirst();
	  tree_->InitAllKNearestNeighborOutput(result_file_, 
				                                 range_);
		tree_->AllKNearestNeighbor(parent_, range_);
    tree_->CloseAllKNearestNeighborOutput();
    struct stat info;
    FATAL(stat(data_file_.c_str(), &info)!=0, "Error %s file %s\n",
				  %strerror(errno), file_name.c_str());
		uint64 map_size = info.st_size-sizeof(int32);

    int fp=open(result, O_RDWR);
    Node_t::NNResult *res=mmap(NULL, 
				                       map_size, 
															 PROT_READ | PROT_WRITE, 
															 MAP_SHARED, fp,
			                         0);
		close(fp);
		std::sort(res, res+num_of_points_*knns);
		pair<Precisiont_t, index_t> nearest_naive[num_of_points_];
		index_t i=0;
    while (i<num_of_points_) {
		  Naive(data_->At(res[i].point_id_), nearest_naive);
			index_t j=0;
		 	while (nearest_naive[j+1].first<range) {
			 	ASSERT_DOUBLE_APPROX(nearest_naive[j+1].first, 
			 	  	                 res[i].distance_,
					                   numeric_limits<Precision_t>::epsilon());
			  TEST_ASSERT(res[i].second.get_id()==
				            nearest_naive[j+1].second);
				i++;
				j++;
			}			
		}		
		munmap(res, map_size);
	}

	
	TEST_SUITE(BuildDepthFirst,
		         BuildBreadthFirst,
		         kNearestNeighbor,
		         RangeNearestNeighbor,
		         AllKNearestNeighbors,
						 AllRangeNearestNeighbors)	
 private:
  BinaryTree_t tree_; 
  BiinaryDataset<Precision_t> data_
  string data_file_;
	string result_file_;
	int32 dimension_;
	index_t num_of_points_;
	index_t knns_;
	Precision_t range_;
	
	void Naive(index_t query, 
			       pair<Precision_t, index_t> *result) {
    
		for(index_t i=0;  i<num_of_points_; i++) {
		  if (unlikely(data_.get_id(i)==data_.get_id(query))) {
			  continue;
			}
			Precision_t dist=Metric_t::Distance(data_.At(i), 
					                                data_.At(query), 
					                                dimension_);
		  result[i].first=i;
		  result[i].second=dist
		}
		sort(result, result+num_of_points_);
	}
	
};

TREE_PARAMETERS(float32,
		            MemoryManager<false>,
		            EuclideanMetric<float32>,
	              HyperRectangle,
	              NullStatistics,
                SimpleDiscriminator,
		            KdPivoter1) 

typedef BinaryTreeTest<Parameters, false> BinaryTreeTest_t;
RUN_ALL_TESTS(BinaryTreeTest_t)
