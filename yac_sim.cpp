/*****************************************************************************\
* YAC Sim - Yet Another Cache Simulator, created for the purposes of CS-225
* course CSD, University of Crete, creates a cache with user defined attributes
* and simulates accesses from user input
* Copyright (C) 2018  Ioannis Vardas - vardas@ics.forth.gr
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>
*******************************************************************************/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <limits.h>
#include <vector>
#include <stdint.h>
#include <bitset>
#include <locale>
#include <limits>
#include <cmath>
#define MAX_FILENAME 256
using namespace std;

unsigned word_size;

typedef struct profile_info{
	int check;
	int hit;
	int miss;
}cache_prof;

/*Cache entry layout*/
typedef struct cache_entry{
	unsigned short valid[16]; /*TODO use dynamic tables */
	unsigned long tag[16];
    unsigned short LRU[16];
}entry;

/*Cache characteristics and some useful masks for bit operations*/
typedef struct characteristics_of_cache{
    unsigned long tmp_tag; /*tag mask*/
    unsigned long tmp; /*index mask*/
    unsigned block_offset;
    unsigned tag_shift; /*how many bits are the tag bits shifted to the left*/
    unsigned tag_size; /*in bits*/
    unsigned index_size;
    unsigned no_set;
    unsigned asso = 1;
}cache_char;

/*************************Function Declarations*******************************/

unsigned LRU_policy(vector<entry> &cache,cache_char*);

void set_cache_specs(unsigned long,unsigned long,unsigned long,unsigned,
        cache_char*);

void print_bin_index(unsigned long, unsigned);

unsigned long return_word(unsigned long , unsigned, unsigned long, unsigned );

unsigned powerof2(bitset<32>);

unsigned pow2(unsigned );

void clear_contents(vector<entry> &cache, cache_char*);

/*************************Function Definitions********************************/

unsigned LRU_policy(vector<entry> &cache,cache_char*specs, unsigned long index)
{
    unsigned short min_lru;
    unsigned way;
    way = 0 ;
    min_lru = cache[index].LRU[0];
    for(unsigned i=0; i<specs->asso; i++){
        if(cache[index].LRU[i] < min_lru ){
            min_lru = cache[index].LRU[i];
            way = i;
            cout << " MIN_LRU =  " << min_lru << " ";
        }
    }
    return way;
}

void set_cache_specs(unsigned long cache_size, unsigned long block_size, 
        unsigned long memory_size,unsigned asso, cache_char* characteristics)
{
    unsigned i, j;
    unsigned long no_blocks;
	bitset<32> tag_mask;
	bitset<32> index_mask{ ULONG_MAX };
	bitset<32> bitset0{ memory_size };
    bitset<32> bitset1{ cache_size };
	bitset<32> bitset2{ block_size };
	characteristics->tag_size = powerof2(bitset0);
    unsigned pow_of_asso = pow2(asso);
	i = powerof2(bitset1);
	characteristics->index_size = i - pow_of_asso;
	index_mask = index_mask << characteristics->index_size;
	tag_mask = index_mask;
	index_mask = ~index_mask;
    characteristics->asso = asso;
    characteristics->block_offset = powerof2(bitset2);
    characteristics->index_size = characteristics->index_size -
        characteristics->block_offset;
    for(j=0; j<characteristics->block_offset; j++){
        index_mask[j]=0;
    }
    /*The number of bits needed for RIGHT shift for tag*/
    characteristics->tag_shift = i-pow_of_asso;
    no_blocks = cache_size / block_size;
    characteristics->no_set=no_blocks/characteristics->asso;
    characteristics->tag_size = characteristics->tag_size - i + pow_of_asso;
    characteristics->tmp = index_mask.to_ulong();
    characteristics->tmp_tag = tag_mask.to_ulong();
}

void print_specs(cache_char* specs)
{
    cout<<"Tag Size: "<<specs->tag_size<<"\nIndex size: "<<specs->index_size
        <<endl<<"Tmp: "<<specs->tmp<<"\nTmp_tag: "<<specs->tmp_tag<<endl;
    cout << "Tag Shift: " << specs->tag_shift<<endl;
    cout << "Number of Sets: "<< specs->no_set << endl;
}

void cache_access(vector<entry> &cache, unsigned long address, 
       cache_char* specs, cache_prof* prof_info)
{
    unsigned long index, tag, old_address;
    unsigned short done, min_lru, max_lru;
    unsigned way;
	cout << "ADDR: " << address << " ";
	index = address & specs->tmp;
	index = index >> specs->block_offset;
	tag = address & specs->tmp_tag;
	tag = tag >> specs->tag_shift;
    /*cout << "TAG: " << tag <<endl;*/
	prof_info->check++;
    done = 0;
    min_lru = 0;
    way = 0;
    for(unsigned i=0; i<specs->asso; i++){
	    if (cache[index].valid[i] == 1 && cache[index].tag[i] == tag) {
		    prof_info->hit++;
            /*Update the LRU info*/
            cache[index].LRU[i]++;
		    cout << "HIT with index: ";
		    print_bin_index(index,specs->index_size);
            if(specs->asso > 1) {
                cout << " WAY: " << i;
                cout << " LRU: " << cache[index].LRU[i] ;
            }
            done = 1;
	    }
    }
    if(!done){
        prof_info->miss++;
        /*Miss-first search for an invalid position in the way*/
        for(unsigned i=0; i<specs->asso; i++){
            if(cache[index].valid[i] == 0){
                cache[index].tag[i] = tag;
                cache[index].valid[i] = 1;
                cache[index].LRU[i] = 1;
                cout << "MISS, with index: ";
                print_bin_index(index,specs->index_size);
                if(specs->asso > 1){
                    cout << " WAY: " << i;
                    cout << " LRU: " << cache[index].LRU[i] ;
                }
                done = 1;
                break;
            }
        }
        if(!done && specs->asso > 1){
            /*LRU replacement algorithm*/
            min_lru = cache[index].LRU[0];
            max_lru = cache[index].LRU[0];
            for(unsigned i=0; i<specs->asso; i++){
                if(cache[index].LRU[i] < min_lru ){
                    min_lru = cache[index].LRU[i];
                    way = i;
                    //cout << " MIN_LRU =  " << min_lru << " ";
                }
                if(cache[index].LRU[i] > max_lru){
                    max_lru = cache[index].LRU[i];
                }
            }
            old_address=return_word(cache[index].tag[way],specs->tag_shift,
                    index,specs->block_offset);
			cout <<"MISS, Replace address: "<< old_address;
			cout << "  with index: ";
			print_bin_index(index,specs->index_size);
            cout << " WAY: " << way;
            cout << " LRU: " << cache[index].LRU[way] ;
            cache[index].tag[way] = tag;
//            cache[index].LRU[way] = 1;
            cache[index].LRU[way] = max_lru+1;

        }
        /*Directed mapped cache access no need for LRU*/
        else if(!done && specs->asso == 1){
			old_address = return_word(cache[index].tag[0],specs->tag_shift,
                    index,specs->block_offset);
			cout <<"MISS, replace address: "<< old_address;
			cout << "  with index: ";
			print_bin_index(index,specs->index_size);
		    cache[index].tag[0] = tag;
        }
    }
	cout << endl;
}


unsigned powerof2(bitset<32> bitset1)
{
    unsigned i;
	for (i = 0; i < bitset1.size(); i++)
	{
		if (bitset1[i] == 1) break;
	}
	return i;
}

unsigned pow2(unsigned num)
{
    unsigned i = 0;
	while(num != 0 && num!=1 ){
		num = num / 2;
		i++;
	}
	return i;
}

void print_bin_index(unsigned long index, unsigned size)
{
    bitset<32> bits = {index};
	for(unsigned i=size-1; i>=0; i--){
		cout << bits[i];
		if(i==0) break;
	}
}

void clear_contents(vector<entry> &cache, cache_char* specs, cache_prof *prof_info){
    for(unsigned i=0; i<specs->no_set; i++) {
        for(unsigned j=0; j<specs->asso; j++){
            cache[i].valid[j] = 0;
            cache[i].tag[j] = 0; //not necessary
        }
    }
    prof_info->check = 0;
    prof_info->hit = 0;
    prof_info->miss = 0;
    cout << "CLEARED Cache Contents and Statistics" << endl;
}

void display_contents(vector<entry> &cache, cache_char* specs)
{
    for(unsigned i=0; i<specs->no_set; i++) {
        for(unsigned j=0; j<specs->asso; j++){
		bitset<32> bits = {cache[i].tag[j]};
		cout << "Index: ";
		print_bin_index(i,specs->index_size);
        cout << " Way: " << j;
		cout << " Valid: " << cache[i].valid[j];
        if(cache[i].valid[j]){
            cout << ", Tag: ";
            for(unsigned k=specs->tag_size-1; k>=0; k--){
                cout << bits[k] ;
                if (k==0) break;
            }
            if(specs->asso > 1){
                cout << " LRU: " << cache[i].LRU[j];
            }
            cout << " First Addr in Block: " << return_word(cache[i].tag[j],
                                       specs->tag_shift,i,specs->block_offset);
        }
		cout << endl;

        }
    }
}

unsigned long return_word(unsigned long tag, unsigned tag_shift,
        unsigned long index, unsigned block_offset)
{
    unsigned long retval;
	retval = tag << tag_shift;
	index = index << block_offset;
	retval = retval | index;
	return retval;
}

void print_results(cache_prof *prof_info)
{
    if(prof_info->check == 0 ) return ;
	float hitrate = 0.0;
	cout << "************* Cache Simulation Statistics *********"<< endl;
	printf ("*             Total ACCESSES : %18d *\n",prof_info->check);
	printf ("*             Number of HITS : %18d *\n",prof_info->hit);
	printf ("*             Number of MISSES : %16d *\n",prof_info->miss);
	hitrate =100* (float(prof_info->hit) / float(prof_info->check));
	printf ("*             HIT RATE : %23.2f%% *\n",hitrate);
	cout << "***************************************************"<< endl;
}

bool isPowerOfTwo(int n)
{
    return (ceil(log2(n)) == floor(log2(n)));
}

int main(void)
{
/******************************** Variables declarations **********************/
	string filename;
    unsigned asso;
	unsigned long cache_size, block_size, memory_size, address;
	string str;
	ifstream infile;
	vector<entry> cache;
	entry init_entry;
    cache_prof prof_info;
    cache_char cache_specs;
    int alnum_flag;
    string::size_type i;
    locale loc;

/*********************************** Initialization ***************************/
    prof_info.check = 0;
    prof_info.hit = 0;
    prof_info.miss = 0;
    i = 0;
    cout << "\nYAC Simulator, Copyright (C) 2018 Ioannis Vardas\n";
    cout << "Contact vardas@ics.forth.gr\n\n";
    /*Print info about GPL licence*
    cout << "This program is under ";
    cout << "the terms of the GNU General Public License v3\nFor more info see";
    cout << ": <https://www.gnu.org/licenses/>" << endl << endl;
    */
get_memsize:
	cout << "Enter the Main Memory size in Bytes: ";
	cin >> memory_size;
    if(cin.fail()){
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "Bad Input: Memory size is not a number" << endl;
        goto get_memsize;
    }

get_wordsize:
    cout << "Enter the size of the Word in Bytes: ";
	cin >> word_size;
    if(cin.fail()){
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "Bad Input: Word size is not a number" << endl;
        goto get_wordsize;
    }

get_cachesize:
	cout << "Enter the Cache size in Bytes: ";
	cin >> cache_size;
    if(cin.fail()){
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "Bad Input: Cache size is not a number" << endl;
        goto get_cachesize;
    }
    if(cache_size >= memory_size){
        cout << "Bad Input: Cache size should not exceed Main Memory size"<<endl;
        cin.clear();
        goto get_cachesize;
    }

get_blocksize:
	cout << "Enter the Cache Block(Line) size in Bytes: ";
	cin >> block_size;
    if(cin.fail()){
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "Bad Input: Block size is not a number" << endl;
        goto get_blocksize;
    }
    if(block_size < word_size){
        cout << "Bad Input: Block size should not be less than Word size"<<endl;
        cin.clear();
        goto get_blocksize;
    }

get_asso:
	cout << "Enter associativity (1,2,4,8 or 16): ";
	cin >> asso;
    if(cin.fail()){
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "Bad Input: Associativity is not a number" << endl;
        goto get_asso;
    }
    if( asso < 1 || asso > 16 || !isPowerOfTwo(asso) ){
        cout << "Bad Input: Associativity should be either 1,2,4,8 or 16"<<endl;
        cin.clear();
        goto get_asso;
    }
    if( asso > cache_size/block_size ){
        cout << "Bad Input: Associativity level is too high, try a lower value"<<endl;
        cin.clear();
        goto get_asso;
    }
    set_cache_specs(cache_size,block_size,memory_size,asso, &cache_specs);
    for(unsigned i=0; i<asso; i++){
        init_entry.tag[i] = 0;
        init_entry.valid[i] = 0;
        init_entry.LRU[i] = 0;
    }
	for(unsigned i=0; i<cache_specs.no_set; i++){
		cache.push_back(init_entry);
	}

/***************************Simulation Starting********************************/
    cout << "Insert an address or a valid command, type \"cmd\" for a list of available commands"<<endl;
	while (true){
        getline(cin,str);
        if(str.length() > 0 ){
            if(str.compare("cmd") == 0){
                printf("\texit:\t\t\tPrints the Statistics and exits YAC Simulator\n");
                printf("\tsource <filename>:\tRead addresses from a file\n");
                printf("\tdisplay:\t\tPrints the contents of the Cache and the\
 Statistics\n");
                printf("\tclear:\t\t\tClears the contents of the Cache and resets\
 statistics\n");
                continue;
            }
            if(str.compare("exit") == 0 ) {
                print_results(&prof_info);
                break;
            }
            /*Display contents of cache*/
            if(str.compare("display") == 0 ){
                display_contents(cache,&cache_specs);
                cout << endl;
                print_results(&prof_info);
                cout << endl;
                cout << "Insert an address or a valid command, type \"cmd\" for a list of available commands"<<endl;
                continue;
            }
            if(str.compare("clear") == 0 ){
                clear_contents(cache,&cache_specs,&prof_info);
                cout << endl;
                cout << "Insert an address or a valid command, type \"cmd\" for a list of available commands"<<endl;
                continue;
            }
            /*Input from file*/
            if(str.length() > 6 ){
                string token1 = str.substr(0,6);
                string token2 = str.substr(7,string::npos);
                if(token1.compare("source") == 0 ){
                    filename.assign(token2);
                    if(filename.size() > MAX_FILENAME){
                        cout << "Filename exceeded max size\nExiting..." << endl;
                        exit(-1);
                    }
                    cout << "Filename given: " << filename << endl << endl;
                    infile.open(filename, ios::in);
                    if (!infile){
                        cout << "Error! File not found...\n";
                        exit(-1);
                    }
                    while (getline(infile, str)){
                        address = stoi(str);
                        cache_access(cache, address, &cache_specs,&prof_info);
                    }
                    infile.close();
                    cout << endl;
                    cout << "Insert an address or a valid command, type \"cmd\" for a list of available commands"<<endl;
                    continue;
                }
            }
            /*Input single address*/
            alnum_flag = 0;
            while(i<str.length()) {
                if(isdigit(str[i])==false){
                    alnum_flag = 1;
                    break;
                }
                i++;
            }
            i = 0;
            if(alnum_flag == 1){
                cout << "Wrong command or Address not alphanumeric"<< endl;
                cout << endl;
                cout << "Insert an address or a valid command, type \"cmd\" for a list of available commands"<<endl;
                str.clear();
                continue;
            }
            address = stoi(str);
            cache_access(cache, address, &cache_specs,&prof_info);
            cout << endl;
            cout << "Insert an address or a valid command, type \"cmd\" for a list of available commands"<<endl;
        }
	}
	return 0;
}
