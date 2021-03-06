#include <iostream>


//notice: 数组中是按照下标为0开始的，所以内层循环中的j为 >= 0。
//O(n*n)
template<typename T>
void InsertSort(T &arr, int len) {
	for (int i = 1; i < len; ++i) {
		auto key = arr[i];
		int j = 0;
		for (j = i - 1; arr[j] < key && j >= 0; --j) {
			arr[j + 1] = arr[j];
		}
		arr[j + 1] = key;
	}
}


