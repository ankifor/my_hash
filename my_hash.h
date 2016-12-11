#pragma once
#include <tuple>
#include <vector>
#include "block_storage.h"

using namespace std;
template<
	typename _Key, 
	typename _Tp,
	typename _Hash = hash<_Key>,
	typename _Pred = equal_to<_Key>,
	bool _Unique = true >
class My_Hash {

public://private
	using _Entry = tuple<_Key, _Tp, void*>;
	
	static const size_t _Key_N = 0;
	static const size_t _Tp_N = 1;
	static const size_t _Next_N = 2;
	
	Block_Storage<_Entry, 1024*400> _storage;
	vector<_Entry*> _hash_table;
	size_t _size;
	
public:
	using Entry = _Entry;
	using Update_Fun = void (*)(_Tp& left, const _Tp& right);
	
	class Iterator {
	public:
		Iterator(My_Hash& my_hash, size_t bucket, My_Hash::_Entry* entry) 
			: _hash(my_hash), _bucket(bucket), _entry(entry) {}
			
		~Iterator() {}
		
		bool operator==(const Iterator& it) const { return _entry==it._entry; }
		bool operator!=(const Iterator& it) const { return _entry!=it._entry; }
		
		Iterator& operator++() { //prefix increment
			//cout << "my_hash::it::++: " << *_entry << endl;
			if (get<_Next_N>(*_entry) != nullptr) {
				_entry = reinterpret_cast<_Entry*>(get<_Next_N>(*_entry));
			} else {
				_entry = nullptr;
				while (++_bucket < _hash._hash_table.size() && _entry == nullptr) {
					_entry = _hash._hash_table[_bucket];
				}
			}
			return *this;
		}
		
		Entry& operator*() const { return *reinterpret_cast<Entry*>(_entry); }
	private:
		My_Hash& _hash;
		size_t _bucket;
		_Entry* _entry;
	};
	
	Iterator begin() { 
		size_t i = 0;
		for (i = 0; i < _hash_table.size() && _hash_table[i] == nullptr; ++i) {
			;
		}
		if (i == _hash_table.size()) {
			return Iterator(*this,i, nullptr); 
		} else {
			return Iterator(*this,i, _hash_table[i]); 
		}
	}
	Iterator end() { return Iterator(*this,_hash_table.size(),nullptr); }

	My_Hash() : _size(0) 
	{
		_hash_table.resize(2, 0);
	}
	~My_Hash() {}

	size_t size() const { return _size;}
	
	Iterator insert(pair<_Key, _Tp>&& x) {
		_Hash h;
		
		size_t bucket = h(x.first) % _hash_table.size();
		_Entry** current = &_hash_table[bucket];
		//if unique, then insert to the beginning of the bucket
		//else insert before the first item with the same key
		if (!_Unique) {
			_Pred eq;
			while (*current != nullptr && !eq(x.first, get<_Key_N>(**current))) {
				*current = reinterpret_cast<_Entry*>(get<_Next_N>(**current));
			}
		}
		*current = _storage.insert(make_tuple(x.first, x.second, (void*) *current));
		//cout << *current << ":" << **current << endl;
		++_size;
		
		return Iterator(*this, bucket, *current);
	}
	
	Iterator modify(pair<_Key, _Tp>&& x, Update_Fun upd) {
		_Hash h;
		_Pred eq;
		
		size_t bucket = h(x.first) % _hash_table.size();
		_Entry** current = &_hash_table[bucket];
		//if unique, then insert to the beginning of the bucket
		//else insert before the first item with the same key
		while (*current != nullptr && !eq(x.first, get<_Key_N>(**current))) {
			cout << *current << ";";
			current = reinterpret_cast<_Entry**>(&get<_Next_N>(**current));
		}
		cout << *current << endl;
		
		if (!_Unique || *current == nullptr) {
			*current = _storage.insert(make_tuple(x.first, x.second, (void*) *current));
		} else {
			//need to modify
			upd(get<_Tp_N>(**current),x.second);
		}
		
		cout << *current << ":" << **current << endl;
		++_size;
		
		return Iterator(*this, bucket, *current);
	}
	
	Iterator find(_Key&& x) {
		_Hash h;
		
		size_t bucket = h(x);
		bucket %= _hash_table.size();
		_Entry* current = _hash_table[bucket];
		_Pred eq;
		while (current != nullptr && !eq(x, get<_Key_N>(*current))) {
			current = reinterpret_cast<_Entry*>(get<_Next_N>(*current));
		}
		return Iterator(*this, bucket, current);
	}
	
};