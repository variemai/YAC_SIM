#include <iostream>
#include <string>
using namespace std;

int main(void){
    int i,j,k;
    int x_way_frame = 21;
    int y_way_frame = 60;
    int x_way_title_flame = x_way_frame;
    int x_index_frame = 7;
    int no_ways = 4;
    string Valid = " Valid |";
    string Data = " Data |";
    int tag_size = 7;
    string Tag = " Tag";
    string Index = " Index";
    int index_size = 6;
    if (tag_size > 5 ){
        int offset = tag_size - Tag.length() - 1;
        for(i=0; i<offset; i++){
            Tag.append(" ");
        }
    }
    Tag.append(" |");
    if ( index_size > 8 ){
        int offset = index_size - Index.length() - 1;
        for(i=0; i<offset; i++){
            Index.append(" ");
        }
    }
    Index.append(" |");
    x_way_frame = Valid.length()+Data.length()+Tag.length();
    for(k=0; k<no_ways; k++){
        if( k == 0 ){
            for(j=0; j<x_index_frame; j++){
                printf(" ");
            }
        }
        for( i=0; i < x_way_frame; i++){
            printf("-");
        }
    }
    printf("\n");
    for(j=0; j<x_index_frame; j++){
        printf(" ");
    }
    printf("|");
    for(k=0; k<no_ways; k++){
        for(i=0; i<(x_way_frame-5)/2; i++){
            printf(" ");
        }
        printf("Way%d",k);
        for(i=0; i<(x_way_frame-5)/2; i++){
            printf(" ");
        }
        printf("|");
    }
    printf("\n");
    for(k=0; k<no_ways; k++){
        if ( k ==0 ){
            for(j=0; j<x_index_frame; j++){
                printf(" ");
            }
        }
        for(i=0; i < x_way_frame; i++){
            printf("-");
        }
    }
    printf("\n");
    cout << Index;
    for(k=0; k<no_ways; k++){
        cout << Valid;
        cout << Data;
        cout << Tag ;
    }
    // for(i=0; i<x_way_frame-17; i++){
    //     printf(" ");
    // }
    printf("\n");
    printf("%d, %d, %d\n",Valid.length(),Data.length(),Tag.length());
    return 0;
}
