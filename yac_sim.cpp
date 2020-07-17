/*****************************************************************************\
* YAC Sim - Yet Another Cache Simulator, created for the purposes of CS-225
* course CSD, University of Crete. YAC Sim emulates a cache  with user defined
* attributes, simulates accesses via user input and measures the total hit/miss
* ratio.
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
#include <string.h>
#include <string>
#include "utils.h"

#define MAX_FILENAME 256

using namespace std;

unsigned long word_size = 0;
unsigned x_way_frame = 0;
unsigned x_index_frame = 0;
unsigned x_tag_frame = 0;


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
    unsigned long asso = 1;
}cache_char;

/*************************Function Declarations*******************************/
unsigned LRU_policy(vector<entry> &cache,cache_char*);
void set_cache_specs(unsigned long,unsigned long,unsigned long,unsigned long,
        cache_char*);
void print_bin_index(unsigned long, unsigned);
unsigned long return_word(unsigned long , unsigned, unsigned long, unsigned );
void clear_contents(vector<entry> &cache, cache_char*);
void cache_access(vector<entry>& cache, unsigned long address,
	cache_char* specs, cache_prof* prof_info);
/*************************Function Definitions********************************/

/*Not used as function call*/
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
            std::cout << " MIN_LRU =  " << min_lru << " ";
        }
    }
    return way;
}

void set_cache_specs(unsigned long cache_size, unsigned long block_size,
        unsigned long memory_size,unsigned long asso, cache_char* characteristics)
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
    std::cout<<"Tag Size: "<<specs->tag_size<<"\nIndex size: "<<specs->index_size
        <<endl<<"Tmp: "<<specs->tmp<<"\nTmp_tag: "<<specs->tmp_tag<<endl;
    std::cout << "Tag Shift: " << specs->tag_shift<<endl;
    std::cout << "Number of Sets: "<< specs->no_set << endl;
}

void cache_access(vector<entry> &cache, unsigned long address,
       cache_char* specs, cache_prof* prof_info)
{
    unsigned long index, tag, old_address;
    unsigned short done, min_lru, max_lru;
    unsigned way;
	std::cout << "ADDR: " << address << " ";
	index = address & specs->tmp;
	index = index >> specs->block_offset;
	tag = address & specs->tmp_tag;
	tag = tag >> specs->tag_shift;
	prof_info->check++;
    done = 0;
    min_lru = 0;
    way = 0;
    for(unsigned long i=0; i<specs->asso; i++){
	    if (cache[index].valid[i] == 1 && cache[index].tag[i] == tag) {
		    prof_info->hit++;
            /*Update the LRU info*/
            cache[index].LRU[i]++;
		    std::cout << "HIT with index: ";
		    print_bin_index(index,specs->index_size);
            if(specs->asso > 1) {
                std::cout << " WAY: " << i;
                std::cout << " LRU: " << cache[index].LRU[i] ;
            }
            done = 1;
	    }
    }
    if(!done){
        prof_info->miss++;
        /*Miss-first search for an invalid position in the way*/
        for(unsigned long i=0; i<specs->asso; i++){
            if(cache[index].valid[i] == 0){
                cache[index].tag[i] = tag;
                cache[index].valid[i] = 1;
                cache[index].LRU[i] = 1;
                std::cout << "MISS, with index: ";
                print_bin_index(index,specs->index_size);
                if(specs->asso > 1){
                    std::cout << " WAY: " << i;
                    std::cout << " LRU: " << cache[index].LRU[i] ;
                }
                done = 1;
                break;
            }
        }
        if(!done && specs->asso > 1){
            /*LRU replacement algorithm*/
            min_lru = cache[index].LRU[0];
            max_lru = cache[index].LRU[0];
            for(unsigned long i=0; i<specs->asso; i++){
                if(cache[index].LRU[i] < min_lru ){
                    min_lru = cache[index].LRU[i];
                    way = i;
                }
                if(cache[index].LRU[i] > max_lru){
                    max_lru = cache[index].LRU[i];
                }
            }
            old_address=return_word(cache[index].tag[way],specs->tag_shift,
                    index,specs->block_offset);
			std::cout <<"MISS, Replace address: "<< old_address;
			std::cout << "  with index: ";
			print_bin_index(index,specs->index_size);
            std::cout << " WAY: " << way;
            std::cout << " LRU: " << cache[index].LRU[way] ;
            cache[index].tag[way] = tag;
//            cache[index].LRU[way] = 1;
            cache[index].LRU[way] = max_lru+1;

        }
        /*Directed mapped cache access no need for LRU*/
        else if(!done && specs->asso == 1){
			old_address = return_word(cache[index].tag[0],specs->tag_shift,
                    index,specs->block_offset);
			std::cout <<"MISS, replace address: "<< old_address;
			std::cout << "  with index: ";
			print_bin_index(index,specs->index_size);
		    cache[index].tag[0] = tag;
        }
    }
	std::cout << endl;
}


void print_bin_index(unsigned long index, unsigned size)
{
    bitset<32> bits = {index};
	for(unsigned i=size-1; i>=0; i--){
		std::cout << bits[i];
		if(i==0) break;
	}
}

void clear_contents(vector<entry> &cache, cache_char* specs, cache_prof *prof_info){
    for(unsigned i=0; i<specs->no_set; i++) {
        for(unsigned long j=0; j<specs->asso; j++){
            cache[i].valid[j] = 0;
            cache[i].tag[j] = 0; //not necessary
            if(specs->asso > 1){
                cache[i].LRU[j] = 0;
            }
        }
    }
    prof_info->check = 0;
    prof_info->hit = 0;
    prof_info->miss = 0;
    std::cout << "Flushed Cache and Reseted Statistics" << endl;
}
/* Print the frame of the cache  for each cache line and way*/
void _print_frame(cache_char* specs){
    unsigned i,j,k;
    unsigned no_ways = specs->asso;
    string Valid = " V |";
    string Dirty = " D |";
    string Data = " Data |";
    unsigned tag_size = specs->tag_size;
    string Tag = "";
    string Index = "Index";
    unsigned index_size = specs->index_size;
    unsigned l_side, r_side;
    /*Prettier tag*/
    if (tag_size > 5){
        if ( (tag_size-3) % 2 == 0 ){
            unsigned tmp = (tag_size-3)/2;
            for(i=0; i<tmp; i++){
                Tag.append(" ");
            }
            Tag.append("Tag");
            for(i=0; i<tmp; i++){
                Tag.append(" ");
            }
        }
        else{
            unsigned tmp = (tag_size-3)/2;
            for(i=0; i<tmp+1; i++){
                Tag.append(" ");
            }
            Tag.append("Tag");
            for(i=0; i<tmp; i++){
                Tag.append(" ");
            }
        }
        Tag.append(" |");
    }
    else{
        Tag.append(" Tag |");
    }
    if ( index_size > 7 ){
        unsigned offset = index_size - Index.length() - 1;
        for(i=0; i<offset; i++){
            Index.append(" ");
        }
    }
    Index.append(" |");
    x_index_frame = Index.length();
    x_way_frame = Valid.length()+Dirty.length()+Data.length()+Tag.length();
	x_tag_frame = Tag.length();
    if (x_way_frame % 2 == 0){
        l_side = (x_way_frame - 6 ) / 2;
        r_side = l_side;
    }
    else{
        l_side = 1+(x_way_frame - 6) / 2;
        r_side =(x_way_frame - 6 ) / 2;
    }
    for(k=0; k<no_ways; k++){
        if( k == 0 ){
            for(j=0; j<x_index_frame - 1; j++){
                printf(" ");
            }
        }
        for( i=0; i < x_way_frame; i++){
            printf("-");
        }
        if( k == no_ways -1 ){
            printf("-");
        }
    }
    printf("\n");
    for(j=0; j<x_index_frame - 1; j++){
        printf(" ");
    }
    printf("|");
    for(k=0; k<no_ways; k++){
        for(i=0; i<l_side; i++){
            printf(" ");
        }
        printf("Way %d",k);
        for(i=0; i<r_side; i++){
            printf(" ");
        }
        printf("|");
    }
    printf("\n");
    for(k=0; k<no_ways; k++){
        if ( k ==0 ){
            for(j=0; j<x_index_frame -1; j++){
                printf(" ");
            }
        }
        for(i=0; i < x_way_frame; i++){
            printf("-");
        }
        if( k == no_ways -1 ){
            printf("-");
        }
    }
    printf("\n");
    std::cout << Index;
    for(k=0; k<no_ways; k++){
        std::cout << Valid;
        std::cout << Dirty;
        std::cout << Data;
        std::cout << Tag ;
    }
    printf("\n");
    for(i=0; i<no_ways; i++){
        if(i==0){
            for(j=0; j<x_index_frame+x_way_frame; j++){
                printf("-");
            }
        }
        else{
            for(j=0; j<x_way_frame; j++){
                printf("-");
            }
        }
    }
    printf("\n");
}

void display_contents(vector<entry> &cache, cache_char* specs)
{
	unsigned j;
	int tmp, tmp_valid, tmp_tag;
    _print_frame(specs);
    for(unsigned i=0; i<2*specs->no_set; i++) {
        for(unsigned long k=0; k<specs->asso; k++){
		bitset<32> bits = {cache[i/2].tag[k]};
		bitset<32> bits_of_index = { i/2 };
        //std::cout << " Way: " << j;
        unsigned max;
        if(k == 0){
            max = x_index_frame+x_way_frame;
        }
        else{
            max = x_way_frame;
        }
		
        if( i%2 == 0){
				tmp = specs->index_size-1;
				tmp_tag = specs->tag_size-1;
				tmp_valid = 2;
                for(j=0; j<max-1; j++){
                    if (k ==0 ){
                        if(j == x_index_frame - 1 ){
                            printf("|");
                        }
                        else if( j == x_index_frame + 4 -1  ){
                            printf("|");
                        }
                        else if ( j == x_index_frame + 4 + 4 - 1 ){
                            printf("|");
                        }
                        else if ( j == x_index_frame + 4 + 4 + 7 -1 ){
                            printf("|");
                        }
                        else{
                            /*Print the index bits*/
                            if(j < x_index_frame - 1){
								if (tmp >= 0) {
									cout << bits_of_index[tmp] ;
									tmp--;
								}
								else {
									cout << " ";
								}
                            }
							/* Print the Valid bit and some spaces*/
							else if ( j < x_index_frame + 4 - 1 ) {
								if (tmp_valid == 1) {
									cout << cache[i/2].valid[k];
								}
								else {
									cout << " ";
								}
								tmp_valid--;
							}

							else if (( j > x_index_frame + 4 + 4 + 7 - 1) && (j < x_index_frame+ 4 + 4 + 7 + x_tag_frame - 1) ){
								//cout << " ";
								if (tmp_tag >= 0) {
									cout << bits[tmp_tag];
									tmp_tag--;
								}
								else {
									cout << " ";
								}
							}
							/* Print the Spaces for Dirty bit and Data
							 * Since we dont have evictions or simulating
							 * actual data those two are empty
							 */
							else {
								cout << " ";
							}
                        }

                    }
                    else{
                        if ( j == 4 -1){
                            printf("|");
                        }
                        else if ( j == 4 + 4 - 1 ){
                            printf("|");
                        }
                        else if ( j == 4 + 4 + 7 -1){
                            printf("|");
                        }
                        else{
							/* Print the Valid bit and some spaces*/
							if (j < 4 - 1) {
								if (tmp_valid == 1) {
									cout << cache[i / 2].valid[k];
								}
								else {
									cout << " ";
								}
								tmp_valid--;
							}

							else if ((j > 4 + 4 + 7 - 1) && (j < 4 + 4 + 7 + x_tag_frame - 1)) {
								//cout << " ";
								if (tmp_tag >= 0) {
									cout << bits[tmp_tag];
									tmp_tag--;
								}
								else {
									cout << " ";
								}
							}
							/* Print the Spaces for Dirty bit and Data
							 * Since we dont have evictions or simulating
							 * actual data those two are empty
							 */
							else {
								cout << " ";
							}
                        }
                    }

                }
                printf("|");
		}
            else{
                for(j=0; j<max; j++){
                    printf("-");
                }
            }
        }
        printf("\n");
    }

		/*std::cout << " Valid: " << cache[i].valid[j];
        if(cache[i].valid[j]){
            std::cout << ", Tag: ";
            for(unsigned k=specs->tag_size-1; k>=0; k--){
                std::cout << bits[k] ;
                if (k==0) break;
            }
            if(specs->asso > 1){
                std::cout << " LRU: " << cache[i].LRU[j];
            }
            std::cout << " First Addr in Block: " << return_word(cache[i].tag[j],
                                       specs->tag_shift,i,specs->block_offset);
        }
		std::cout << endl;

        }*/
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
	std::cout << "************* Cache Simulation Statistics *********"<< endl;
	printf ("*             Total ACCESSES : %18d *\n",prof_info->check);
	printf ("*             Number of HITS : %18d *\n",prof_info->hit);
	printf ("*             Number of MISSES : %16d *\n",prof_info->miss);
	hitrate =100* (float(prof_info->hit) / float(prof_info->check));
	printf ("*             HIT RATE : %23.2f%% *\n",hitrate);
	std::cout << "***************************************************"<< endl;
}



int main(int argc, char* argv[])
{
/******************************** Variables declarations **********************/
	string filename,fname;
    unsigned long asso, val;
	unsigned long cache_size, block_size, memory_size, address;
	string str;
	ifstream infile,file;
	vector<entry> cache;
	entry init_entry;
    cache_prof prof_info;
    cache_char cache_specs;
    int alnum_flag;
    string::size_type i;
    locale loc;
    size_t found;
    char *token,*endptr;

/*********************************** Initialization ***************************/
    prof_info.check = 0;
    prof_info.hit = 0;
    prof_info.miss = 0;
    i = 0;
    asso = 0;
    cache_size = 0;
    block_size = 0;
    memory_size = 0;
    word_size = 0 ;
    std::cout << "\nYAC Simulator, Copyright (C) 2018 Ioannis Vardas\n";
    std::cout << "Contact vardas@ics.forth.gr\n\n";
    /*Print info about GPL licence*
    std::cout << "This program is under ";
    std::cout << "the terms of the GNU General Public License v3\nFor more info see";
    std::cout << ": <https://www.gnu.org/licenses/>" << endl << endl;
    */
    if( argc < 2 ){

        while( ( memory_size = get_sizeof_memory() ) == 0 ){
            std::cout << "Bad Input: Memory Size must be power of 2!\n" << endl;
        }

        while( ( word_size = get_sizeof_word() ) == 0 ){
            std::cout << "Bad Input: Word Size must be power of 2!\n" << endl;;
        }

        while( ( cache_size = get_sizeof_cache() ) == 0  ){
            std::cout << "Bad Input: Cache Size must be power of 2!\n" << endl;;
        }

        while( ( block_size = get_sizeof_cacheline() ) == 0 ){
            std::cout << "Bad Input: Cache Line Size must be power of 2!\n" << endl;;
        }

        while( ( asso = get_associativity() ) == 0 ){
            std::cout << "Bad Input: Associativity should be either 1,2,4,8 or 16"<<endl;
        }

    }
    else{
        std::cout << "Filename given: " << argv[1] << endl;
        infile.open(argv[1], ios::in);
        if (!infile){
            std::cout << "Error! File not found...\n";
            exit(-1);
        }
        while (getline(infile, str)){
            found = str.find("memsize");
            if ( found != string::npos ) {
                char *line = &(str[0]);
                std::cout << line << endl;
                token = strtok(line," ");
                if(token != NULL) {
                    token = strtok(NULL," ");
                    if(token == NULL ){
                        std::cout << "Bad Input: memsize found but no digits were found!" << endl;
                        while( ( val = get_sizeof_memory() ) == 0 ){
                            std::cout << "Bad Input: Memory Size must be power of 2!\n" << endl;
                        }
                        memory_size = val;
                        continue;
                    }
                    val = strtoul(token,&endptr,10);
                    if ((errno == ERANGE && (val == ULONG_MAX) )
                        || (errno != 0 && val == 0))  {
                        perror("memsize: bad input - strtol\n");
                        while( ( val = get_sizeof_memory() ) == 0 ){
                            std::cout << "Bad Input: Memory Size must be power of 2!\n" << endl;
                        }
                    }
                    if (endptr == token) {
                        fprintf(stderr, "Bad Input: memsize found but no digits were found!\n");
                         while( ( val = get_sizeof_memory() ) == 0 ){
                            std::cout << "Bad Input: Memory Size must be power of 2!\n" << endl;
                        }
                    }
                }
                else{
                    std::cout << "Bad Input: memsize found but no digits were found!" << endl;
                    while( ( val = get_sizeof_memory() ) == 0 ){
                        std::cout << "Bad Input: Memory Size must be power of 2!\n" << endl;
                    }
                }
                memory_size = val;
                continue;
            }
            found = str.find("wordsize");
            if ( found != string::npos ) {
                char *line = &(str[0]);
                std::cout << line << endl;
                token = strtok(line," ");
                if(token != NULL) {
                    token = strtok(NULL," ");
                    if(token == NULL ) {
                        std::cout << "Bad Input: wordsize found but no digits were found!" << endl;
                        while( ( val = get_sizeof_word() ) == 0 ){
                            std::cout << "Bad Input: Word Size must be power of 2!\n" << endl;
                        }
                        word_size = val;
                        continue;
                    }
                    val = strtoul(token,&endptr,10);
                    if ((errno == ERANGE && (val == ULONG_MAX) )
                        || (errno != 0 && val == 0))  {
                        perror("wordsize: bad input - strtol\n");
                        while( ( val = get_sizeof_word() ) == 0 ){
                            std::cout << "Bad Input: Word Size must be power of 2!\n" << endl;
                        }
                    }
                    if (endptr == token) {
                        fprintf(stderr, "Bad Input: wordsize found but no digits were found!\n");
                        while( ( val = get_sizeof_word() ) == 0 ){
                            std::cout << "Bad Input: Word Size must be power of 2!\n" << endl;
                        }
                    }
                }
                else{
                    std::cout << "Bad Input: wordsize found but no digits were found!" << endl;
                    while( ( val = get_sizeof_word() ) == 0 ){
                        std::cout << "Bad Input: Word Size must be power of 2!\n" << endl;
                    }
                }
                word_size = val;
                continue;
            }
            found = str.find("cachesize");
            if ( found != string::npos ) {
                char *line = &(str[0]);
                std::cout << line << endl;
                token = strtok(line," ");
                if(token != NULL) {
                    token = strtok(NULL," ");
                    if(token == NULL ){
                        std::cout << "Bad Input: cachesize found but no digits were found!" << endl;
                        while( ( val = get_sizeof_cache()) == 0 ){
                            std::cout << "Bad Input: Cache Size must be power of 2!\n" << endl;
                        }
                        cache_size = val;
                        continue;
                    }
                    val = strtoul(token,&endptr,10);
                    if ((errno == ERANGE && (val == ULONG_MAX) )
                        || (errno != 0 && val == 0))  {
                        perror("cachesize: bad input - strtol\n");
                        while( ( val = get_sizeof_cache() ) == 0 ){
                            std::cout << "Bad Input: Cache Size must be power of 2!\n" << endl;
                        }
                    }
                    if (endptr == token) {
                        fprintf(stderr, "Bad Input: cachesize found but no digits were found!\n");
                        while( ( val = get_sizeof_cache() ) == 0  ){
                            std::cout << "Bad Input: Cache Size must be power of 2!\n" << endl;
                        }
                    }
                }
                else{
                    std::cout << "Bad Input: cachesize found but no digits were found!" << endl;
                    while( ( val = get_sizeof_cache() ) == 0 ){
                        std::cout << "Bad Input: Cache Size must be power of 2!\n" << endl;
                    }
                }
                cache_size = val;
                continue;
            }
            found = str.find("linesize");
            if ( found != string::npos ) {
                char *line = &(str[0]);
                std::cout << line << endl;
                token = strtok(line," ");
                if(token != NULL) {
                    token = strtok(NULL," ");
                    if(token == NULL ){
                        std::cout << "Bad Input: linesize found but no digits were found!" << endl;
                        while( ( val = get_sizeof_cacheline() ) == 0 ){
                            std::cout << "Bad Input: Cache Line Size must be power of 2!\n" << endl;
                        }
                        block_size = val;
                        continue;
                    }
                    val = strtoul(token,&endptr,10);
                    if ((errno == ERANGE && (val == ULONG_MAX) )
                        || (errno != 0 && val == 0))  {
                        perror("linesize: bad input - strtol\n");
                        while( ( val = get_sizeof_cacheline() ) == 0 ){
                                std::cout << "Bad Input: Cache Line Size must be power of 2!\n" << endl;
                        }
                    }
                    if (endptr == token) {
                        fprintf(stderr, "Bad Input: linesize found but no digits were found!\n");
                        while( ( val = get_sizeof_cacheline() ) == 0 ){
                            std::cout << "Bad Input: Cache Line Size must be power of 2!\n" << endl;
                        }
                    }
                }
                else{
                    std::cout << "Bad Input: linesize found but no digits were found!" << endl;
                    while( ( val = get_sizeof_cacheline() ) == 0 ){
                        std::cout << "Bad Input: Cache Line Size must be power of 2!\n" << endl;
                    }
                }
                block_size = val;
                continue;
            }
            found = str.find("asso");
            if ( found != string::npos ) {
                char *line = &(str[0]);
                std::cout << line << endl;
                token = strtok(line," ");
                if(token != NULL) {
                    token = strtok(NULL," ");
                    if(token == NULL ){
                        std::cout << "Bad Input: associativity found but no digits were found!" << endl;
                        while( ( val = get_associativity() ) == 0 ){
                            std::cout << "Bad Input: Associativity must be power of 2!\n" << endl;
                        }
                        asso = val;
                        continue;
                    }
                    val = strtoul(token,&endptr,10);
                    if ((errno == ERANGE && (val == ULONG_MAX) )
                        || (errno != 0 && val == 0))  {
                        perror("associativity: bad input - strtol\n");
                        while( ( val = get_associativity() ) == 0 ){
                            std::cout << "Bad Input: Associativity must be power of 2!\n" << endl;
                        }
                    }
                    if (endptr == token) {
                        fprintf(stderr, "Bad Input: associativity found but no digits were found!\n");
                        while( ( val = get_associativity() ) == 0 ){
                            std::cout << "Bad Input: Associativity must be power of 2!\n" << endl;
                        }
                    }
                }
                else{
                    std::cout << "Bad Input: associativity found but no digits were found!" << endl;
                    while( ( val = get_associativity() ) == 0 ){
                        std::cout << "Bad Input: Associativity must be power of 2!\n" << endl;
                    }
                }
                asso = val;
                continue;
            }
            infile.close();
        }
        if(memory_size == 0){
            while( ( memory_size = get_sizeof_memory() ) == 0 ){
                std::cout << "Bad Input: Memory Size must be power of 2!\n" << endl;
            }
        }
        if(word_size == 0 ){
            while( ( word_size = get_sizeof_word() ) == 0 ){
                std::cout << "Bad Input: Word Size must be power of 2!\n" << endl;
            }
        }
        if(cache_size == 0){
            while( ( cache_size = get_sizeof_cache()) == 0 ){
                std::cout << "Bad Input: Cache Size must be power of 2!\n" << endl;
            }
        }
        if(block_size == 0){
            while( ( block_size = get_sizeof_cacheline() ) == 0 ){
                std::cout << "Bad Input: Cache Line Size must be power of 2!\n" << endl;
            }
        }
        if(asso == 0 ){
            while( ( asso = get_associativity() ) == 0 ){
                std::cout << "Bad Input: Associativity must be power of 2!\n" << endl;
            }
        }
    }
    /* Input from user is complete and correct to this point */

    set_cache_specs(cache_size,block_size,memory_size,asso, &cache_specs);
    for(unsigned long i=0; i<asso; i++){
        init_entry.tag[i] = 0;
        init_entry.valid[i] = 0;
        init_entry.LRU[i] = 0;
    }
	for(unsigned long i=0; i<cache_specs.no_set; i++){
		cache.push_back(init_entry);
	}

    print_specs(&cache_specs);

/***************************Simulation Starting********************************/
    std::cout << "Insert an address or a valid command, type \"help\" for a list of available commands"<<endl;
	while (true){
        getline(cin,str);
        if(str.length() > 0 ){
            if(str.compare("help") == 0){
                printf("\texit:\t\t\tPrints the Statistics and exits YAC Simulator\n");
                printf("\tsource <filename>:\tRead addresses from a file\n");
                printf("\tdisplay:\t\tPrints the contents of the Cache and the\
 Statistics\n");
                printf("\tflush:\t\t\tClears the contents of the Cache and resets\
 statistics\n");
                printf("\thelp:\t\t\tDisplays this message\n");
                continue;
            }
            if(str.compare("exit") == 0 ) {
                print_results(&prof_info);
                std::cout << "Exit YAC Simulator!" << endl;
                break;
            }
            /*Display contents of cache*/
            if(str.compare("display") == 0 ){
                display_contents(cache,&cache_specs);
                std::cout << endl;
                print_results(&prof_info);
                std::cout << endl;
                std::cout << "Insert an address or a valid command, type \"help\" for a list of available commands"<<endl;
                continue;
            }
            if(str.compare("flush") == 0 ){
                clear_contents(cache,&cache_specs,&prof_info);
                std::cout << endl;
                std::cout << "Insert an address or a valid command, type \"help\" for a list of available commands"<<endl;
                continue;
            }
            /*Input from file*/
            if(str.length() > 6 ){
                string token1 = str.substr(0,6);
                string token2 = str.substr(7,string::npos);
                if(token1.compare("source") == 0 ){
                    fname.assign(token2);
                    if(fname.size() > MAX_FILENAME){
                        std::cout << "Filename exceeded max size\nExiting..." << endl;
                        exit(-1);
                    }
                    std::cout << "Filename given: " << fname << endl;
                    file.open(fname, ios::in);
                    if (!file){
                        std::cout << "Error! File not found...\n";
                        exit(-1);
                    }
                    while (getline(file, str)){
                        address = std::stoul(str);
                        cache_access(cache, address, &cache_specs,&prof_info);
                    }
                    file.close();
                    std::cout << endl;
                    std::cout << "Insert an address or a valid command, type \"help\" for a list of available commands"<<endl;
                    continue;
                }
                else{
                    alnum_flag = 1;
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
                std::cout << "Wrong command or Address not alphanumeric"<< endl;
                std::cout << endl;
                std::cout << "Insert an address or a valid command, type \"help\" for a list of available commands"<<endl;
                str.clear();
                alnum_flag = 0;
                continue;
            }
            address = stoul(str);
            cache_access(cache, address, &cache_specs,&prof_info);
            std::cout << endl;
            std::cout << "Insert an address or a valid command, type \"help\" for a list of available commands"<<endl;
        }
	}
	return 0;
}
