/*****************************************************
 * University of Crete, Computer Science Department  *
 * cache simulator for the purposes of CS-225 ex 11  *
 * Author: Vardas Ioannis 04/02/2018                 *
 *****************************************************/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <limits.h>
#include <vector>
#include <stdint.h>
#include <bitset>
#define MAX_FILENAME 256
using namespace std;

unsigned word_size;

typedef struct profile_info{
	int check;
	int hit;
	int miss;
}cache_prof;

/*Create entry struct for cache*/
typedef struct cache_entry{
	unsigned short valid;
	unsigned long tag;
}entry;

typedef struct cache_characteristics{
	unsigned long cache_size;
    unsigned long block_size;
    unsigned long memory_size;
    unsigned long tmp_tag; 
    unsigned long tmp;
}cache_char;

/*************************Functions Definitions and Declarations***************/
unsigned
powerof2(bitset<32> bitset1) {
	unsigned i;
	for (i = 0; i < bitset1.size(); i++)
	{
		if (bitset1[i] == 1) break;
	}
	return i;
}

unsigned
pow2(unsigned num){
	unsigned i = 0;
	while(num != 0 && num!=1 ){
		num = num / 2;
		i++;
	}
	return i;
}

void
print_bin_index(unsigned long index, unsigned size){
	bitset<32> bits = {index};
	for(unsigned i=size-1; i>=0; i--){
		cout << bits[i];
		if(i==0) break;
	}
}

void
display_contents(vector<entry> &cache, unsigned no_set, unsigned tag_size,
	unsigned index_size){
	for(unsigned i=0; i<no_set; i++) {
		bitset<32> bits = {cache[i].tag};
		cout << "Index: ";
		print_bin_index(i,index_size);
		cout << " Valid: " << cache[i].valid << ", Tag: ";
		for(unsigned j=tag_size-1; j>=0; j--){
			cout << bits[j] ;
			if (j==0) break;
		}
		cout << endl;
	}
}

unsigned long
return_word(unsigned long tag, unsigned tag_shift, unsigned long index,
	unsigned block_offset){
	unsigned long retval;
	retval = tag << tag_shift;
	index = index << block_offset;
	retval = retval | index;
	return retval;
}

void
print_results(cache_prof *prof_info){
	if(prof_info->check == 0 ) return ;
	float hitrate = 0.0;
	cout << "************* Cache Simulation Results ************"<< endl;
	printf ("*             Total ACCESSES : %18d *\n",prof_info->check);
	printf ("*             Number of HITS : %18d *\n",prof_info->hit);
	printf ("*             Number of MISSES : %16d *\n",prof_info->miss);
	hitrate =100* (float(prof_info->hit) / float(prof_info->check));
	printf ("*             HIT RATE : %23.2f%% *\n",hitrate);
	cout << "***************************************************"<< endl;
}

void
cache_access(vector<entry> &cache, unsigned long tmp, unsigned tag_shift,
	unsigned long tmp_tag, unsigned long address, unsigned block_offset,
	unsigned index_size, cache_prof* prof_info){
	unsigned long index, tag, old_address;
	cout << "ADDR : " << address << " ";
	index = address & tmp;
	index = index >> block_offset;
	tag = address & tmp_tag;
	tag = tag >> tag_shift;
	prof_info->check++;
	if (cache[index].valid == 1 && cache[index].tag == tag) {
		prof_info->hit++;
		cout << "HIT with index: ";
		print_bin_index(index,index_size);
	}
	else {
		prof_info->miss++;
		if(cache[index].valid==1){
			old_address = return_word(cache[index].tag,tag_shift,index,block_offset);
			cout <<"miss, replace address: "<< old_address;
			cout << "  with index: ";
			print_bin_index(index,index_size);
		}
		else{
			cout << "MISS, with index: ";
			print_bin_index(index,index_size);
			cache[index].valid = 1;
		}
		cache[index].tag = tag;
	}
	cout << endl;
}

int
main(void){
/******************************** Variables declarations **********************/
	unsigned i, j, no_blocks, block_offset, tag_shift, tag_size, index_size;
	string filename;
	unsigned no_set, asso = 1;
	unsigned long cache_size, block_size, memory_size, tmp_tag, tmp, address;
	bitset<32> index_mask{ ULONG_MAX };
	bitset<32> tag_mask;
	string str;
	ifstream infile;
	vector<entry> cache;
	entry init_entry;
    cache_prof prof_info;
/*********************************** Initialization ***************************/
	init_entry.valid=0;
	init_entry.tag=0;
    prof_info.check = 0;
    prof_info.hit = 0;
    prof_info.miss = 0;
	cout << "Enter the memory size: ";
	cin >> memory_size;
	bitset<32> bitset0{ memory_size };
	tag_size = powerof2(bitset0);
	cout << "Enter the cache size: ";
	cin >> cache_size;
	bitset<32> bitset1{ cache_size };
	cout << "Enter the size of the Word: ";
	cin >> word_size;
	i = powerof2(bitset1);
	index_size = i;
	index_mask = index_mask << i;
	index_mask = ~index_mask;
	tag_mask = ~index_mask;
	cout << "Enter the block size: ";
	cin >> block_size;
	bitset<32> bitset2{ block_size };
	block_offset = powerof2(bitset2);
	index_size = index_size - block_offset;
	for (j = 0; j < block_offset; j++)
	{
		index_mask[j] = 0;
	}
	tag_shift = i;
	no_blocks = cache_size / block_size ;
	no_set = no_blocks / asso;
	for(unsigned i=0; i<no_set; i++){
		cache.push_back(init_entry);
	}
	tag_size = tag_size - i;
	tmp = index_mask.to_ulong();
	tmp_tag = tag_mask.to_ulong();

/***************************Simulation Starting********************************/
	printf("Insert an address or a valid command\n");
	printf("For a list of the available commands type \"cmd\"\n");
	while (cin >> str){
		if(str.compare("cmd") == 0){
			printf("\texit:\t\t\t\tPrints the results and exits YAC SIM\n");
			printf("\tsource:\t\t\t\tRead addresses from a file\n");
			printf("\tdisplay_contents:\t\tPrints the contents of the Cache\n");
			continue;
		}
		if(str.compare("exit") == 0 ) {
			print_results(&prof_info);
			break;
		}
		//Input from file
		if(str.compare("source") == 0 ){
			cout << "Enter trace filename : ";
			cin >> filename;
			if(filename.size() > MAX_FILENAME){
				cout << "Filename exceeded max size" << endl;
				exit(-1);
			}
			cout << "Filename given: " << filename << endl;
			infile.open(filename, ios::in);
			if (!infile){
				cout << "Error! File not found...\n";
				exit(-1);
			}
			while (getline(infile, str)){
				address = stoi(str);
				cache_access(cache, tmp, tag_shift, tmp_tag, address,
				block_offset, index_size, &prof_info);
			}
			str.clear();
			infile.close();
			continue;
		}
		if(str.compare("display_contents") == 0 ){
			display_contents(cache,no_set,tag_size,index_size);
			continue;
		}
		//Input single address
		address = stoi(str);
		cache_access(cache, tmp, tag_shift, tmp_tag, address,
		block_offset, index_size, &prof_info);
	}
	return 0;
}
