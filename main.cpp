#include <iostream>
#include <string>
#include "schema_1.hpp"
#include "block_storage.h"
#include "my_hash.h"

struct Database db;

static void read_data(const string& path)
{
	ifstream in;

	in.open(path + "tpcc_order.tbl");
	db.order.read_from_file(in);
	in.close();
}

static void upd(tuple<Numeric<2,0>>& left, const tuple<Numeric<2,0>>& right) {
	get<0>(left) += get<0>(right);
}


int main(int argc, char **argv)
{
	string path = string(argv[1],strlen(argv[1])) + "/";
	read_data(string(path));
	
	using type_key = tuple<Integer,Integer,Integer>;
	using type_val = tuple<Numeric<2,0>>;
	using p = tuple<type_key,type_val,void*>;
	
//	Block_Storage<p, 1024*400> storage;
//	for (size_t tid = 0; tid < db.order.size(); ++tid) {
//		storage.insert(make_tuple(
//			make_tuple(db.order.o_d_id[tid],db.order.o_w_id[tid],db.order.o_carrier_id[tid])
//			,make_tuple(db.order.o_ol_cnt[tid])
//			,nullptr
//		));
//	}
//	storage.reset();
//	
//	for (size_t tid = 0; tid < db.order.size(); ++tid) {
//		storage.insert(make_tuple(
//			make_tuple(db.order.o_d_id[tid],db.order.o_w_id[tid],db.order.o_carrier_id[tid])
//			,make_tuple(db.order.o_ol_cnt[tid])
//			,nullptr
//		));
//	}
//	
//	storage.shrink(100);
//	
//	for (auto it = storage.begin(); it != storage.end(); ++it) {
//		auto key = get<0>(*it);
//		auto val = get<1>(*it);
//		cout << get<0>(key)
//			<< "," << get<1>(key)
//			<< "," << get<2>(key)
//			<< "," << get<0>(val)
//			<< endl;
//	}
//	
//	cout << storage.size() << endl;cout.flush();
	
	My_Hash<type_key, type_val, hash_types::hash<type_key>, equal_to<type_key>,true> h;
	for (size_t tid = 0; tid < db.order.size(); ++tid) {
		
//		auto it = h.find(make_tuple(db.order.o_d_id[tid],db.order.o_w_id[tid],db.order.o_carrier_id[tid]));
//		if (it == h.end()) {
//			h.insert(make_pair(
//				make_tuple(db.order.o_d_id[tid],db.order.o_w_id[tid],db.order.o_carrier_id[tid])
//				,make_tuple(db.order.o_ol_cnt[tid])
//			));
//		}
			h.modify(make_pair(
				make_tuple(db.order.o_d_id[tid],db.order.o_w_id[tid],db.order.o_carrier_id[tid])
				,make_tuple(db.order.o_ol_cnt[tid])
			), upd);
	}
	

//	cout << "raw" << endl;
//	for (auto x : h._hash_table) {
//		cout << x << endl;
//	}	
//	for (auto it = h._storage.begin(); it != h._storage.end();++it) {
//		cout << *it << endl;
//	}
	
	cout << "hash" << endl;
	for (auto it = h.begin(); it != h.end(); ++it) {
		cout << *it << endl;
	}
	
	return 0;
}
