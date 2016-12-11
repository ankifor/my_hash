#include <iostream>
#include <string>
#include "schema_1.hpp"
#include "block_storage.h"

struct Database db;

static void read_data(const string& path)
{
	ifstream in;

	in.open(path + "tpcc_order.tbl");
	db.order.read_from_file(in);
	in.close();
}


int main(int argc, char **argv)
{
	string path = string(argv[1],strlen(argv[1])) + "/";
	read_data(string(path));
	cout << db.order.size() << endl;
	
	using type_key = tuple<Integer,Integer,Integer>;
	using type_val = tuple<Numeric<2,0>>;
	using p = tuple<type_key,type_val,void*>;
	
	Block_Storage<p, 1024*400> storage;
	for (size_t tid = 0; tid < db.order.size(); ++tid) {
		storage.insert(make_tuple(
			make_tuple(db.order.o_d_id[tid],db.order.o_w_id[tid],db.order.o_carrier_id[tid])
			,make_tuple(db.order.o_ol_cnt[tid])
			,nullptr
		));
	}
	
	storage.reset();
	
	for (size_t tid = 0; tid < db.order.size(); ++tid) {
		storage.insert(make_tuple(
			make_tuple(db.order.o_d_id[tid],db.order.o_w_id[tid],db.order.o_carrier_id[tid])
			,make_tuple(db.order.o_ol_cnt[tid])
			,nullptr
		));
	}
	
	storage.shrink(10000);
	
	cout << storage.size() << endl;
	
	return 0;
}
