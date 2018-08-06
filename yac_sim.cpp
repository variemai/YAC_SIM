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
#define WORD 4
using namespace std;

unsigned word_size;

/*Create entry struct for cache*/
typedef struct cache_entry{
	unsigned short valid;
	unsigned long tag;
}entry;

unsigned powerof2(bitset<32> bitset1) {
	unsigned i;
	for (i = 0; i < bitset1.size(); i++)
	{
		if (bitset1[i] == 1) break;
	}
	return i;
}

unsigned pow2(unsigned num){
	unsigned i = 0;
	while(num != 0 && num!=1 ){
		num = num / 2;
		i++;
	}
	return i;
}

void print_bin_index(unsigned long index, unsigned size){
	bitset<32> bits = {index};
	for(unsigned i=size-1; i>=0; i--){
		cout << bits[i];
		if(i==0) break;
	}
}

void display_contents(vector<entry> &cache, unsigned no_set, unsigned tag_size, unsigned index_size){
	for(unsigned i=0; i<no_set; i++) {
		bitset<32> bits = {cache[i].tag};
		//printf("Index: %d, Valid: %u, Tag: ",i,cache[i].valid);
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

unsigned long return_word(unsigned long tag, unsigned tag_shift, unsigned long index, unsigned block_offset){
	unsigned long retval;
	retval = tag << tag_shift;
	index = index << block_offset;
	retval = retval | index;
	return retval;
}

int main(void){
/******************************** Variables declarations *************************************/
	unsigned i, j, no_blocks, block_offset, tag_shift, tag_size, index_size;
	float hitrate = 0.0;
	char filename[256];
	unsigned no_set, asso = 1;
	int check = 0, hit = 0, miss = 0;
	unsigned long cache_size, block_size,address,memory_size;
	bitset<32> index_mask{ ULONG_MAX };
	bitset<32> tag_mask;
	unsigned long index, tmp, tmp_tag, tag,old_address;
	string str;
	unsigned interactive_mode, con_sim;
	ifstream infile;
	vector<entry> cache;
	entry init_entry;
/*********************************** Initialization *****************************************/
	init_entry.valid=0;
	init_entry.tag=0;
	interactive_mode = 0;
	cout << "Enter the memory size: ";
	cin >> memory_size;
	bitset<32> bitset0{ memory_size };
	tag_size = powerof2(bitset0);
	cout << "Enter the cache size: ";
	cin >> cache_size;
	bitset<32> bitset1{ cache_size };
	cout << "Enter the size of the Word: ";
	cin >> word_size;
	//cout << "size of Word: "<<word_size << "power: " << pow2(word_size) << endl;
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
	//cout << "Index Size: " << index_size << endl;
	//cout << "Block Offset: " << block_offset << endl;
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
	//cout << "Tag size in bits: " << tag_size << endl;
	//cout << "Tag Shift: " << tag_shift << endl;
	//cout << "No of indexes: " << no_blocks << endl;
	tmp = index_mask.to_ulong();
	//cout << "INDEX MASK: " << tmp << endl;
	tmp_tag = tag_mask.to_ulong();
	//cout << "TAG MASK: " << tmp_tag << endl;
start_sim:
cout << "Insert an address or one of the commands\n";
while (cin >> str){

	if(str.compare("exit") == 0 ) {
		/*TODO replace this shit with a function call and a break*/
		goto results;
	}

	if(str.compare("source") == 0 ){
		cout << "Enter trace filename : ";
		cin >> filename;
		cout << "Filename given: " << filename << endl;
		infile.open(filename, ios::in);
		if (!infile){
			cout << "Error! File not found...\n";
			exit(0);
		}
		while (getline(infile, str)){
			address = stoi(str);
			cout << "ADDR : " << address << " ";
			index = address & tmp;
			index = index >> block_offset;
			tag = address & tmp_tag;
			tag = tag >> tag_shift;
			check++;
			if (cache[index].valid == 1 && cache[index].tag == tag) {
				hit++;
				cout << "HIT with index: ";
				print_bin_index(index,index_size);
			}
			else {
				miss++;
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
		continue;
		//interactive_mode = 0;
		//goto normal_mode;
	}
	address = stoi(str);
	cout << "ADDR : " << address << " ";
	index = address & tmp;
	index = index >> block_offset;
	tag = address & tmp_tag;
	tag = tag >> tag_shift;
	check++;
	if (cache[index].valid == 1 && cache[index].tag == tag) {
		hit++;
		//cout << "HIT, with index: " << index;
		cout << "HIT, with index: ";
		print_bin_index(index,index_size);
	}
	else {
		miss++;
		if(cache[index].valid==1){
			old_address = return_word(cache[index].tag,tag_shift,index,block_offset);
			cout <<"miss, replace address: "<< old_address;
			//cout << "  with index: " << index;
			cout << "  with index: ";
			print_bin_index(index,index_size);
		}
		else{
			//cout << "MISS, with index: " << index;
			cout << "MISS, with index: ";
			print_bin_index(index,index_size);
			cache[index].valid = 1;
		}
		cache[index].tag = tag;
	}
	/*cout << "Interactive Mode? Yes [1], No [0]: ";
	cin >> interactive_mode;
	if( interactive_mode != 0 && interactive_mode != 1){
		cout << "Invalid Value, exiting..." <<endl;
		exit(0);
	}*/
}
normal_mode:
	if( !interactive_mode) {
	cout << "Enter trace filename : ";
	cin >> filename;
	cout << "Filename given: " << filename << endl;
	infile.open(filename, ios::in);
	if (!infile){
		cout << "Error! File not found...\n";
		exit(0);
	}

/****************************************** Simulation ***************************************/

	while (getline(infile, str))
	{
		if(str.compare("display") == 0){
			display_contents(cache,no_set,tag_size,index_size);
			continue;
		}
		address = stoi(str);
		cout << "ADDR : " << address << " ";
		index = address & tmp;
		index = index >> block_offset;
		tag = address & tmp_tag;
		tag = tag >> tag_shift;
		check++;
		if (cache[index].valid == 1 && cache[index].tag == tag) {
			hit++;
			cout << "HIT with index: ";
			print_bin_index(index,index_size);
			//cout << "HIT, with index: " << index;
		}
		else {
			miss++;
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
		printf("  HIT RATE: %.2f%%\n",100.0*(float(hit)/float(check)));
	}
	}
	else{
		while (cin >> str){
		if(str.compare("exit") == 0 ) goto results;

		if(str.compare("source") == 0 ){
			interactive_mode = 0;
			goto normal_mode;
		}

		if(str.compare("display") == 0){
			display_contents(cache,no_set,tag_size,index_size);
			continue;
		}
		address = stoi(str);
		cout << "ADDR : " << address << " ";
		index = address & tmp;
		index = index >> block_offset;
		tag = address & tmp_tag;
		tag = tag >> tag_shift;
		check++;
		if (cache[index].valid == 1 && cache[index].tag == tag) {
			hit++;
			//cout << "HIT, with index: " << index;
			cout << "HIT, with index: ";
			print_bin_index(index,index_size);
		}
		else {
			miss++;
			if(cache[index].valid==1){
				old_address = return_word(cache[index].tag,tag_shift,index,block_offset);
				cout <<"miss, replace address: "<< old_address;
				//cout << "  with index: " << index;
				cout << "  with index: ";
				print_bin_index(index,index_size);
			}
			else{
				//cout << "MISS, with index: " << index;
				cout << "MISS, with index: ";
				print_bin_index(index,index_size);
				cache[index].valid = 1;
			}
			cache[index].tag = tag;
		}
		printf("  HIT RATE: %.2f%%\n",100.0*(float(hit)/float(check)));

		}
	}
	if(!interactive_mode) infile.close();
	cout << "Continue Simulation? Yes [1], No [0]: ";
	cin >> con_sim;
	if(con_sim !=0 && con_sim !=1){
		cout << "Invalid Value, exiting..." << endl;
		exit(0);
	}
	if(con_sim)
	goto start_sim;
results:

/******************************************** Results *************************************************************/
	cout << "************* Cache Simulation Results ************"<< endl;
	printf ("*             Total ACCESSES : %18d *\n",check);
	printf ("*             Number of HITS : %18d *\n",hit);
	printf ("*             Number of MISSES : %16d *\n",miss);
	hitrate =100* (float(hit) / float(check));
	printf ("*             HIT RATE : %23.2f%% *\n",hitrate);
	cout << "***************************************************"<< endl;
	return 0;
}
